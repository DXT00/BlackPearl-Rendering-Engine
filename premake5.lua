workspace "BlackPearl"
	architecture "x86"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "BlackPearl/vendor/GLFW/include"
IncludeDir["Glad"] = "BlackPearl/vendor/Glad/include"
IncludeDir["ImGui"] = "BlackPearl/vendor/imgui"
IncludeDir["glm"] = "BlackPearl/vendor/glm"
IncludeDir["stb"] = "BlackPearl/vendor/stb"
IncludeDir["assimp"] = "BlackPearl/vendor/assimp/include"

include "BlackPearl/vendor/GLFW"
include "BlackPearl/vendor/Glad"
include "BlackPearl/vendor/imgui"
--include "BlackPearl/vendor/assimp"

project "BlackPearl"
	location "BlackPearl"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "BlackPearl/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.jpg",
		"%{prj.name}/src/**.png",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb/**.h",
		
		--"%{prj.name}/vendor/assimp/include/assimp/**.hpp",
		--"%{prj.name}/vendor/assimp/include/assimp/**.h",
		--"%{prj.name}/vendor/assimp/include/assimp/**.inl",
		--"%{prj.name}/vendor/assimp/include/assimp/**.cpp",

		--"%{prj.name}/vendor/assimp/contrib/irrXML/**",
		--"%{prj.name}/vendor/assimp/contrib/zlib/**",
		--"%{prj.name}/vendor/assimp/contrib/rapidjson/include/**",
	}
	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"STB_IMAGE_IMPLEMENTATION"
	}
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.assimp}"
	}
	libdirs{
	"%{prj.name}/vendor/assimp_build/code/Debug"
	}
	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		--"assimp",
		"opengl32.lib",
		"assimp-vc140-mt.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		    "GE_PLATFORM_WINDOWS",
			--"HZ_PLATFORM_WINDOWS",
			--"HZ_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
		}

	
	filter "configurations:Debug"
		defines "GE_DEBUG"
		runtime "Debug"
		symbols "on" --meaning it will be the debug version of the library

	filter "configurations:Release"
		defines "GE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "GE_DIST"
		runtime "Release"
		optimize "on"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"BlackPearl/vendor/spdlog/include",
		"BlackPearl/src",
		"BlackPearl/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.Glad}",
	
	}

	links
	{
		"BlackPearl",
		
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		"GLFW_INCLUDE_NONE",
			"GE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "GE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "GE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "GE_DIST"
		runtime "Release"
		optimize "on"
