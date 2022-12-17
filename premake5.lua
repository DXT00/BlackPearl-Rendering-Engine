workspace "BlackPearl"
	--architecture "x86"
	architecture "x86_64"
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
IncludeDir["GLEW"] = "BlackPearl/vendor/GLEW/include"
IncludeDir["vulkan"] = "BlackPearl/vendor/VulkanSDK/1.3.236.0/Include"
IncludeDir["directxtex"] = "BlackPearl/vendor/directxtex_uwp.2022.7.30.1/include"
IncludeDir["directxmesh"] = "BlackPearl/vendor/directxmesh_desktop_win10.2022.7.30.1/include"

IncludeDir["hlslShader"] = "SandboxDX/assets/shaders_hlsl_h"



include "BlackPearl/vendor/GLFW"
include "BlackPearl/vendor/Glad"
include "BlackPearl/vendor/imgui"
include "BlackPearl/vendor/GLEW"
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
		"Sandbox/assets/shaders_hlsl_h/**.h"
		--"%{prj.name}/vendor/GLEW/**.h",
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
		--"STB_IMAGE_IMPLEMENTATION"
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
		"%{IncludeDir.assimp}",
		"%{IncludeDir.GLEW}",
		"%{IncludeDir.hlslShader}",
		"%{IncludeDir.vulkan}",
		"%{IncludeDir.directxtex}",
		"%{IncludeDir.directxmesh}",

		"BlackPearl/vendor",
		"BlackPearl/vendor/GLEW"
	}

	libdirs{
	"%{prj.name}/vendor/assimp_build/code/Debug",
	"%{prj.name}/vendor/assimp_build/lib/Debug",
	"%{prj.name}/vendor/assimp_build/lib/Release",
	"%{prj.name}/vendor/assimp_build/bin/Debug",
	"%{prj.name}/vendor/assimp_build/bin/Release",
	"%{prj.name}/vendor/assimp_build/bin/Release",
	"%{prj.name}/vendor/directxtex_uwp.2022.5.10.1/native/lib",
	"%{prj.name}/vendor/directxmesh_desktop_win10.2022.7.30.1/native/lib/x64/Debug",
	"%{prj.name}/vendor/directxmesh_desktop_win10.2022.7.30.1/native/lib/x64/Release",

	"%{prj.name}/vendor/VulkanSDK/1.3.236.0/Lib",

	}

	links 
	{ 
		"GLFW",
		"GLEW",
		"Glad",
		"ImGui",
		--"assimp",
		"opengl32.lib",
		"vulkan-1.lib",
		--"assimp-vc140-mt.lib"
		"assimp-vc142-mtd.lib",


	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		    "GE_PLATFORM_WINDOWS",
			--"HZ_PLATFORM_WINDOWS",
			--"HZ_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
			--"GE_D3D12RHI",


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
	--kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/assets/shaders_hlsl/**.hlsl",
		
	}

	filter { "files:Sandbox/assets/shaders_hlsl/**.hlsl" }
	filter { "files:**.hlsl" }
		flags("ExcludeFromBuild")
		shadermodel("6.3")
		shaderobjectfileoutput("bin/"..outputdir.."/%{prj.name}/%{file.basename}"..".cso")
		shaderheaderfileoutput("./assets/shaders_hlsl_h/%{file.basename}.hlsl.h")
		shadervariablename("g_p".."%{file.basename}")
		shaderentry ""
	filter {}
	files { "%{prj.name}/assets/shaders_hlsl_h/**.hlsl.h"; }

	includedirs
	{
		"BlackPearl/vendor/spdlog/include",
		"BlackPearl/src",
		"BlackPearl/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glslShader}",
		"%{IncludeDir.vulkan}"

	}


	links
	{
		"BlackPearl",
		"d3d12.lib",
		"dxgi.lib",
		"dxguid.lib",
		--"vulkan-1.lib",
		--"directxtex.lib",
	}


	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE",
			"GE_PLATFORM_WINDOWS",
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




project "SandboxDX"
	location "SandboxDX"
	--kind "ConsoleApp"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/assets/shaders_hlsl/**.hlsl",
		
	}

	filter { "files:SandboxDX/assets/shaders_hlsl/**.hlsl" }
	filter { "files:**.hlsl" }
		flags("ExcludeFromBuild")
		shadermodel("6.3")
		shaderobjectfileoutput("bin/"..outputdir.."/%{prj.name}/%{file.basename}"..".cso")
		shaderheaderfileoutput("./assets/shaders_hlsl_h/%{file.basename}.hlsl.h")
		shadervariablename("g_p".."%{file.basename}")
		shaderentry ""
	filter {}
	files { "%{prj.name}/assets/shaders_hlsl_h/**.hlsl.h"; }

	includedirs
	{
		"BlackPearl/vendor/spdlog/include",
		"BlackPearl/src",
		"BlackPearl/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glslShader}",
		--"packages/directxtex_uwp.2022.7.30.1/native/lib/x64/Debug"

	}
	
	links
	{
		"BlackPearl",
		"d3d12.lib",
		"dxgi.lib",
		"dxguid.lib",
		--"directxtex.lib",
	}
	
	
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE",
			"GE_PLATFORM_WINDOWS",
			"GE_D3D12RHI",
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


project "SandboxVK"
	location "SandboxVK"
	--kind "ConsoleApp"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/assets/shaders_hlsl/**.hlsl",
		
	}

	filter { "files:SandboxVK/assets/shaders_hlsl/**.hlsl" }
	filter { "files:**.hlsl" }
		flags("ExcludeFromBuild")
		shadermodel("6.3")
		shaderobjectfileoutput("bin/"..outputdir.."/%{prj.name}/%{file.basename}"..".cso")
		shaderheaderfileoutput("./assets/shaders_hlsl_h/%{file.basename}.hlsl.h")
		shadervariablename("g_p".."%{file.basename}")
		shaderentry ""
	filter {}
	files { "%{prj.name}/assets/shaders_hlsl_h/**.hlsl.h"; }

	includedirs
	{
		"BlackPearl/vendor/spdlog/include",
		"BlackPearl/src",
		"BlackPearl/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glslShader}",
		--"packages/directxtex_uwp.2022.7.30.1/native/lib/x64/Debug"

	}
	
	links
	{
		"BlackPearl",
		"d3d12.lib",
		"dxgi.lib",
		"dxguid.lib",
		--"directxtex.lib",
	}
	
	defines
	{
		"VULKAN_API"
	}


	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE",
			"GE_PLATFORM_WINDOWS",
			"VULKAN_API"
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
