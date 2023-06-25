#include "RHI.h"

void RHI::Initialise()
{
	CreateDevice();
	CreateFenceAndDescriptorSizes();
}

void RHI::CreateDevice()
{
#if defined(DEBUG) || defined(_DEBUG)
	// D3D12 Debug Layer
	{
		ComPtr<ID3D12Debug> debug;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
		debug->EnableDebugLayer();
	}
#endif // defined(DEBUG) || defined(_DEBUG)

	// Create DXGI Factory
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&Factory)));

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
