project "Sandbox"
	location "."
	kind "ConsoleApp"
	--kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	--targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	--objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	targetdir ("./")
	objdir ("./")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"assets/shaders_hlsl/**.hlsl",
		
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
		"../BlackPearl/vendor/spdlog/include",
		"../BlackPearl/src",
		"../BlackPearl/vendor",
		"../%{IncludeDir.glm}",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.assimp}",
		"../%{IncludeDir.Glad}",
		"../%{IncludeDir.glslShader}",
		"../%{IncludeDir.vulkan}"

	}

	defines
	{
		"GE_API_OPENGL"
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

