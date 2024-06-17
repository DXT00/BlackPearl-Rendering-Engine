project "SandBoxAndriod"
	location "."
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

--      architecture "armv7"
--      toolset "clang"
--      --ndkversion "25" -- 设置NDK版本号
--      api_level "25" -- 设置最小API级别

    local ndk_path = os.getenv("NDK_ROOT")
       if not ndk_path then
           error("ANDROID_NDK environment variable not set.")
       end
       local ndk_include_path = ndk_path
       includedirs {
            ndk_include_path .. "toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include"
       }
       print(ndk_path)
	files
	{
		"src/**.h",
		"src/**.cpp",
		"assets/shaders/spv/**.spv",
        "assets/shaders/**.frag",
        "assets/shaders/**.vert"
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
		--"../%{IncludeDir.vulkan}",
		"../%{IncludeDir.ImGui}",
		-- "%{IncludeDir.glslShader}",
		--"../%{IncludeDir.vulkan}"
		--"packages/directxtex_uwp.2022.7.30.1/native/lib/x64/Debug"

	}

    local ndk_lib_path = ndk_path
	libdirs{
       ndk_lib_path .. "toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/lib/x86_64-linux-android",

    }

	defines
	{
		"GE_API_OPENGL"
	}

	links
	{
		"BlackPearl",
		"andriod",
		"EGL",
		"GLESv2"
		--"d3d12.lib",
		--"dxgi.lib",
		--"dxguid.lib",
		--"vulkan-1.lib"
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
