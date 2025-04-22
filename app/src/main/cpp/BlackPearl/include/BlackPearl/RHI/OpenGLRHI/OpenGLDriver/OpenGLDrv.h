#ifdef GE_PLATFORM_WINDOWS
#include "Windows/OpenGLDrvWindows.h"
#elif defined GE_PLATFORM_ANDROID
#include "Android/OpenGLDrvAndroid.h"
#else
#error "platform not support"
#endif