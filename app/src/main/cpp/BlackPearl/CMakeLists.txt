cmake_minimum_required(VERSION 3.10)
project(BlackPearl LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 静态链接运行时库
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" CACHE STRING "" FORCE)

if(WIN32)
    set(SYSTEM "windows")
    set(ARCH "x86_64")
elseif (ANDROID)
    set(SYSTEM "android")
    set(ARCH "arm64-v8a")
endif ()

set(ROOT_DIR "${CMAKE_SOURCE_DIR}")
set(BLACKPEARL_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(VENDOR_DIR "${BLACKPEARL_DIR}/vendor")
set(OUTPUT_DIR "${CMAKE_BUILD_TYPE}-${SYSTEM}-${ARCH}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_OBJECT_OUTPUT_DIRECTORY "${ROOT_DIR}/bin-int/${OUTPUT_DIR}/${PROJECT_NAME}")


message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY---${CMAKE_RUNTIME_OUTPUT_DIRECTORY},OUTPUT_DIR=${OUTPUT_DIR}")
set(RENDER_API "opengl")
# 添加源文件
file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/hlsl/core/**.h"
)
if(${RENDER_API} STREQUAL "opengl")
list(REMOVE_ITEM SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/BlackPearl/Renderer/MasterRendererBak/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/BlackPearl/Renderer/MasterRendererBak/*.h"

)
endif ()
# 创建静态库目标
add_library(BlackPearl STATIC ${SRC_FILES})

# 宏定义
add_definitions(
        -D_CRT_SECURE_NO_WARNINGS
        -DGLFW_INCLUDE_NONE
)

# 头文件包含目录
include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}//assets/shaders/"
        "${VENDOR_DIR}"
        "${VENDOR_DIR}/spdlog/include"
        "${VENDOR_DIR}/GLEW/include"
        "${VENDOR_DIR}/Glad/include"
        "${VENDOR_DIR}/glm"
        "${VENDOR_DIR}/stb"
        "${VENDOR_DIR}/assimp/include"
        "${VENDOR_DIR}/GLFW/include"
        "${VENDOR_DIR}/VulkanSDK/1.3.236.0/Include"
        "${VENDOR_DIR}/directxtex_uwp.2022.7.30.1/include"
        "${VENDOR_DIR}/GLEW/include"
        "${VENDOR_DIR}/directxmesh_desktop_win10.2022.7.30.1/include"
        "${VENDOR_DIR}/ShaderMake/include"
)
# 库目录
target_link_directories(
        ${PROJECT_NAME} PUBLIC
        "${VENDOR_DIR}/directxtex_uwp.2022.5.10.1/native/lib"
        "${VENDOR_DIR}/directxmesh_desktop_win10.2022.7.30.1/native/lib/x64/${CMAKE_BUILD_TYPE}"
        "${VENDOR_DIR}/VulkanSDK/1.3.236.0/Lib"
        "${VENDOR_DIR}/ShaderMake/build/Debug"
        "${VENDOR_DIR}/spdlog/bin/${OUTPUT_DIR}/spdlog"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp_build/bin/${CMAKE_BUILD_TYPE}/${SYSTEM}"
)
if(WIN32)
    target_link_directories(
            ${PROJECT_NAME} PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/bin/${OUTPUT_DIR}/GLFW"
            "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/bin/${OUTPUT_DIR}/ImGui"
    )
endif ()

# 链接库和条件编译
if(${RENDER_API} STREQUAL "opengl")


    message("RENDER_API = opengl")
    target_link_libraries(
            BlackPearl
            PRIVATE
            spdlog
    )
    if(WIN32)
        target_link_libraries(
                BlackPearl
                PRIVATE
                glfw
                GLEW
                Glad
                opengl32
                ImGui
        )
    elseif(ANDROID)
        target_link_libraries(
            BlackPearl
            PRIVATE
                EGL
                GLESv3
                assimp
    )
    endif ()
    add_definitions(
            -DGLFW_INCLUDE_NONE
            -DGE_API_OPENGL
    )
elseif(${RENDER_API} STREQUAL "vulkan")
    message("RENDER_API = vulkan")
    target_link_libraries(
            BlackPearl
            PRIVATE
            glfw
            GLEW
            Glad
            ImGui
            vulkan-1
            assimp-vc142-mtd
            spdlog
    )
    add_definitions(
            -DGLFW_INCLUDE_NONE
            -DGE_API_VULKAN
    )
elseif(${RENDER_API} STREQUAL "direct3d")
    message("RENDER_API = direct3d")
    target_link_libraries(
            BlackPearl
            PRIVATE
            glfw
            GLEW
            Glad
            ImGui
            assimp-vc142-mtd
    )
    add_definitions(
            -DGLFW_INCLUDE_NONE
            -DGE_API_D3D12
    )
else() #default opengl
    message("set default RENDER_API = opengl")
    target_link_libraries(
            BlackPearl
            PRIVATE
            glfw
            GLEW
            Glad
            ImGui
            opengl32
            assimp-vc142-mtd
            spdlog
    )
    add_definitions(
            -DGLFW_INCLUDE_NONE
            -DGE_API_OPENGL
    )
endif()

# Windows平台特有设置
if(WIN32)
    message(STATUS "Building for Windows")
    add_definitions(
            -DGE_PLATFORM_WINDOWS
    )
elseif(ANDROID)
    message(STATUS "Building for Android")
    add_definitions(
            -DGE_PLATFORM_ANDRIOD
            -DGLEW_NO_GLU
    )
endif()

# 构建配置设置
# 设置Windows系统版本，适用于Visual Studio Generator
if(WIN32)
    set(CMAKE_SYSTEM_VERSION "10.0") # Windows 10
endif()

# 定义构建类型
set(BUILD_TYPES Debug Release MinSizeRel)
foreach(BUILD_TYPE ${BUILD_TYPES})
    string(TOUPPER ${BUILD_TYPE} BUILD_TYPE_UC)

    # 设置CMake构建类型
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "Setting CMAKE_BUILD_TYPE to ${BUILD_TYPE_UC}")
        set(CMAKE_BUILD_TYPE ${BUILD_TYPE} CACHE STRING "Build type" FORCE)
    endif()

    # 根据构建类型设置编译选项
    if(BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Configuring for Debug build")
        if(WIN32)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi") # Disable optimizations, enable debug info
        endif ()
    elseif(BUILD_TYPE STREQUAL "Release")
        message(STATUS "Configuring for Release build")
        if(WIN32)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2") # Optimize for speed
        endif ()
    elseif(BUILD_TYPE STREQUAL "MinSizeRel")
        message(STATUS "Configuring for MinSizeRel build")
    endif()
endforeach()



# 预编译头
target_precompile_headers(BlackPearl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")
set(PRECOMPILED_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.cpp")