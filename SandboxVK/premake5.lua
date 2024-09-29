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
		"assets/shaders/spv/**.spv",
        "assets/shaders/**.frag",
        "assets/shaders/**.vert"
	}


	--filter { "files:src/BlackPearl/RHI/OpenGLRHI/**.cpp" }
	--filter { "files:src/BlackPearl/RHI/OpenGLRHI/**.h" }
	--filter { "files:src/BlackPearl/Renderer/Buffer/D3D12Buffer/**.h" }
	--filter { "files:src/BlackPearl/Renderer/Buffer/D3D12Buffer/**.cpp" }
	--filter { "files:src/BlackPearl/Renderer/MasterRenderer/D3D12**.h" }
	--filter { "files:src/BlackPearl/Renderer/MasterRenderer/D3D12**.cpp" }
	--filter { "files:src/BlackPearl/Renderer/Shader/D3D12Shader/**.h" }
	--filter { "files:src/BlackPearl/Renderer/Shader/D3D12Shader/**.cpp" }
	includedirs
	{
		"../BlackPearl/vendor/spdlog/include",
		"../BlackPearl/src",
		"../BlackPearl/vendor",
		"../BlackPearl/assets/shaders",
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
