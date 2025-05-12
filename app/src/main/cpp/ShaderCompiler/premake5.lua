project "ShaderCompiler"
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
	}
	excludes {
        "**/.vshistory/**",  -- 排除整个文件夹
    }
	
	removefiles  { ".*/.vshistory/**.cpp" }
    removefiles  { ".*/.vshistory/**.h" }
    removefiles  { ".*/.vshistory/.*" }
	removefiles  { ".*/.vshistory/**.*" }

	includedirs
	{
		"../BlackPearl/include",
		"../BlackPearl/src",
		"../BlackPearl/vendor/spdlog/include",
		"../BlackPearl/vendor/GLFW/include",
		"../%{IncludeDir.glm}",
	}

	libdirs{
        -- "../BlackPearl/vendor/vulkan/1.3.290.0/Lib"
    }

	defines
	{
		"GE_SHADERCOMPILE"
	}

	links
	{
		"BlackPearl",
	}
	



	filter "system:windows"
		systemversion "latest"

		defines
		{
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
