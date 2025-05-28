cmake_minimum_required(VERSION 3.22.1)

project("SandBoxAndroid" LANGUAGES CXX)

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
set(ANDROID_SANDBOX_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BLACKPEARL_DIR "${ROOT_DIR}/BlackPearl")
set(SANDBOX_DIR "${ROOT_DIR}/SandBox")
set(VENDOR_DIR "${BLACKPEARL_DIR}/vendor")
message(STATUS "ROOT_DIR=${ROOT_DIR}")
set(OUTPUT_DIR "${CMAKE_BUILD_TYPE}-${SYSTEM}-${ARCH}")
set(VENDOR_DIR "${BLACKPEARL_DIR}/vendor")
set(JniLibs_DIR "${ROOT_DIR}/../jniLibs")
set(INPUT_Shaders_DIR "${ROOT_DIR}/assets/shaders")
set(OUTPUT_Shaders_DIR "${ROOT_DIR}/../assets/shaders")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ROOT_DIR}/bin/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_OBJECT_OUTPUT_DIRECTORY "${ROOT_DIR}/bin-int/${OUTPUT_DIR}/${PROJECT_NAME}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
# 获取环境变量
set(NDKROOT_PATH $ENV{NDKROOT})
message(STATUS "NDKROOT_PATH=${NDKROOT_PATH}")

file(GLOB src-files
        ${ANDROID_SANDBOX_DIR}/src/*.cpp
        ${ANDROID_SANDBOX_DIR}/src/*.h
#        ${ANDROID_SANDBOX_DIR}/src/native_app_glue_25/*.cpp
#        ${ANDROID_SANDBOX_DIR}/src/native_app_glue_25/*.h
#        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
#        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.h
)
list(FILTER src-files EXCLUDE REGEX  ${ANDROID_SANDBOX_DIR}/src/native-lib.cpp)


list(FILTER src-files EXCLUDE REGEX "/.vshistory/.*")


add_definitions(-DGE_API_GLES)
add_definitions(-DGE_PLATFORM_ANDROID)
add_definitions(-DSPDLOG_HEADER_ONLY)



############################## 创建 android_native_app_glue 库 ##########################

message(STATUS "ANDROID_NDK = ${ANDROID_NDK}, include ${ANDROID_NDK}/sources/android/native_app_glue/include")

## 添加native_app_glue static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate -u JNI_OnLoad")

#
#set_target_properties(native_app_glue PROPERTIES
#        INTERFACE_INCLUDE_DIRECTORIES ${ANDROID_NDK}/sources/android/native_app_glue
#        VISIBILITY_INTERFACE_PUBLIC ON
#)


############################## 链接库 ##########################


# 添加 spdlog 子目录
#add_subdirectory("${VENDOR_DIR}/spdlog")
# 创建静态库目标
add_library(
        SandBoxAndroid
        SHARED
        ${src-files}
)
#target_include_directories(SandBoxAndroid PRIVATE
#        ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_directories(${PROJECT_NAME} PUBLIC
        "${ROOT_DIR}/bin/${OUTPUT_DIR}/BlackPearl"
        "${ROOT_DIR}/BlackPearl/vendor/assimp_build/bin/${CMAKE_BUILD_TYPE}/${SYSTEM}"

)
set(BLACKPEARL_LIBRARY  "${ROOT_DIR}/bin/${OUTPUT_DIR}/BlackPearl/libBlackPearl.a")
target_include_directories(
        SandBoxAndroid
        PRIVATE
      #  "${ANDROID_SANDBOX_DIR}/src"
        "${ROOT_DIR}/BlackPearl/include"
        "${ROOT_DIR}/BlackPearl/include/BlackPearl"
        "${ROOT_DIR}/BlackPearl/vendor/glm"
        "${ROOT_DIR}/BlackPearl/vendor/spdlog/include"
        "${ROOT_DIR}/BlackPearl/vendor/assimp/include"
        "${ANDROID_NDK}/sources/android/native_app_glue"
        "${ROOT_DIR}/../assets/shaders/"
)
set_target_properties(SandBoxAndroid PROPERTIES
        LINK_FLAGS "-Wl,--export-dynamic"
)

target_link_libraries(
            SandBoxAndroid
            PUBLIC
            ${BLACKPEARL_LIBRARY}
            #BlackPearl
            EGL
            GLESv3
            android
            log
            spdlog
            assimp
            native_app_glue
)

# ======================================================
# 函数：copy_if_md5_changed
# 功能：递归拷贝文件夹，仅当文件 MD5 不同时才复制
# 参数：
#   - SOURCE_DIR: 源文件夹路径
#   - DEST_DIR: 目标文件夹路径
#   - TARGET_NAME: 关联的目标（用于 POST_BUILD）
# ======================================================
function(copy_if_md5_changed SOURCE_DIR DEST_DIR TARGET_NAME)
    # 获取所有文件列表（递归）
    file(GLOB_RECURSE FILES_TO_COPY
            LIST_DIRECTORIES false
            RELATIVE "${SOURCE_DIR}"
            "${SOURCE_DIR}/*"
    )

    # 遍历所有文件
    foreach(RELATIVE_PATH ${FILES_TO_COPY})
        # 源文件和目标文件的完整路径
        set(SRC_FILE "${SOURCE_DIR}/${RELATIVE_PATH}")
        set(DEST_FILE "${DEST_DIR}/${RELATIVE_PATH}")

        string(FIND "${item}" "/.vshistory/" pos)
        if(NOT pos EQUAL -1)
            message(STATUS "exclude ${item}")
            continue()
        endif()

        # 计算源文件的 MD5
        file(MD5 "${SRC_FILE}" SOURCE_MD5)

        # 检查目标文件是否存在并计算 MD5
        if(EXISTS "${DEST_FILE}")
            file(MD5 "${DEST_FILE}" DEST_MD5)
        else()
            set(DEST_MD5 "FILE_NOT_EXIST")  # 目标文件不存在，需要复制
        endif()

        # 如果 MD5 不同或目标文件不存在，则添加拷贝命令
        if(NOT "${SOURCE_MD5}" STREQUAL "${DEST_MD5}")
            # 确保目标目录存在
            get_filename_component(DEST_DIRECTORY "${DEST_FILE}" DIRECTORY)
            add_custom_command(
                    TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIRECTORY}"
                    COMMAND ${CMAKE_COMMAND} -E copy "${SRC_FILE}" "${DEST_FILE}"
                    COMMENT "Copying ${RELATIVE_PATH} (MD5 mismatch)"
            )
        endif()
    endforeach()
endfunction()


if(ANDROID)
    # copy libSandBoxAndroid.so to android jnilib folder
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libSandBoxAndroid.so
            "${JniLibs_DIR}/${ARCH}/libSandBoxAndroid.so"
    )

    # copy SandBox shader to android assets folder if md5sum is different
#    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy_directory
#            "${INPUT_Shaders_DIR}"
#            "${OUTPUT_Shaders_DIR}"
#    )

#    # 定义源文件夹和目标文件夹
#    set(SHADERS_SOURCE_DIR "${INPUT_Shaders_DIR}")
#    set(SHADERS_DEST_DIR "${OUTPUT_Shaders_DIR}")
#
#    # 调用函数（在目标构建完成后执行）
#    copy_if_md5_changed(
#            "${SHADERS_SOURCE_DIR}"
#            "${SHADERS_DEST_DIR}"
#            ${PROJECT_NAME}  # 关联的目标（通常是可执行文件或库）
#    )



endif ()


