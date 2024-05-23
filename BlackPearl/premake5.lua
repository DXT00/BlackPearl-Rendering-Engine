
project "BlackPearl"
	location "../"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "./src/pch.cpp"

	files
	{
		"./src/**.h",
		"./src/**.cpp",
		"./src/**.jpg",
		"./src/**.png",
		"./vendor/glm/glm/**.hpp",
		"./vendor/glm/glm/**.inl",
		"./vendor/stb/**.h"
	}
	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		--"STB_IMAGE_IMPLEMENTATION"
	}
	includedirs
	{
		"./src",
		"./vendor/spdlog/include",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.Glad}",
		"../%{IncludeDir.ImGui}",
		"../%{IncludeDir.glm}",
		"../%{IncludeDir.stb}",
		"../%{IncludeDir.assimp}",
		"../%{IncludeDir.GLEW}",
		"../%{IncludeDir.hlslShader}",
		"../%{IncludeDir.vulkan}",
		"../%{IncludeDir.directxtex}",
		"../%{IncludeDir.directxmesh}",
		"../%{IncludeDir.ShaderMake}",

		"./vendor",
		"./vendor/GLEW"
	}

	libdirs{
	"./vendor/assimp_build/code/Debug",
	"./vendor/assimp_build/lib/Debug",
	"./vendor/assimp_build/lib/Release",
	"./vendor/assimp_build/bin/Debug",
	"./vendor/assimp_build/bin/Release",
	"./vendor/assimp_build/bin/Release",
	"./vendor/directxtex_uwp.2022.5.10.1/native/lib",
	"./vendor/directxmesh_desktop_win10.2022.7.30.1/native/lib/x64/Debug",
	"./vendor/directxmesh_desktop_win10.2022.7.30.1/native/lib/x64/Release",
    --"./vendor/imgui/bin/".. outputdir .. "/ImGui",
	"./vendor/VulkanSDK/1.3.236.0/Lib",
	"./vendor/ShaderMake/build/Debug",

	}

   filter { "options:RenderAPI=opengl" }
   links 
   { 
		"GLFW",
		"GLEW",
		"Glad",
		"ImGui",
		--"ShaderMakeBlob",
		--"assimp",
		"opengl32.lib",
		"assimp-vc142-mtd.lib", 
   }
   defines
	{
		"GLFW_INCLUDE_NONE",
		"GE_API_OPENGL"
	}

   filter { "options:RenderAPI=vulkan" }
   links 
   { 
		"GLFW",
		"GLEW",
		"Glad",
		"ImGui",
		--"ShaderMakeBlob",

		--"assimp",
		"opengl32.lib",
		"vulkan-1.lib",
		--"assimp-vc140-mt.lib"
		"assimp-vc142-mtd.lib", 
   }
   defines
   {
		"GLFW_INCLUDE_NONE",
		"GE_API_VULKAN"
   }

   filter { "options:RenderAPI=direct3d" }
	links 
	{ 
		"GLFW",
		"GLEW",
		"Glad",
		"ImGui",
		--"ShaderMakeBlob",
		--"assimp",
		"opengl32.lib",
		--"assimp-vc140-mt.lib"
		"assimp-vc142-mtd.lib",

	}
	defines
	{
		"GLFW_INCLUDE_NONE",
		"GE_API_D3D12"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		    "GE_PLATFORM_WINDOWS",
			--"HZ_PLATFORM_WINDOWS",
			--"HZ_BUILD_DLL",
			--"GLFW_INCLUDE_VULKAN",
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

