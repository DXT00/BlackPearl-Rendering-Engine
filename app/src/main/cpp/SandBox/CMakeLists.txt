cmake_minimum_required(VERSION 3.15)
project(Sandbox)

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
        ../BlackPearl/vendor/spdlog/include
        ../BlackPearl/include/BlackPearl
        ../BlackPearl/include/
        ../BlackPearl/vendor
        ../BlackPearl/vendor/stb
        ../BlackPearl/assets/shaders
        ../${IncludeDir.glm}
        ../${IncludeDir.GLFW}
        ../${IncludeDir.assimp}
        ../${IncludeDir.Glad}
        ../${IncludeDir.glslShader}
        ../${IncludeDir.vulkan}
)

# 源文件
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")
file(GLOB HLSL_SHADERS "assets/shaders_hlsl/*.hlsl")

# 添加可执行文件
add_executable(Sandbox ${SOURCES} ${HEADERS})

# 链接库
target_link_libraries(Sandbox
        BlackPearl
        #d3d12
        #dxgi
        #dxguid
        # vulkan-1
        # directxtex
)

# 定义宏
target_compile_definitions(Sandbox PRIVATE
        GE_API_OPENGL
)

# 平台特定设置
if (WIN32)
    target_compile_definitions(Sandbox PRIVATE
            GLFW_INCLUDE_NONE
            GE_PLATFORM_WINDOWS
    )
endif()

# 配置特定设置
set_property(TARGET Sandbox PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Debug 配置
set_target_properties(Sandbox PROPERTIES
        COMPILE_DEFINITIONS_DEBUG "GE_DEBUG"
        CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /RTC1"
)

# Release 配置
set_target_properties(Sandbox PROPERTIES
        COMPILE_DEFINITIONS_RELEASE "GE_RELEASE"
        CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG"
)

# Dist 配置
set_target_properties(Sandbox PROPERTIES
        COMPILE_DEFINITIONS_DIST "GE_DIST"
        CMAKE_CXX_FLAGS_DIST "/MD /O2 /Ob2 /DNDEBUG"
)

# HLSL 着色器编译（需要自定义命令）
foreach(HLSL_SHADER ${HLSL_SHADERS})
    get_filename_component(SHADER_NAME ${HLSL_SHADER} NAME_WE)
    set(CSO_OUTPUT "${CMAKE_BINARY_DIR}/bin/${SHADER_NAME}.cso")
    set(HEADER_OUTPUT "${CMAKE_SOURCE_DIR}/assets/shaders_hlsl_h/${SHADER_NAME}.hlsl.h")

    add_custom_command(
            OUTPUT ${CSO_OUTPUT} ${HEADER_OUTPUT}
            COMMAND fxc.exe /T ps_6_3 /E main /Fo ${CSO_OUTPUT} /Fh ${HEADER_OUTPUT} ${HLSL_SHADER}
            DEPENDS ${HLSL_SHADER}
            COMMENT "Compiling HLSL shader: ${HLSL_SHADER}"
    )

    list(APPEND SHADER_OBJECTS ${CSO_OUTPUT})
    list(APPEND SHADER_HEADERS ${HEADER_OUTPUT})
endforeach()

# 将生成的着色器文件添加到项目中
target_sources(Sandbox PRIVATE ${SHADER_OBJECTS} ${SHADER_HEADERS})