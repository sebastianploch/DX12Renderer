#pragma once

// Standard Library
#include <format>
#include <vector>
#include <memory>

// DX12
#include "directx/d3d12.h"
#include "directx/d3dx12.h"

// Windows
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl.h>
using Microsoft::WRL::ComPtr;