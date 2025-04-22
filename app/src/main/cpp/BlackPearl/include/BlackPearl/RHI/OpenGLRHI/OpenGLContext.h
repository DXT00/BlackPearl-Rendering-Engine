#pragma once
namespace BlackPearl {
    struct FPlatformOpenGLDevice;
	class OpenGLContext
	{

	public:
		/** Underlying platform-specific data */
		FPlatformOpenGLDevice* PlatformDevice;
	};
}
