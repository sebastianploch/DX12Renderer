add_executable(DX12Renderer WIN32)

# Enable Edit and Continue for MSVC compilers if supported.
if(MSVC)
  target_compile_options(DX12Renderer PUBLIC "/Zi")
  target_link_options(DX12Renderer PUBLIC "/INCREMENTAL")
endif()

# Additional compile flags
if (MSVC)
  target_compile_options(DX12Renderer PRIVATE "/W4")
else()
  target_compile_options(DX12Renderer PRIVATE "-Wall -Wextra -Wpedantic")
endif()

# Use UNICODE instead of ANSI
target_compile_definitions(DX12Renderer
  PRIVATE "UNICODE;_UNICODE"
)

# Link Libs
find_package(directx-headers REQUIRED)
target_link_libraries(DX12Renderer PRIVATE d3dcompiler.lib d3d12.lib dxgi.lib dxguid.lib Microsoft::DirectX-Guids Microsoft::DirectX-Headers)

# PCH
target_precompile_headers(DX12Renderer PRIVATE "pch.h")

# Link Header/Source files
target_include_directories(DX12Renderer PRIVATE "${CMAKLE_CURRENT_LIST_DIR}")
target_sources(DX12Renderer
	PRIVATE "main.cpp" "Resource.h" "Core/Application.h" "Core/Application.cpp" "Utility/Assert.h")