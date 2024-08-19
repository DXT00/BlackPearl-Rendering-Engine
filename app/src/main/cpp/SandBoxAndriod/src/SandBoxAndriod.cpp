//
// Created by DXT00 on 2024/8/8.
//

#include "SandBoxAndriod.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/RHI/DynamicRHI.h"

SandBoxAndriod::SandBoxAndriod()
{
    const std::string a= "aa";


}
BlackPearl::Application* SandBoxAndriod::m_App = nullptr;
BlackPearl::Application* SandBoxAndriod::GetInstance(){

    if (m_App == nullptr)
    {
#ifdef GE_PLATFORM_WINDOWS
        m_App = nullptr;
#else
        m_App = new BlackPearl::Application();

#endif
    }
    return m_App;

}
void SandBoxAndriod::DestroyInstance()
{
    //LOGCATE("MyGLRenderContext::DestroyInstance");
    if (m_App)
    {
        delete m_App;
        m_App = nullptr;
    }

}