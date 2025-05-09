
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
		"./include/**.h",
		"./src/**.cpp",
		"./src/**.jpg",
		"./src/**.png",
		"./vendor/glm/glm/**.hpp",
		--"./vendor/glm/glm/**.inl",
		"./vendor/stb/**.h",
		"./assets/shaders/hlsl/core/**.h",
		"!**/.vshistory/**"
	}
	excludes {
        "**/.vshistory/**",  -- 排除整个文件夹
    }
	
   removefiles  { ".*/.vshistory/**.cpp" }
   removefiles  { ".*/.vshistory/**.h" }
   removefiles  { ".*/.vshistory/.*" }
    removefiles  { ".*/.vshistory/**.*" }
-- The premake file is only effective for windows by default
   removefiles  { "./src/BlackPearl/Core/Android/**.cpp" }
   removefiles  { "./include/BlackPearl/Core/Android/**.h" }
   removefiles  { "./src/BlackPearl/RHI/OpenGLRHI/OpenGLDriver/Android/**.cpp" }
   removefiles  { "./include/BlackPearl/RHI/OpenGLRHI/OpenGLDriver/Android/**.h" }
	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		--"STB_IMAGE_IMPLEMENTATION"
	}

	includedirs
	{
		"./src",
		"./include",
		"./include/BlackPearl",
		"./vendor/spdlog/include",
		"../%{IncludeDir.GLFW}",
		--"../%{IncludeDir.Glad}",
		--"../%{IncludeDir.ImGui}",
		"../%{IncludeDir.glm}",
		"../%{IncludeDir.stb}",
		"../%{IncludeDir.assimp}",
		"../%{IncludeDir.GLEW}",
		"../%{IncludeDir.hlslShader}",
		"../%{IncludeDir.vulkan}",
		"../%{IncludeDir.directxtex}",
		"../%{IncludeDir.directxmesh}",
		"../%{IncludeDir.ShaderMake}",
		--"../%{IncludeDir.OpenGL}",
		"./vendor",
		"./vendor/GLEW",
		"./assets/shaders/"
	}


	libdirs{
	--"./vendor/assimp_build/code/Debug",
	"./vendor/assimp_build/lib/Debug/windows",
	"./vendor/assimp_build/lib/Release/windows",
	"./vendor/assimp_build/bin/Debug/windows",
	"./vendor/assimp_build/bin/Release/windows",
	"./vendor/assimp_build/bin/Release/windows",
	"./vendor/directxtex_uwp.2022.5.10.1/native/lib",
	"./vendor/directxmesh_desktop_win10.2022.7.30.1/native/lib/x64/Debug",
	"./vendor/directxmesh_desktop_win10.2022.7.30.1/native/lib/x64/Release",
    --"./vendor/imgui/bin/".. outputdir .. "/ImGui",
	"./vendor/vulkan/1.3.290.0/Lib",
	"./vendor/ShaderMake/build/Debug",

	}



   filter { "options:RenderAPI=opengl" }
   --removefiles  { "./src/BlackPearl/RHI/VulkanRHI/**.cpp" }
   --removefiles  { "./src/BlackPearl/RHI/VulkanRHI/**.h" }
   removefiles  { "./src/BlackPearl/Renderer/MasterRendererBak/**.cpp" }
   removefiles  { "./include/BlackPearl/Renderer/MasterRendererBak/**.h" }
   removefiles  { "./include/BlackPearl/Renderer/Lumen/**.h" }
   removefiles  { "./src/BlackPearl/Renderer/Lumen/**.cpp" }
   --removefiles  { "./src/BlackPearl/RayTracing/**.cpp" }
	--removefiles  { "./src/BlackPearl/RayTracing/**.h" }

	--removefiles  { "./src/BlackPearl/ImGui/imgui_impl_opengl3.cpp" }
	--removefiles  { "./src/BlackPearl/ImGui/imgui_impl_opengl3.h" }
   removefiles  { "./src/BlackPearl/RHI/D3D12RHI/**.cpp" }
   removefiles  { "./include/BlackPearl/RHI/D3D12RHI/**.h" }
   removefiles  { "./src/BlackPearl/RHI/VulkanRHI/**.cpp" }
   removefiles  { "./include/BlackPearl/RHI/VulkanRHI/**.h" }
   removefiles  { "./include/BlackPearl/Renderer/Buffer/D3D12Buffer/**.h" }
   removefiles  { "./src/BlackPearl/Renderer/Buffer/D3D12Buffer/**.cpp" }
   removefiles  { "./include/BlackPearl/Renderer/MasterRenderer/D3D12**.h" }
   removefiles  { "./src/BlackPearl/Renderer/MasterRenderer/D3D12**.cpp" }
   removefiles  { "./include/BlackPearl/Renderer/Shader/D3D12Shader/**.h" }
   removefiles  { "./src/BlackPearl/Renderer/Shader/D3D12Shader/**.cpp" }
   removefiles  { "./include/BlackPearl/Renderer/Buffer/VkBuffer/**.h" }
   removefiles  { "./src/BlackPearl/Renderer/Buffer/VkBuffer/**.cpp" }
   removefiles  { "./include/BlackPearl/Renderer/Shader/VkShader/**.h" }
   removefiles  { "./src/BlackPearl/Renderer/Shader/VkShader/**.cpp" }
   removefiles  { "./include/BlackPearl/Renderer/Image/Vk**.h" }
   removefiles  { "./src/BlackPearl/Renderer/Image/Vk**.cpp" }
   removefiles  { "./include/BlackPearl/Renderer/MasterRenderer/Vk**.h" }
   removefiles  { "./src/BlackPearl/Renderer/MasterRenderer/Vk**.cpp" }
   
   removefiles  { ".*/.vshistory/.*" }

   removefiles  { ".*/.vshistory/**.cpp" }
   removefiles  { ".*/.vshistory/**.h" }
   links 
   { 
		"GLFW",
		--"GLEW",
		--"Glad",
		--"ImGui",
		--"ShaderMakeBlob",
		--"assimp",
		"opengl32",
		"assimp-vc142-mtd.lib", 
   }



   includedirs
	{
		"../%{IncludeDir.OpenGL}",
		"./include/BlackPearl/RHI/OpenGLRHI/OpenGLDriver",
		"./include/BlackPearl/RHI/OpenGLRHI",
		"./include/BlackPearl/ImGui",

	}


   defines
	{
		"GLFW_INCLUDE_NONE",
		"GE_API_OPENGL",
		"USE_IMGUI"  --premake 默认 windows 平台， 可以使用imgui
			
	}
   	flags
   	{
		"MultiProcessorCompile"
   	}

   filter { "options:RenderAPI=vulkan" }
   removefiles  { "./src/BlackPearl/RHI/OpenGLRHI/**.cpp" }
   removefiles  { "./include/BlackPearl/RHI/OpenGLRHI/**.h" }
   removefiles  { "./src/BlackPearl/RHI/D3D12RHI/**.cpp" }
   removefiles  { "./include/BlackPearl/RHI/D3D12RHI/**.h" }
   removefiles  { "./include/BlackPearl/Renderer/Buffer/D3D12Buffer/**.h" }
   removefiles  { "./src/BlackPearl/Renderer/Buffer/D3D12Buffer/**.cpp" }
   removefiles  { "./include/BlackPearl/Renderer/MasterRenderer/D3D12**.h" }
   removefiles  { "./src/BlackPearl/Renderer/MasterRenderer/D3D12**.cpp" }
   removefiles  { "./include/BlackPearl/Renderer/Shader/D3D12Shader/**.h" }
   removefiles  { "./src/BlackPearl/Renderer/Shader/D3D12Shader/**.cpp" }
   links 
   { 
		"GLFW",
		"GLEW",
		"Glad",
		"ImGui",
		--"ShaderMakeBlob",

		--"assimp",
		--"opengl32.lib",
		"vulkan-1",
		--"assimp-vc140-mt.lib"
		"assimp-vc142-mtd",
   }
   defines
   {
		"GLFW_INCLUDE_NONE",
		"GE_API_VULKAN"
   }
   	flags
   	{
		"MultiProcessorCompile"
   	}

	   


   filter { "options:RenderAPI=direct3d" }
   removefiles { "./src/BlackPearl/RHI/VulkanRHI/**.cpp" }
   removefiles { "./include/BlackPearl/RHI/VulkanRHI/**.h" }
   removefiles { "./src/BlackPearl/RHI/OpenGLRHI/**.cpp" }
   removefiles { "./include/BlackPearl/RHI/OpenGLRHI/**.h" }
   removefiles { "./include/BlackPearl/Renderer/Buffer/VkBuffer/**.h" }
   removefiles { "./src/BlackPearl/Renderer/Buffer/VkBuffer/**.cpp" }
   removefiles { "./include/BlackPearl/Renderer/Shader/VkShader/**.h" }
   removefiles { "./src/BlackPearl/Renderer/Shader/VkShader/**.cpp" }
   removefiles  { "./include/BlackPearl/Renderer/Image/Vk**.h" }
   removefiles  { "./src/BlackPearl/Renderer/Image/Vk**.cpp" }
   removefiles { "./include/BlackPearl/Renderer/MasterRenderer/Vk**.h" }
   removefiles { "./src/BlackPearl/Renderer/MasterRenderer/Vk**.cpp" }
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
	flags
    {
		"MultiProcessorCompile"
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

