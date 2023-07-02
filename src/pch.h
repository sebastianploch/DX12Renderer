#pragma once

// Standard Library
#include <cassert>
#include <string>
#include <string_view>
#include <format>
#include <vector>
#include <memory>
#include <functional>

// DX12
#include <dxgi1_6.h>
#include <directx/dxgiformat.h>
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <dxguids/dxguids.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

// Windows
#include <SDKDDKVer.h>

// Taken from https://github.com/walbourn/directx-vs-templates/blob/main/d3d12game_win32_dr/pch.h
#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl.h>
using Microsoft::WRL::ComPtr;

// Application
#include "Utility/TypeDefs.h"
#include "Utility/Assert.h"
#include "Utility/Log.h"