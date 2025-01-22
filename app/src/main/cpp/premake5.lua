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

workspace "BlackPearl"
	--architecture "x86"
	architecture "x86_64"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	-- local renderApi = _OPTIONS["RenderAPI"]
	print(_OPTIONS["RenderAPI"])
	if _OPTIONS["RenderAPI"] == "opengl" then
		startproject "Sandbox"
		include "SandBox"
		include "SandBoxVK"
		print("set startproject to Sandbox.")
	elseif _OPTIONS["RenderAPI"] == "vulkan"  then
		startproject "SandboxVK"
		include "SandBoxVK"
		print("set startproject to SandboxVK.")
	elseif _OPTIONS["RenderAPI"] == "direct3d" then
		startproject "SandboxDX"
		include "SandBoxDX"
		print("set startproject to SandboxDX.")
	else
		print("Invalid RenderAPI option. Defaulting to SandboxVK.")
		startproject "SandboxVK"
		include "SandBoxVK"
	end
	



-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "BlackPearl/vendor/GLFW/include"
IncludeDir["Glad"] = "BlackPearl/vendor/Glad/include"
IncludeDir["ImGui"] = "BlackPearl/vendor/imgui"
IncludeDir["glm"] = "BlackPearl/vendor/glm"
IncludeDir["stb"] = "BlackPearl/vendor/stb"
IncludeDir["assimp"] = "BlackPearl/vendor/assimp/include"
IncludeDir["GLEW"] = "BlackPearl/vendor/GLEW/include"
IncludeDir["vulkan"] = "BlackPearl/vendor/vulkan/1.3.290.0/Include"
IncludeDir["directxtex"] = "BlackPearl/vendor/directxtex_uwp.2022.7.30.1/include"
IncludeDir["directxmesh"] = "BlackPearl/vendor/directxmesh_desktop_win10.2022.7.30.1/include"
IncludeDir["ShaderMake"] = "BlackPearl/vendor/ShaderMake/include"

IncludeDir["hlslShader"] = "SandboxDX/assets/shaders_hlsl_h"
IncludeDir["OpenGL"] = "BlackPearl/vendor/OpenGL/include"



include "BlackPearl/vendor/GLFW"
include "BlackPearl/vendor/Glad"
include "BlackPearl/vendor/imgui"
include "BlackPearl/vendor/GLEW"
--include "BlackPearl/vendor/ShaderMake"

include "BlackPearl"

include "ShaderCompiler"

