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
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY---${CMAKE_RUNTIME_OUTPUT_DIRECTORY},OUTPUT_DIR=${OUTPUT_DIR}")
set(RENDER_API "opengl")

################################################################ 源文件过滤 ################################################################
file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../assets/shaders/hlsl/core/**.h"
)

list(FILTER SRC_FILES EXCLUDE REGEX "/.vshistory/.*")

if(${RENDER_API} STREQUAL "opengl")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Renderer/MasterRendererBak/.*")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/RHI/VulkanRHI/.*")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Renderer/Lumen/.*")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Lumen/.*")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/RHI/D3D12RHI/.*")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Debugger/D3D12Debugger/.*")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Renderer/Buffer/D3D12Buffer/.*")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/D3D12Buffer/.*")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*Renderer/MasterRenderer/D3D12*.")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*Renderer/Shader/D3D12Shader/*.")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*Renderer/Buffer/VkBuffer/*.")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*Renderer/Shader/VkShader/*.")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*Renderer/Image/Vk*.")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*Renderer/MasterRenderer/Vk*.")
elseif(${RENDER_API} STREQUAL "vulkan")
            list(FILTER SRC_FILES EXCLUDE REGEX  ".*/RHI/OpenGLRHI/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX  ".*/RHI/OpenGLRHI/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX  ".*/RHI/D3D12RHI/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX  ".*/RHI/D3D12RHI/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Renderer/Buffer/D3D12Buffer/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Renderer/Buffer/D3D12Buffer/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Renderer/MasterRenderer/D3D12*.")
            list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Renderer/MasterRenderer/D3D12*.")
            list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Renderer/Shader/D3D12Shader/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Renderer/Shader/D3D12Shader/*.")
elseif(${RENDER_API} STREQUAL "direct3d")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/RHI/VulkanRHI/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/RHI/VulkanRHI/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/RHI/OpenGLRHI/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/RHI/OpenGLRHI/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/Renderer/Buffer/VkBuffer/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/Renderer/Buffer/VkBuffer/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/Renderer/Shader/VkShader/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/Renderer/Shader/VkShader/*.")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/Renderer/Image/Vk*.")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/Renderer/Image/Vk*.")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/Renderer/MasterRenderer/Vk*.")
            list(FILTER SRC_FILES EXCLUDE REGEX ".*/Renderer/MasterRenderer/Vk*.")

endif ()


if(ANDROID)
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Core/Windows/.*")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/RHI/OpenGLRHI/OpenGLDriver/Windows/.*")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/ImGui/.*")
elseif (WIN32)
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/Core/Android/.*")
    list(FILTER SRC_FILES EXCLUDE REGEX  ".*/RHI/OpenGLRHI/OpenGLDriver/Android/.*")

endif ()

message(STATUS "SRC_FILES=${SRC_FILES}")


# 创建静态库目标
add_library(BlackPearl STATIC ${SRC_FILES}
        include/BlackPearl/Renderer/MasterRenderer/VoxelConeTracingRenderer.h
        src/BlackPearl/Renderer/MasterRenderer/VoxelConeTracingRenderer.cpp
)

# 宏定义
add_definitions(
        -D_CRT_SECURE_NO_WARNINGS
        -DGLFW_INCLUDE_NONE

)
################################################################ 设置include dir ################################################################
set(INCLUDE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/BlackPearl"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/BlackPearl/Core"
        "${CMAKE_CURRENT_SOURCE_DIR}/../assets/shaders/"
        "${VENDOR_DIR}"
        "${VENDOR_DIR}/spdlog/include"
)
if(${RENDER_API} STREQUAL "opengl")
    list(APPEND INCLUDE_DIR
            "${CMAKE_CURRENT_SOURCE_DIR}/include/BlackPearl/RHI/OpenGLRHI"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/BlackPearl/RHI/OpenGLRHI/OpenGLDriver"
    )
elseif (${RENDER_API} STREQUAL "vulkan")
    list(APPEND INCLUDE_DIR
            "${CMAKE_CURRENT_SOURCE_DIR}/include/BlackPearl/RHI/VulkanRHI"
    )
elseif (${RENDER_API} STREQUAL "direct3d")
    list(APPEND INCLUDE_DIR
            "${CMAKE_CURRENT_SOURCE_DIR}/include/BlackPearl/RHI/D3D12RHI"
    )
endif ()

if(WIN32)
    list(APPEND INCLUDE_DIR
            "${VENDOR_DIR}/OpenGL/include"
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
elseif (ANDROID)
    list(APPEND INCLUDE_DIR
            "${VENDOR_DIR}/glm"
            "${VENDOR_DIR}/stb"
            "${VENDOR_DIR}/assimp/include"
            "${VENDOR_DIR}/ShaderMake/include"
    )

endif ()
# 头文件包含目录
target_include_directories(
        BlackPearl
        PUBLIC
        ${INCLUDE_DIR}
)
############################## 创建 android_native_app_glue 库 ##########################
if(ANDROID)
    # 获取环境变量
    set(NDKROOT_PATH $ENV{NDKROOT})
    message(STATUS "bp NDKROOT_PATH=${NDKROOT_PATH}")

    include_directories(
            "${NDKROOT_PATH}/sources/android/native_app_glue"
    )

    # 添加 native_app_glue 的源文件（需编译为静态库）
    set(ANDROID_NATIVE_APP_GLUE_SRC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c

    )

    # 将 native_app_glue 编译为静态库
    add_library(android_native_app_glue STATIC ${ANDROID_NATIVE_APP_GLUE_SRC})

    # 设置目标属性（禁用无关警告）
    target_compile_options(android_native_app_glue PRIVATE -Wno-unused-parameter)


############################## 创建 cpufeatures 库 ##########################


    # 添加 cpufeatures 库
    add_library(cpufeatures STATIC
            ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
    include_directories(
            "${ANDROID_NDK}/sources/android/cpufeatures"
    )

#    target_link_libraries(BlackPearl
#            cpufeatures  # 链接 cpufeatures
#            )

endif()


# 标记为可被其他模块使用
set_target_properties(android_native_app_glue PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ANDROID_NDK}/sources/android/native_app_glue
        VISIBILITY_INTERFACE_PUBLIC ON
)

################################################################ 库目录 ################################################################
target_link_directories(
        ${PROJECT_NAME} PRIVATE
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

############################## 添加 spdlog 库 ##########################

#set(SPDLOG_BUILD_SHARED OFF)
#set(SPDLOG_BUILD_TESTS OFF)
add_subdirectory(${VENDOR_DIR}/spdlog)

##############################  链接库和条件编译 ##########################

if(${RENDER_API} STREQUAL "opengl")
    message("RENDER_API = opengl")
    if(WIN32)
        target_link_libraries(
                BlackPearl
                PUBLIC
                glfw
                GLEW
                Glad
                opengl32
                ImGui
        )
        add_definitions(
                -DUSE_IMGUI
        )
    elseif(ANDROID)
        target_link_libraries(
            BlackPearl
                PUBLIC
                EGL
                GLESv3
                assimp
                android_native_app_glue
                cpufeatures
                #android
                spdlog::spdlog_header_only
        )
        add_definitions(
                -DUSE_ANDROID_JNI=1
                -DUSE_ANDROID_EVENTS
                -DSPDLOG_COMPILED_LIB
        )
        message(STATUS "Android -DSPDLOG_COMPILED_LIB")
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
            spdlog::spdlog_header_only
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
    message("set default RENDER_API = vulkan")
    target_link_libraries(
            BlackPearl
            PRIVATE
            glfw
            GLEW
            Glad
            ImGui
            vulkan-1
            assimp-vc142-mtd
            spdlog::spdlog_header_only
    )
    add_definitions(
            -DGLFW_INCLUDE_NONE
            -DGE_API_VULKAN
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
            -DGE_PLATFORM_ANDROID
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
target_precompile_headers(BlackPearl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/pch.h")
set(PRECOMPILED_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.cpp")