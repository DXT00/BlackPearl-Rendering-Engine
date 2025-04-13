//
// Created by DXT00 on 2024/8/8.
//

#include "BlackPearl/Application.h"
#include "BlackPearl/RHI/DynamicRHI.h"
#include "BlackPearl/EntryPoint.h"
#include <android_native_app_glue.h>


enum RenderSample {

    BP_RHIRenderGraphLayer
};


class SandBoxAndroid :public BlackPearl::Application {

public:

    SandBoxAndroid(INSTANCE_HANDLE hInstance, int nShowCmd, BlackPearl::DynamicRHI::Type rhiType, RenderSample renderer, struct android_app* state)
            :Application(hInstance, nShowCmd, rhiType, selectAppVersion(renderer)) {





        BlackPearl::Layer* layer = NULL;
        const std::string layer_name = renderer + "Layer";

//        if (renderer == BP_VkTest) {
//           // layer = DBG_NEW VkTestLayer(layer_name);
//        }
//        else if (renderer == BP_VkGpuParticles) {
//           // layer = DBG_NEW VkGpuParticlesLayer(layer_name);
//        }
//        else if (renderer == BP_VkRayTracing) {
//           // layer = DBG_NEW VkRayTracingLayer(layer_name);
//        }
        if (renderer == BP_RHIRenderGraphLayer) {
          //  layer = DBG_NEW VkRHIRenderGraphLayer(layer_name);
        }
        GetLayerManager()->PushLayer(layer);
    }
    virtual ~SandBoxAndroid() {

    }

private:
    BlackPearl::AppVersion selectAppVersion(RenderSample sample) {
//        if (sample <= BP_VkRayTracing) {
//            return BlackPearl::AppVersion::VERSION_0_0;
//        }
//        else {
//            return BlackPearl::AppVersion::VERSION_1_0;
//        }

        return BlackPearl::AppVersion::VERSION_1_0;
    }
};

BlackPearl::Application* BlackPearl::CreateApplication(INSTANCE_HANDLE hInstance, int nShowCmd, struct android_app* state) {
    return DBG_NEW SandBoxAndroid(hInstance, nShowCmd,  BlackPearl::DynamicRHI::Type::OpenGL, RenderSample::BP_RHIRenderGraphLayer, state);
}






//SandBoxAndriod::SandBoxAndriod()
//{
//    const std::string a= "aa";
//
//
//}
//BlackPearl::Application* SandBoxAndriod::m_App = nullptr;
//
// void SandBoxAndriod::add(){
//    int a = 1;
//}
//BlackPearl::Application* SandBoxAndriod::GetInstance(){
//
////    if (m_App == nullptr)
////    {
////#ifdef GE_PLATFORM_WINDOWS
////        m_App = nullptr;
////#else
////        m_App = new BlackPearl::Application();
////
////#endif
////    }
//    return m_App;
//
//}
//void SandBoxAndriod::DestroyInstance()
//{
//    //LOGCATE("MyGLRenderContext::DestroyInstance");
//    if (m_App)
//    {
//        delete m_App;
//        m_App = nullptr;
//    }
//
//}


struct Engine {
    struct android_app* app;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
};

// 初始化 OpenGL ES
static int init_gl(Engine* engine) {
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint format, numConfigs;
    EGLConfig config;

    engine->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(engine->display, nullptr, nullptr);
    eglChooseConfig(engine->display, attribs, &config, 1, &numConfigs);
    eglGetConfigAttrib(engine->display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);
    engine->surface = eglCreateWindowSurface(engine->display, config, engine->app->window, nullptr);
    engine->context = eglCreateContext(engine->display, config, nullptr, nullptr);

    if (eglMakeCurrent(engine->display, engine->surface, engine->surface, engine->context) == EGL_FALSE) {
      //  LOGI("Failed to make GL context current");
        return -1;
    }

    eglQuerySurface(engine->display, engine->surface, EGL_WIDTH, &engine->width);
    eglQuerySurface(engine->display, engine->surface, EGL_HEIGHT, &engine->height);
    return 0;
}

// 处理输入事件
static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    Engine* engine = (Engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
       // LOGI("Touch at: (%.1f, %.1f)", x, y);
        return 1;
    }
    return 0;
}

// 主循环
static void handle_cmd(struct android_app* app, int32_t cmd) {
    Engine* engine = (Engine*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (engine->app->window != nullptr) {
                init_gl(engine);
                glClearColor(0.2f, 0.3f, 0.8f, 1.0f); // 设置蓝色背景
            }
            break;
        case APP_CMD_TERM_WINDOW:
            eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroySurface(engine->display, engine->surface);
            eglDestroyContext(engine->display, engine->context);
            eglTerminate(engine->display);
            break;
    }
}




