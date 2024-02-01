project "SandboxVK"
	location "."
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"assets/shaders/spv/**.spv"
	}


	includedirs
	{
		"../BlackPearl/vendor/spdlog/include",
		"../BlackPearl/src",
		"../BlackPearl/vendor",
		"../%{IncludeDir.glm}",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.assimp}",
		"../%{IncludeDir.Glad}",
		"../%{IncludeDir.vulkan}",
		--"../%{IncludeDir.ImGui}",
		-- "%{IncludeDir.glslShader}",
		"../%{IncludeDir.vulkan}"
		--"packages/directxtex_uwp.2022.7.30.1/native/lib/x64/Debug"

	}

	libdirs{
        "../BlackPearl/vendor/VulkanSDK/1.3.236.0/Lib"
    }

	defines
	{
		"GE_API_VULKAN"
	}

	links
	{
		"BlackPearl",
		"d3d12.lib",
		"dxgi.lib",
		"dxguid.lib",
		"vulkan-1.lib"
		--"directxtex.lib",
	}
	



	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE",
			"GLFW_INCLUDE_VULKAN",
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
