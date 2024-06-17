# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("SandBoxAndriod")

find_library(BlackPearlLib1 BlackPearl.lib ../BlackPearl/bin/Debug-windows-x86_64/BlackPearl/ NO_DEFAULT_PATH)
message("aaa BlackPearlLib = " ${BlackPearlLib1})
file(GLOB src-files
        ../BlackPearl/vendor/spdlog/include
        ../BlackPearl/src
        ../BlackPearl/vendor
        ./src/native-lib.cpp
)

add_library( # Sets the name of the library.
        SandBoxAndriod

        # Sets the library as a shared library.
        STATIC

        # Provides a relative path to your source file(s).
        ${src-files}
)


target_include_directories(SandBoxAndriod PUBLIC ../BlackPearl/src)
# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
#target_include_directories(SandBoxAndriod PUBLIC ../BlackPearl/src)
target_link_libraries(
        SandBoxAndriod
        # List libraries link to the target library
        BlackPearl
        EGL
        GLESv3
        android
        log)