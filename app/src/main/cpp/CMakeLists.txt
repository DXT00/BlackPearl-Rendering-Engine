cmake_minimum_required(VERSION 3.10)
project(BlackPearl-Engine)

# 设置构建配置
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 选择渲染API

#option(RENDER_API_OPENGL "Use OpenGL for rendering" OFF)
#option(RENDER_API_DIRECT3D "Use Direct3D (Windows only)" OFF)
#option(RENDER_API_VULKAN "Use Vulkan" ON)

# 选择目标平台
option(TARGET_PLATFORM_WINDOWS "Target Windows platform" ON)
option(TARGET_PLATFORM_ANDROID "Target Android platform" OFF)

# 输出目录
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
elseif(ANDROID)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_ANDROID_ARCH_ABI})
endif()

set(RENDER_API "opengl")
## 包含目录
#include_directories(
#        ${CMAKE_CURRENT_SOURCE_DIR}/BlackPearl/vendor/GLFW/include
#        ${CMAKE_CURRENT_SOURCE_DIR}/BlackPearl/vendor/Glad/include
#        ${CMAKE_CURRENT_SOURCE_DIR}/BlackPearl/vendor/imgui
#        ${CMAKE_CURRENT_SOURCE_DIR}/BlackPearl/vendor/glm
#        ${CMAKE_CURRENT_SOURCE_DIR}/BlackPearl/vendor/stb
#        ${CMAKE_CURRENT_SOURCE_DIR}/BlackPearl/vendor/assimp/include
#        ${CMAKE_CURRENT_SOURCE_DIR}/BlackPearl/vendor/GLEW/include
#        ${CMAKE_CURRENT_SOURCE_DIR}/BlackPearl/vendor/VulkanSDK/1.3.236.0/Include
#        ${CMAKE_CURRENT_SOURCE_DIR}/BlackPearl/vendor/directxtex_uwp.2022.7.30.1/include
#        ${CMAKE_CURRENT_SOURCE_DIR}/BlackPearl/vendor/directxmesh_desktop_win10.2022.7.30.1/include
#        ${CMAKE_CURRENT_SOURCE_DIR}/BlackPearl/vendor/ShaderMake/include
#        ${CMAKE_CURRENT_SOURCE_DIR}/SandboxDX/assets/shaders_hlsl_h
#)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# 根据平台和渲染API选择合适的库
#if(TARGET_PLATFORM_WINDOWS)
#    if(${RENDER_API} STREQUAL "opengl")
#        find_package(GLFW REQUIRED)
#        find_package(GLEW REQUIRED)
#        find_package(ImGui REQUIRED)
#    elseif(${RENDER_API} STREQUAL "direct3d")
#        # 导入Direct3D相关库
#    elseif(${RENDER_API} STREQUAL "vulkan")
#        find_package(Vulkan REQUIRED)
#    endif()
#elseif(TARGET_PLATFORM_ANDROID)
#    # Android平台的配置
#    find_package(OpenGL REQUIRED)
#    find_package(EGL REQUIRED)
#    find_package(GLESv2 REQUIRED)
#    # ...其他Android特定配置
#endif()

# 添加子项目
add_subdirectory(BlackPearl)
add_subdirectory(BlackPearl/vendor/imgui)
add_subdirectory(BlackPearl/vendor/spdlog)
add_subdirectory(BlackPearl/vendor/assimp)
if(WIN32)
    add_subdirectory(BlackPearl/vendor/GLFW)
    add_subdirectory(Sandbox)
    add_subdirectory(SandboxVK)
    add_subdirectory(SandboxDX)
    add_subdirectory(ShaderCompiler)
endif()

if(ANDROID)
add_subdirectory(SandboxAndriod)
add_subdirectory(native_render)

endif()
#

# 如果需要，添加更多子项目
# add_subdirectory(BlackPearl/vendor/assimp)
# add_subdirectory(Sandbox)
# add_subdirectory(SandboxVK)
# add_subdirectory(SandboxDX)
# add_subdirectory(SandboxAndriod)
# add_subdirectory(ShaderCompiler)