project "assets"
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
		"shaders/glsl/**.glsl",
		"shaders/hlsl/**.hlsl",
		"shaders/hlsl/**.h",

		--"../assets/shaders_hlsl/**.hlsl",
		
	}
	excludes {
        "**/.vshistory/**",  -- 排除整个文件夹
    }
	
	removefiles  { ".*/.vshistory/**.cpp" }
	removefiles  { ".*/.vshistory/**.h" }
	removefiles  { ".*/.vshistory/.*" }
	--filter { "files:../assets/shaders_hlsl/**.hlsl" }
	--filter { "files:**.hlsl" }
	--	flags("ExcludeFromBuild")
	--	shadermodel("6.3")
	--	shaderobjectfileoutput("bin/"..outputdir.."/%{prj.name}/%{file.basename}"..".cso")
	--	shaderheaderfileoutput("../assets/shaders_hlsl_h/%{file.basename}.hlsl.h")
	--	shadervariablename("g_p".."%{file.basename}")
	--	shaderentry ""
	--filter {}
	--files { "../assets/shaders_hlsl_h/**.hlsl.h"; }

	includedirs
	{
		
		"shaders/glsl/**.glsl",
		"shaders/hlsl/**.hlsl",
		"shaders/hlsl/**.h",
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

