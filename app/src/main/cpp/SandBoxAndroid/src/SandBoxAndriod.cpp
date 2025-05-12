//
// Created by DXT00 on 2024/8/8.
//

#include "BlackPearl/Application.h"
#include "BlackPearl/RHI/DynamicRHI.h"
#include "BlackPearl/EntryPoint.h"
#include "Layers/PbrRenderingLayer.h"
#include "Layers/RHIRenderGraphLayer.h"
enum RenderSample {
    BP_PbrRendering,
    BP_RHIRenderGraphLayer
};

std::string RenderSampleName(RenderSample sample){
    switch (sample) {
        case BP_PbrRendering:
            return "BP_PbrRenderering";
        case BP_RHIRenderGraphLayer:
            return "BP_RHIRenderGraph";
        default:
            return "BP_Unknown";

    }
}
class SandBoxAndroid :public BlackPearl::Application {

public:

    SandBoxAndroid(INSTANCE_HANDLE hInstance, int nShowCmd, BlackPearl::DynamicRHI::Type rhiType, RenderSample renderer, struct android_app* state)
            :Application(hInstance, nShowCmd, rhiType, selectAppVersion(renderer)) {





        const std::string layer_name = RenderSampleName(renderer) + "Layer";

//        if (renderer == BP_VkTest) {
//           // layer = DBG_NEW VkTestLayer(layer_name);
//        }
//        else if (renderer == BP_VkGpuParticles) {
//           // layer = DBG_NEW VkGpuParticlesLayer(layer_name);
//        }
//        else if (renderer == BP_VkRayTracing) {
//           // layer = DBG_NEW VkRayTracingLayer(layer_name);
//        }
        if (renderer == BP_PbrRendering) {
            layer = DBG_NEW PbrRenderingLayer(layer_name);
        }
        if (renderer == BP_RHIRenderGraphLayer) {
            layer = DBG_NEW RHIRenderGraphLayer(layer_name);
        }

    }
    virtual ~SandBoxAndroid() {

    }
    virtual void Init() override{
        BlackPearl::Application::Init();
        if(layer)
            GetLayerManager()->PushLayer(layer);
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
    BlackPearl::Layer* layer = NULL;

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




