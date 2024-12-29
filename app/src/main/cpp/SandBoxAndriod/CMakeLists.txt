cmake_minimum_required(VERSION 3.22.1)

project("SandBoxAndriod" LANGUAGES CXX)

if(WIN32)
    set(SYSTEM "windows")
    set(ARCH "x86_64")
elseif (ANDROID)
    set(SYSTEM "android")
    set(ARCH "arm64-v8a")
endif ()

set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SANDBOX_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BLACKPEARL_DIR "${ROOT_DIR}/BlackPearl")

set(OUTPUT_DIR "${CMAKE_BUILD_TYPE}-${SYSTEM}-${ARCH}")
set(VENDOR_DIR "${BLACKPEARL_DIR}/vendor")
set(JniLibs_DIR "${ROOT_DIR}/../jniLibs")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_OBJECT_OUTPUT_DIRECTORY "${ROOT_DIR}/bin-int/${OUTPUT_DIR}/${PROJECT_NAME}")

file(GLOB src-files
        ${BLACKPEARL_DIR}/src/*.h
        ${SANDBOX_DIR}/src/*.cpp
        ${SANDBOX_DIR}/src/*.h
)

add_definitions(-DGE_API_GLES)
add_definitions(-DGE_PLATFORM_ANDRIOD)

# 创建静态库目标
add_library(
        SandBoxAndriod
        SHARED
        ${src-files}
)

target_link_directories(${PROJECT_NAME} PUBLIC
        "${ROOT_DIR}/bin/${OUTPUT_DIR}/BlackPearl"
        "${VENDOR_DIR}/spdlog/bin/${OUTPUT_DIR}/spdlog"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp_build/bin/${CMAKE_BUILD_TYPE}/${SYSTEM}"
)

target_include_directories(
        SandBoxAndriod
        PUBLIC
        "${SANDBOX_DIR}/src"
        "${ROOT_DIR}/BlackPearl/src"
        "${ROOT_DIR}/BlackPearl/vendor/glm"
        "${ROOT_DIR}/BlackPearl/vendor/spdlog/include"
        "${ROOT_DIR}/BlackPearl/vendor/assimp/include"
)


target_link_libraries(
            SandBoxAndriod
            PRIVATE
            BlackPearl
            EGL
            GLESv3
            android
            log
            spdlog
            assimp
)

if(ANDROID)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libSandBoxAndriod.so
            "${JniLibs_DIR}/${ARCH}/libSandBoxAndriod.so"
    )
endif ()