cmake_minimum_required(VERSION 3.22.1)

project("SandBoxAndriod" LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_C_STANDARD 99)
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
set(VENDOR_DIR "${BLACKPEARL_DIR}/vendor")
message(STATUS "ROOT_DIR=${ROOT_DIR}")
set(OUTPUT_DIR "${CMAKE_BUILD_TYPE}-${SYSTEM}-${ARCH}")
set(VENDOR_DIR "${BLACKPEARL_DIR}/vendor")
set(JniLibs_DIR "${ROOT_DIR}/../jniLibs")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_OBJECT_OUTPUT_DIRECTORY "${ROOT_DIR}/bin-int/${OUTPUT_DIR}/${PROJECT_NAME}")
# 获取环境变量
set(NDKROOT_PATH $ENV{NDKROOT})
message(STATUS "NDKROOT_PATH=${NDKROOT_PATH}")

file(GLOB src-files
        ${SANDBOX_DIR}/src/*.cpp
        ${SANDBOX_DIR}/src/*.h
)
list(FILTER src-files EXCLUDE REGEX  ${SANDBOX_DIR}/src/native-lib.cpp)


list(FILTER src-files EXCLUDE REGEX "/.vshistory/.*")


add_definitions(-DGE_API_GLES)
add_definitions(-DGE_PLATFORM_ANDROID)
add_definitions(-DSPDLOG_HEADER_ONLY)

# 添加 spdlog 子目录
#add_subdirectory("${VENDOR_DIR}/spdlog")
# 创建静态库目标
add_library(
        SandBoxAndriod
        SHARED
        ${src-files}
)
#add_definitions(
#        -DSPDLOG_HEADER_ONLY
#
#)
target_link_directories(${PROJECT_NAME} PUBLIC
        "${ROOT_DIR}/bin/${OUTPUT_DIR}/BlackPearl"
        "${ROOT_DIR}/BlackPearl/vendor/assimp_build/bin/${CMAKE_BUILD_TYPE}/${SYSTEM}"

)
set(BLACKPEARL_LIBRARY  "${ROOT_DIR}/bin/${OUTPUT_DIR}/BlackPearl/libBlackPearl.a")
target_include_directories(
        SandBoxAndriod
        PRIVATE
      #  "${SANDBOX_DIR}/src"
        "${ROOT_DIR}/BlackPearl/include"
        "${ROOT_DIR}/BlackPearl/vendor/glm"
        "${ROOT_DIR}/BlackPearl/vendor/spdlog/include"
        "${ROOT_DIR}/BlackPearl/vendor/assimp/include"
        "${NDKROOT_PATH}/sources/android/native_app_glue"
        "${ROOT_DIR}/BlackPearl/assets/shaders/"
)


target_link_libraries(
            SandBoxAndriod
            PUBLIC
            ${BLACKPEARL_LIBRARY}
            #BlackPearl
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