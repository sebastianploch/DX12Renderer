#pragma once

struct RHIWindowInfo
{
	HWND Window {nullptr};
	uint32 Width {1920};
	uint32 Height {1080};
	DXGI_RATIONAL RefreshRate {60, 1};
	bool Windowed {true};
};

struct RHIDescriptorSizes
{
	uint32 RTV {0};
	uint32 DSV {0};
	uint32 CBVSRVUAV {0};
};

class RHI
{
public:
	static void Initialise();

	static void ResizeWindow(uint32 Width, uint32 Height);

private:
	static void CreateDevice();
	static void CreateFenceAndDescriptorSizes();
	static void CreateCommandQueueAndList();
	static void CreateSwapChain();
	static void CheckMSAAQualitySupport();

public:
	inline static ComPtr<ID3D12Device> Device {nullptr};
	inline static ComPtr<IDXGIFactory4> Factory {nullptr};
	inline static ComPtr<ID3D12Fence> Fence {nullptr};
	inline static ComPtr<IDXGISwapChain3> SwapChain {nullptr};

	inline static ComPtr<ID3D12CommandQueue> CommandQueue {nullptr};
	inline static ComPtr<ID3D12CommandAllocator> CommandListAllocator {nullptr};
	inline static ComPtr<ID3D12GraphicsCommandList> CommandList {nullptr};

	inline static RHIWindowInfo WindowInfo {};
	inline static RHIDescriptorSizes DescriptorSizes {};
	inline static uint32 SwapChainBufferCount {2};
	inline static uint32 MSAA4XQuality {0};

	inline static DXGI_FORMAT BackBufferFormat {DXGI_FORMAT_R8G8B8A8_UNORM};
};
