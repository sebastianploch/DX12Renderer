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

public:
	inline static ComPtr<ID3D12Device> Device {nullptr};
	inline static ComPtr<IDXGIFactory6> Factory {nullptr};
	inline static ComPtr<ID3D12Fence> Fence {nullptr};

	inline static RHIDescriptorSizes DescriptorSizes {};
};
