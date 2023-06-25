#pragma once

// Standard Library
#include <string>
#include <string_view>
#include <format>
#include <vector>
#include <memory>
#include <functional>

// DX12
#include "dxgi1_6.h"
#include "directx/d3d12.h"
#include "directx/d3dx12.h"

// Windows
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl.h>
using Microsoft::WRL::ComPtr;

// Application
#include "Utility/Assert.h"