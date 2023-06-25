#pragma once

class RHI
{
public:
	static void Initialise();

private:
	static void CreateDevice();

public:
	inline static ComPtr<ID3D12Device> Device {nullptr};
	inline static ComPtr<IDXGIFactory6> Factory {nullptr};
};
