#include "RHI.h"

void RHI::Initialise()
{
	HRESULT hr = S_OK;

	CreateDevice();
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
