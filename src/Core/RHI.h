#pragma once

struct RHIWindowInfo
{
	HWND m_Window {nullptr};
	uint32 m_Width {1920};
	uint32 m_Height {1080};
	DXGI_RATIONAL m_RefreshRate {60, 1};
	bool m_Windowed {true};
};

struct RHIDescriptorSizes
{
	uint32 m_Rtv {0};
	uint32 m_Dsv {0};
	uint32 m_Cbvsrvuav {0};
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
	inline static ComPtr<ID3D12Device> m_Device {nullptr};
	inline static ComPtr<IDXGIFactory4> m_Factory {nullptr};
	inline static ComPtr<ID3D12Fence> m_Fence {nullptr};
	inline static ComPtr<IDXGISwapChain3> m_SwapChain {nullptr};

	inline static ComPtr<ID3D12CommandQueue> m_CommandQueue {nullptr};
	inline static ComPtr<ID3D12CommandAllocator> m_CommandListAllocator {nullptr};
	inline static ComPtr<ID3D12GraphicsCommandList> m_CommandList {nullptr};

	inline static RHIWindowInfo m_WindowInfo {};
	inline static RHIDescriptorSizes m_DescriptorSizes {};

	inline static constexpr uint32 s_SwapChainBufferCount {2};
	inline static uint32 m_CurrentBackBuffer {0};

	inline static uint32 m_Msaa4XQuality {0};

	inline static DXGI_FORMAT m_BackBufferFormat {DXGI_FORMAT_R8G8B8A8_UNORM};
};
