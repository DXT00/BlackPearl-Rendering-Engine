project "GLEW"
    kind "StaticLib"
   -- language "C"
    language "C"
	staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        --"include/GL/eglew.h",
        "include/GL/glew.h",
     --   "include/GL/glxew.h",
		--"include/GL/wglew.h",
		"src/glew.c",
		--"src/glewinfo.c",
		--"src/visualinfo.c",

    }

	includedirs
	{
		"include"
	}
    
    filter "system:windows"
        systemversion "latest"
        
        
    filter "configurations:Debug"
	    runtime "Debug"
	    symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
