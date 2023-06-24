add_executable(DX12Renderer WIN32)
target_include_directories(DX12Renderer PRIVATE src)

target_sources(DX12Renderer
	PRIVATE
		"framework.h"
		"main.cpp"
		"Resource.h"
		"targetver.h"
		"Core/Application.h"
		"Core/Application.cpp")