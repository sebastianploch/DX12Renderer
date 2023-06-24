add_executable(DX12Renderer WIN32)

target_compile_features(DX12Renderer PUBLIC cxx_std_23)

# Enable Edit and Continue for MSVC compilers if supported.
if(MSVC)
  target_compile_options(DX12Renderer PUBLIC "/permissive /std:c++23 /ZI /W4 /WX")
  target_link_options(DX12Renderer PUBLIC "/INCREMENTAL")
endif()

# Use UNICODE instead of ANSI
target_compile_definitions(DX12Renderer
  PRIVATE "UNICODE;_UNICODE"
)

# Link DX12 Libs
target_link_libraries(DX12Renderer PRIVATE d3dcompiler.lib d3d12.lib dxgi.lib)

# PCH
target_precompile_headers(DX12Renderer PRIVATE "pch.h")

# Link Header/Source files
target_include_directories(DX12Renderer PRIVATE "${CMAKLE_CURRENT_LIST_DIR}")
target_sources(DX12Renderer
	PRIVATE "main.cpp" "Resource.h" "Core/Application.h" "Core/Application.cpp")