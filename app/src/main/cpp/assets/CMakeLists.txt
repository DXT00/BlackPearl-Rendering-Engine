cmake_minimum_required(VERSION 3.15)
project(assets)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置静态运行时库（MT/MTd）
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(
		shaders/glsl/**.glsl,
		shaders/hlsl/**.hlsl,
		shaders/hlsl/**.h,
)

target_include_directories(
		assets
		PUBLIC
		"shaders/glsl"
		"../"
)
# 源文件
# Add source files
file(GLOB_RECURSE SHADER_FILES 
    "shaders/glsl/*.glsl"
    "shaders/hlsl/*.hlsl"
    "shaders/hlsl/*.h"
)
list(FILTER SHADER_FILES EXCLUDE REGEX "/.vshistory/.*")

# 创建静态库目标
add_library(assets STATIC ${SHADER_FILES})

# 配置特定设置
set_property(TARGET assets PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Debug 配置
set_target_properties(assets PROPERTIES
        COMPILE_DEFINITIONS_DEBUG "GE_DEBUG"
        CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /RTC1"
)

# Release 配置
set_target_properties(assets PROPERTIES
        COMPILE_DEFINITIONS_RELEASE "GE_RELEASE"
        CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG"
)

# Dist 配置
set_target_properties(assets PROPERTIES
        COMPILE_DEFINITIONS_DIST "GE_DIST"
        CMAKE_CXX_FLAGS_DIST "/MD /O2 /Ob2 /DNDEBUG"
)



# 将生成的着色器文件添加到项目中
target_sources(assets PRIVATE ${SHADER_OBJECTS} ${SHADER_HEADERS})