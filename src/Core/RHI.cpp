// Copyright (c) 2023 Sebastian Ploch
#include "RHI.h"

void RHI::Initialise(HWND Window)
{
	LOG("-- Initialise RHI --");
	assert(Window && "HWND has to be created and valid before RHI initialisation");
	m_WindowInfo.m_Window = Window;

	CreateDevice();
	CreateFenceAndDescriptorSizes();
	CheckMSAAQualitySupport();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateDescriptorHeaps();
	CreateRTVsToSwapChain();
	CreateDepthStencilBuffer();
	CreateDepthStencilRTV();
	SetViewport();

	if (m_CommandList)
	{
		m_CommandList->Close();
	}
	LOG("-- RHI Initialised --");
}

void RHI::ResizeWindow(uint32 Width, uint32 Height)
{
	m_WindowInfo.m_Width = Width;
	m_WindowInfo.m_Height = Height;
	LOG("Window resized to Width:{} Height:{}", m_WindowInfo.m_Width, m_WindowInfo.m_Height);
	// TODO: Add call to resize swapchain
}

D3D12_CPU_DESCRIPTOR_HANDLE RHI::GetCurrentBackBufferViewHandle()
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_DescriptorHeaps.m_Rtv->GetCPUDescriptorHandleForHeapStart(),
		static_cast<int32>(m_CurrentBackBuffer),
		m_DescriptorSizes.m_Rtv
	);
}

D3D12_CPU_DESCRIPTOR_HANDLE RHI::GetDepthStencilViewHandle()
{
	return m_DescriptorHeaps.m_Dsv->GetCPUDescriptorHandleForHeapStart();
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
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_Factory)));

	// Create Hardware m_Device
	const HRESULT createDeviceResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));

	// Try to fallback to WARP device
	if (FAILED(createDeviceResult))
	{
		ComPtr<IDXGIAdapter> adapter;
		ThrowIfFailed(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
		ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
	}
}

void RHI::CreateFenceAndDescriptorSizes()
{
	ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
	m_DescriptorSizes.m_Rtv = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DescriptorSizes.m_Dsv = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_DescriptorSizes.m_CbvSrvUav = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void RHI::CreateCommandQueueAndList()
{
	constexpr D3D12_COMMAND_QUEUE_DESC cmdQueueDesc {
		.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
	};

	ThrowIfFailed(m_Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_CommandQueue)));
	ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandListAllocator)));
	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandListAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));
}

void RHI::CreateSwapChain()
{
	// TODO: Add resize support

	// TODO: this needs to be properly hooked with MSAA properties
	constexpr DXGI_SAMPLE_DESC swapChainSampleDesc = {
		.Count = 1,
		.Quality = 0
	};

	const DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
		.Width = m_WindowInfo.m_Width,
		.Height = m_WindowInfo.m_Height,
		.Format = s_BackBufferFormat,
		.SampleDesc = swapChainSampleDesc,
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = s_SwapChainBufferCount,
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_IGNORE,
		.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};

	const DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenSwapChainDesc = {
		.Windowed = m_WindowInfo.m_Windowed
	};

	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(m_Factory->CreateSwapChainForHwnd(
		m_CommandQueue.Get(),
		m_WindowInfo.m_Window,
		&swapChainDesc,
		&fullscreenSwapChainDesc,
		nullptr,
		swapChain.GetAddressOf()
	));

	ThrowIfFailed(swapChain.As(&m_SwapChain));
}

void RHI::CreateDescriptorHeaps()
{
	// RTV
	constexpr D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		.NumDescriptors = s_SwapChainBufferCount,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 0
	};

	ThrowIfFailed(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_DescriptorHeaps.m_Rtv)));

	// DSV
	constexpr D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		.NumDescriptors = 1,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 0
	};

	ThrowIfFailed(m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DescriptorHeaps.m_Dsv)));
}

void RHI::CreateRTVsToSwapChain()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle {m_DescriptorHeaps.m_Rtv->GetCPUDescriptorHandleForHeapStart()};
	for (uint32 i = 0; i < s_SwapChainBufferCount; ++i)
	{
		// Get buffer
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));

		// Create view into the buffer
		m_Device->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);

		// Bump heap
		rtvHeapHandle.Offset(1, m_DescriptorSizes.m_Rtv);
	}
}

void RHI::CreateDepthStencilBuffer()
{
	// TODO: this needs to be properly hooked with MSAA properties
	constexpr DXGI_SAMPLE_DESC depthStencilSampleDesc = {
		.Count = 1,
		.Quality = 0
	};

	const D3D12_RESOURCE_DESC depthStencilDesc = {
		.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		.Alignment = 0,
		.Width = m_WindowInfo.m_Width,
		.Height = m_WindowInfo.m_Height,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = s_DepthStencilFormat,
		.SampleDesc = depthStencilSampleDesc,
		.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	};

	constexpr D3D12_DEPTH_STENCIL_VALUE depthStencilValue = {
		.Depth = 1.f,
		.Stencil = 0
	};

	const D3D12_CLEAR_VALUE optimisedClear = {
		.Format = s_DepthStencilFormat,
		.DepthStencil = depthStencilValue
	};

	const CD3DX12_HEAP_PROPERTIES heapProperties {D3D12_HEAP_TYPE_DEFAULT};
	ThrowIfFailed(m_Device->CreateCommittedResource(&heapProperties,
	                                                D3D12_HEAP_FLAG_NONE,
	                                                &depthStencilDesc,
	                                                D3D12_RESOURCE_STATE_COMMON,
	                                                &optimisedClear,
	                                                IID_PPV_ARGS(&m_DepthStencilBuffer)));
}

void RHI::CreateDepthStencilRTV()
{
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), nullptr, GetDepthStencilViewHandle());

	const CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(),
	                                                                                 D3D12_RESOURCE_STATE_COMMON,
	                                                                                 D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_CommandList->ResourceBarrier(1, &transition);
}

void RHI::SetViewport()
{
	const D3D12_VIEWPORT viewport = {
		.TopLeftX = 0.f,
		.TopLeftY = 0.f,
		.Width = static_cast<float>(m_WindowInfo.m_Width),
		.Height = static_cast<float>(m_WindowInfo.m_Height),
		.MinDepth = 0.f,
		.MaxDepth = 1.f
	};

	m_CommandList->RSSetViewports(1, &viewport);
}

void RHI::CheckMSAAQualitySupport()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels {
		s_BackBufferFormat,
		4,
		D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
		0
	};
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(qualityLevels)));

	m_Msaa4XQuality = qualityLevels.NumQualityLevels;
	assert(m_Msaa4XQuality > 0 && "Unexcpted MSAA quality level");
}
