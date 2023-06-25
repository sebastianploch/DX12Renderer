#pragma once

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

private:
	static void CreateDevice();
	static void CreateFenceAndDescriptorSizes();
	static void CreateCommandQueueAndList();
	static void CheckMSAAQualitySupport();

public:
	inline static ComPtr<ID3D12Device> Device {nullptr};
	inline static ComPtr<IDXGIFactory6> Factory {nullptr};
	inline static ComPtr<ID3D12Fence> Fence {nullptr};

	inline static ComPtr<ID3D12CommandQueue> CommandQueue {nullptr};
	inline static ComPtr<ID3D12CommandAllocator> CommandListAllocator {nullptr};
	inline static ComPtr<ID3D12GraphicsCommandList> CommandList {nullptr};

	inline static RHIDescriptorSizes DescriptorSizes {};
	inline static uint32 MSAA4XQuality {0};

	inline static DXGI_FORMAT BackBufferFormat {DXGI_FORMAT_R32G32B32A32_FLOAT};
};
