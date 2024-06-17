newoption {
	trigger     = "RenderAPI",
	description = "Choose a particular 3D API for rendering",
	default     = "vulkan",
	category    = "Build Options",
	allowed = {
	   { "opengl",    "OpenGL" },
	   { "direct3d",  "Direct3D (Windows only)" },
	   { "vulkan",  "Vulkan" }
	}
}
newoption {
    trigger = "Platform",
    description = "Choose a Device Platform for rendering",
  	default     = "windows",
  	category    = "Build Options",
  	allowed = {
  	   { "windows",  "Windows" },
  	   { "andriod",  "Andriod" }

  	}
}

workspace "BlackPearl"
	--architecture "x86"
	architecture "x86_64"
	startproject "SandboxVK"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

--    filter { "options:Platform=andriod" }
--       architecture "armv7"
--       toolset "clang"
--       ndkversion "25" -- 设置NDK版本号
--       api_level "25" -- 设置最小API级别
--
--       -- 配置模块依赖
--       includedirs {
--          path.join(_PREMAKE_PATH, "modules/android/include"),
--          path.join(_PREMAKE_PATH, "modules/android/sources/cxx-stl/llvm-libc++/include")
--       }
--       links { "log", "android", "EGL", "GLESv2" } -- 链接必要的 Android 库


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "BlackPearl/vendor/GLFW/include"
IncludeDir["Glad"] = "BlackPearl/vendor/Glad/include"
IncludeDir["ImGui"] = "BlackPearl/vendor/imgui"
IncludeDir["glm"] = "BlackPearl/vendor/glm"
IncludeDir["stb"] = "BlackPearl/vendor/stb"
IncludeDir["assimp"] = "BlackPearl/vendor/assimp/include"
IncludeDir["GLEW"] = "BlackPearl/vendor/GLEW/include"
IncludeDir["vulkan"] = "BlackPearl/vendor/VulkanSDK/1.3.236.0/Include"
IncludeDir["directxtex"] = "BlackPearl/vendor/directxtex_uwp.2022.7.30.1/include"
IncludeDir["directxmesh"] = "BlackPearl/vendor/directxmesh_desktop_win10.2022.7.30.1/include"
IncludeDir["ShaderMake"] = "BlackPearl/vendor/ShaderMake/include"

IncludeDir["hlslShader"] = "SandboxDX/assets/shaders_hlsl_h"

include "BlackPearl/vendor/GLFW"
include "BlackPearl/vendor/Glad"
include "BlackPearl/vendor/imgui"
include "BlackPearl/vendor/GLEW"
--include "BlackPearl/vendor/ShaderMake"

include "BlackPearl"
include "SandBox"
include "SandBoxVK"
include "SandBoxDX"
include "SandBoxAndriod"
include "ShaderCompiler"

--include "BlackPearl/vendor/assimp"
-- dofile("BlackPearl/premake5.lua")

-- dofile("SandBox/premake5.lua")

-- dofile("SandBoxVK/premake5.lua")

-- dofile("SandBoxDX/premake5.lua")
