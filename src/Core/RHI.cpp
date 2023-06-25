#include "RHI.h"

void RHI::Initialise()
{
	CreateDevice();
	CreateFenceAndDescriptorSizes();
	CheckMSAAQualitySupport();
	CreateCommandQueueAndList();
	CreateSwapChain();
}

void RHI::ResizeWindow(uint32 Width, uint32 Height)
{
	WindowInfo.Width = Width;
	WindowInfo.Height = Height;
}

void RHI::CreateDevice()
{
	uint32 dxgiFactoryFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	{
		// D3D12 Debug Layer
		ComPtr<ID3D12Debug> debug;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
		debug->EnableDebugLayer();

#ifndef __MINGW32__
		// DXGI Debug, taken from https://github.com/walbourn/directx-vs-templates/blob/main/d3d12game_win32_dr/DeviceResources.cpp#L105
		dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

		ComPtr<IDXGIInfoQueue> infoQueue;
		ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&infoQueue)));
		infoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

		DXGI_INFO_QUEUE_MESSAGE_ID hide[] = {80};
		// IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides.
		DXGI_INFO_QUEUE_FILTER filter = {
			.DenyList = {
				.NumIDs = static_cast<UINT>(std::size(hide)),
				.pIDList = hide
			}
		};
		infoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
#endif // __MINGW32__
	}
#endif // defined(DEBUG) || defined(_DEBUG)

	// Create DXGI Factory
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&Factory)));

	// Create Hardware Device
	const HRESULT createDeviceResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device));

	// Try to fallback to WARP device
	if (FAILED(createDeviceResult))
	{
		ComPtr<IDXGIAdapter> adapter;
		ThrowIfFailed(Factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
		ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device)));
	}
}

void RHI::CreateFenceAndDescriptorSizes()
{
	ThrowIfFailed(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
	DescriptorSizes.RTV = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DescriptorSizes.DSV = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	DescriptorSizes.CBVSRVUAV = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void RHI::CreateCommandQueueAndList()
{
	constexpr D3D12_COMMAND_QUEUE_DESC cmdQueueDesc {
		.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
	};

	ThrowIfFailed(Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&CommandQueue)));
	ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandListAllocator)));
	ThrowIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandListAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList)));

	CommandList->Close();
}

void RHI::CreateSwapChain()
{
	constexpr DXGI_SAMPLE_DESC swapChainSampleDesc = {
		.Count = 1,
		.Quality = 0
	};

	const DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
		.Width = WindowInfo.Width,
		.Height = WindowInfo.Height,
		.Format = BackBufferFormat,
		.SampleDesc = swapChainSampleDesc,
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = SwapChainBufferCount,
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_IGNORE,
		.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};

	const DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenSwapChainDesc = {
		.Windowed = WindowInfo.Windowed
	};

	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(Factory->CreateSwapChainForHwnd(
		CommandQueue.Get(),
		WindowInfo.Window,
		&swapChainDesc,
		&fullscreenSwapChainDesc,
		nullptr,
		swapChain.GetAddressOf()
	));

	ThrowIfFailed(swapChain.As(&SwapChain));
}

void RHI::CheckMSAAQualitySupport()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels {
		BackBufferFormat,
		4,
		D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
		0
	};
	ThrowIfFailed(Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(qualityLevels)));

	MSAA4XQuality = qualityLevels.NumQualityLevels;
	assert(MSAA4XQuality > 0 && "Unexcpted MSAA quality level");
}
