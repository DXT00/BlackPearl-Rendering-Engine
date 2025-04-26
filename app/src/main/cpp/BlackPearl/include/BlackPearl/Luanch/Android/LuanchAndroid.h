//
// Created by DXT00 on 2025/4/13.
//
#pragma once
#ifdef GE_PLATFORM_ANDROID

#include "android_native_app_glue.h"

namespace BlackPearl {

    extern pthread_t G_AndroidEventThread;

    namespace LuanchAndroid {
        void InitAndriodThread(struct android_app *state);
        void InitJavaEnv(struct android_app *state);
    };
}
#endif