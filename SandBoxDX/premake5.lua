project "SandboxDX"
	location "."
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"assets/shaders_hlsl/**.hlsl",
		
	}

	filter { "files:SandboxDX/assets/shaders_hlsl/**.hlsl" }
	filter { "files:**.hlsl" }
		flags("ExcludeFromBuild")
		shadermodel("6.3")
		shaderobjectfileoutput("bin/"..outputdir.."/%{prj.name}/%{file.basename}"..".cso")
		shaderheaderfileoutput("./assets/shsaders_hlsl_h/%{file.basename}.hlsl.h")
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
		"GE_API_D3D12RHI"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE",
			"GE_PLATFORM_WINDOWS",
			"GE_API_D3D12RHI"
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
