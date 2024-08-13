# Install script for directory: G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/BlackPearl-Engine")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "G:/AndriodSDK/ndk/21.1.6352462/toolchains/llvm/prebuilt/windows-x86_64/bin/llvm-objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.2.0" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/bin/arm64-v8a/libassimpd.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimpd.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimpd.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "G:/AndriodSDK/ndk/21.1.6352462/toolchains/llvm/prebuilt/windows-x86_64/bin/aarch64-linux-android-strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimpd.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.2.0" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/anim.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/aabb.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/ai_assert.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/camera.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/color4.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/color4.inl"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/cmake-build-debug/BlackPearl/vendor/assimp/code/../include/assimp/config.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/ColladaMetaData.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/commonMetaData.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/defs.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/cfileio.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/light.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/material.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/material.inl"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/matrix3x3.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/matrix3x3.inl"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/matrix4x4.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/matrix4x4.inl"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/mesh.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/ObjMaterial.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/pbrmaterial.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/GltfMaterial.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/postprocess.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/quaternion.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/quaternion.inl"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/scene.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/metadata.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/texture.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/types.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/vector2.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/vector2.inl"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/vector3.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/vector3.inl"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/version.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/cimport.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/importerdesc.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Importer.hpp"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/DefaultLogger.hpp"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/ProgressHandler.hpp"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/IOStream.hpp"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/IOSystem.hpp"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Logger.hpp"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/LogStream.hpp"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/NullLogger.hpp"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/cexport.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Exporter.hpp"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/DefaultIOStream.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/DefaultIOSystem.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/SceneCombiner.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/fast_atof.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/qnan.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/BaseImporter.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Hash.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/ParsingUtils.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/StreamReader.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/StreamWriter.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/StringComparison.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/StringUtils.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/SGSpatialSort.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/GenericProperty.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/SpatialSort.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/SmallVector.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/SmoothingGroups.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/SmoothingGroups.inl"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/StandardShapes.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/RemoveComments.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Subdivision.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Vertex.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/LineSplitter.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/TinyFormatter.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Profiler.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/LogAux.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Bitmap.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/XMLTools.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/IOStreamBuffer.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/CreateAnimMesh.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/XmlParser.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/BlobIOSystem.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/MathFunctions.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Exceptional.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/ByteSwapper.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Compiler/poppack1.h"
    "G:/AA_AndriodLearn/BlackPearl/app/src/main/cpp/BlackPearl/vendor/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

