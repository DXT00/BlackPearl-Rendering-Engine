
cmake_minimum_required(VERSION 3.12)
project(ShaderCompiler)

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置静态运行时库
if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE})

set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(ShaderCompiler_DIR "${ROOT_DIR}/ShaderCompiler")

message(STATUS "ShaderCompiler_DIR = ${ShaderCompiler_DIR}")
# 添加源文件
file(GLOB
		SOURCES
		"${ShaderCompiler_DIR}/src/*.h"
		"${ShaderCompiler_DIR}/src/*.hpp"
		"${ShaderCompiler_DIR}/src/*.cpp"
)
message(STATUS "SOURCES = ${SOURCES}")

# 添加可执行文件
add_executable(ShaderCompiler ${SOURCES})

# 包含目录
target_include_directories(ShaderCompiler PRIVATE ../BlackPearl/src)

# 链接库
target_link_libraries(ShaderCompiler BlackPearl)

# 定义宏
target_compile_definitions(ShaderCompiler PRIVATE GE_SHADERCOMPILE)

# Windows 平台特定设置
if(WIN32)
	target_compile_definitions(ShaderCompiler PRIVATE GE_PLATFORM_WINDOWS)
endif()

# Debug 配置
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ShaderCompiler)
set_property(TARGET ShaderCompiler PROPERTY CMAKE_DEBUG_POSTFIX "_d")

# 配置设置
foreach(CONFIG IN ITEMS Debug Release Dist)
	string(TOUPPER ${CONFIG} CONFIG_UPPER)
	set_target_properties(ShaderCompiler PROPERTIES
			COMPILE_DEFINITIONS_${CONFIG_UPPER} GE_${CONFIG_UPPER}
			RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG}
			ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin-int/${CONFIG}
			LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG}
	)
endforeach()

# 优化设置
set_property(TARGET ShaderCompiler PROPERTY CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set_property(TARGET ShaderCompiler PROPERTY CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set_property(TARGET ShaderCompiler PROPERTY CMAKE_CXX_FLAGS_DIST "${CMAKE_CXX_FLAGS_RELEASE} -O2")