# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("SandBoxAndriod" LANGUAGES CXX)


file(GLOB src-files
#        ${CMAKE_SOURCE_DIR}/BlackPearl/src/*.cpp
#        ${CMAKE_SOURCE_DIR}/BlackPearl/src/*.h
        ../BlackPearl/src/*.cpp
        ../BlackPearl/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h

)

add_definitions(-DGE_API_GLES)
add_definitions(-DGE_PLATFORM_ANDRIOD)

# 创建静态库目标
add_library( # Sets the name of the library.
        SandBoxAndriod

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${src-files}
)



link_directories(
#        "${CMAKE_CURRENT_SOURCE_DIR}/lib"
        "${CMAKE_SOURCE_DIR}/BlackPearl/vendor/assimp_build/lib/Debug/android"
        "${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}"
        "../bin/arm64-v8a"
        "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/SandBoxAndriod/../bin/arm64-v8a/"

        #       "${CMAKE_SOURCE_DIR}/bin/arm64-v8a"
)

message(STATUS "-------make ${CMAKE_CURRENT_SOURCE_DIR}- ${CMAKE_SOURCE_DIR}/BlackPearl/src--------------------")

target_include_directories(
        SandBoxAndriod
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "../BlackPearl/src"
        "../BlackPearl/vendor/glm"
        "../BlackPearl/vendor/spdlog/include"
        "../BlackPearl//vendor/assimp/include"
)


#include_directories(
#        "${CMAKE_CURRENT_SOURCE_DIR}/src"
#        "../BlackPearl/src"
#        "../BlackPearl/vendor/spdlog/include"
#        "../BlackPearl/vendor/glm"
#        "../BlackPearl/vendor"
#        "../BlackPearl/vendor/assimp/include"
#)
find_library( # Sets the name of the path variable.
        BlackPearl-lib
        NAMES BlackPearl
        HINTS "${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}"
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        )


#if(BUILD_FROM_BLACKPEARL)
#    message(STATUS "---------------BUILD_FROM_BLACKPEARL-------------")
#else ()
    add_library(BlackPearl STATIC IMPORTED)
    set_target_properties(BlackPearl PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../bin/arm64-v8a/libBlackPearl.a)

    add_library(spdlog STATIC IMPORTED)
    set_target_properties(spdlog PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../BlackPearl/vendor/spdlog/bin/Debug-Android-arm64-v8a/spdlog/libspdlog.a)


    add_library(assimpd SHARED IMPORTED)
    set_target_properties(assimpd PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../bin/arm64-v8a/libassimpd.so)

message(STATUS "---------------${CMAKE_CURRENT_SOURCE_DIR}/../bin/arm64-v8a/libBlackPearl.a-------------")
#endif ()
#

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
#target_include_directories(SandBoxAndriod PUBLIC ../BlackPearl/src)


if(BUILD_FROM_BLACKPEARL)
    target_link_libraries(
            SandBoxAndriod
            PRIVATE
            # List libraries link to the target library
            BlackPearl
            EGL
            GLESv3
            android
            log
            spdlog
            #        assimpd
            #        ImGui
    )
else ()
    target_link_libraries(
            SandBoxAndriod
            PRIVATE
            # List libraries link to the target library
            BlackPearl
            EGL
            GLESv3
            android
            log
            spdlog
                    assimpd
            #        ImGui
    )
endif ()