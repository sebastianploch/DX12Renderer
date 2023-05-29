# CMakeList.txt : CMake project for DX12Renderer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.24)

# Enable Edit and Continue for MSVC compilers if supported.
if(MSVC)
  target_compile_options(<target> PUBLIC "/permissive /ZI /W4 /WX")
  target_link_options(<target> PUBLIC "/INCREMENTAL")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(DX12Renderer LANGUAGES CXX)
add_subdirectory("src")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DX12Renderer PROPERTY CXX_STANDARD 23)
endif()

# Use UNICODE instead of ANSI
target_compile_definitions(DX12Renderer
  PRIVATE "UNICODE;_UNICODE"
)