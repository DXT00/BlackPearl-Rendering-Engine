# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("native-render" LANGUAGES CXX)


file(GLOB src-files
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h

)

add_definitions(-DGE_API_GLES)
add_definitions(-DGE_PLATFORM_ANDRIOD)

# 创建静态库目标
add_library( # Sets the name of the library.
        native-render

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${src-files}
)


target_include_directories(
        native-render
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/SandBoxAndriod/src
        ${CMAKE_SOURCE_DIR}/BlackPearl/src
        ${CMAKE_SOURCE_DIR}/BlackPearl/vendor/spdlog/include
        ${CMAKE_SOURCE_DIR}/BlackPearl/vendor/glm
        ${CMAKE_SOURCE_DIR}/BlackPearl//vendor/assimp/include
)



# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
#target_include_directories(SandBoxAndriod PUBLIC ../BlackPearl/src)
target_link_libraries(
        native-render
        PRIVATE
        # List libraries link to the target library
        SandBoxAndriod

    )