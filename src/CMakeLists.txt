set(DX12RENDERER_SOURCE_FILES
		main.cpp
		Core/Application.cpp)

set(DX12RENDERER_HEADER_FILES
		../include/main.h
		../include/framework.h
		../include/Resource.h
		../include/Core/Application.h)

add_executable(DX12Renderer WIN32 ${DX12RENDERER_SOURCE_FILES} ${DX12RENDERER_HEADER_FILES})
target_include_directories(DX12Renderer PUBLIC ../include)