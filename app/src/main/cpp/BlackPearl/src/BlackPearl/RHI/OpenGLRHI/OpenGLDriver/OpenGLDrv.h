#ifdef GE_PLATFORM_WINDOWS
#include "Windows/OpenGLDrvWindows.h"
#elif defined GE_PLATFORM_ANDRIOD
#include "Android/OpenGLDrvAndroid.h"
#else
#error "platform not support"
#endif