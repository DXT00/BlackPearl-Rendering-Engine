# Install script for directory: D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibassimp5.2.0-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp_build/lib/Debug/assimp-vc142-mtd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp_build/lib/Release/assimp-vc142-mt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp_build/lib/MinSizeRel/assimp-vc142-mt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp_build/lib/RelWithDebInfo/assimp-vc142-mt.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibassimp5.2.0x" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp_build/bin/Debug/assimp-vc142-mtd.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp_build/bin/Release/assimp-vc142-mt.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp_build/bin/MinSizeRel/assimp-vc142-mt.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp_build/bin/RelWithDebInfo/assimp-vc142-mt.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/anim.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/aabb.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/ai_assert.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/camera.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/color4.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/color4.inl"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp_build/code/../include/assimp/config.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/ColladaMetaData.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/commonMetaData.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/defs.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/cfileio.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/light.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/material.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/material.inl"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/matrix3x3.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/matrix3x3.inl"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/matrix4x4.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/matrix4x4.inl"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/mesh.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/ObjMaterial.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/pbrmaterial.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/GltfMaterial.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/postprocess.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/quaternion.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/quaternion.inl"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/scene.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/metadata.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/texture.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/types.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/vector2.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/vector2.inl"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/vector3.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/vector3.inl"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/version.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/cimport.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/importerdesc.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Importer.hpp"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/DefaultLogger.hpp"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/ProgressHandler.hpp"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/IOStream.hpp"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/IOSystem.hpp"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Logger.hpp"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/LogStream.hpp"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/NullLogger.hpp"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/cexport.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Exporter.hpp"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/DefaultIOStream.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/DefaultIOSystem.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/SceneCombiner.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/fast_atof.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/qnan.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/BaseImporter.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Hash.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/ParsingUtils.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/StreamReader.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/StreamWriter.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/StringComparison.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/StringUtils.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/SGSpatialSort.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/GenericProperty.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/SpatialSort.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/SmallVector.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/SmoothingGroups.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/SmoothingGroups.inl"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/StandardShapes.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/RemoveComments.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Subdivision.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Vertex.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/LineSplitter.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/TinyFormatter.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Profiler.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/LogAux.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Bitmap.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/XMLTools.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/IOStreamBuffer.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/CreateAnimMesh.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/XmlParser.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/BlobIOSystem.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/MathFunctions.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Exceptional.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/ByteSwapper.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Base64.hpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Compiler/poppack1.h"
    "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp_build/code/Debug/assimp-vc142-mtd.pdb")
  endif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/AA_LEARN_CODING/LearnOpenGL/BlackPearl-Rendering-Engine/BlackPearl/vendor/assimp_build/code/RelWithDebInfo/assimp-vc142-mt.pdb")
  endif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
endif()

