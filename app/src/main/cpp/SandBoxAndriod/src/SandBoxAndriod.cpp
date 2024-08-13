//
// Created by DXT00 on 2024/8/8.
//

#include "SandBoxAndriod.h"

SandBoxAndriod* SandBoxAndriod::m_App = nullptr;
SandBoxAndriod* SandBoxAndriod::GetInstance(){

    if (m_App == nullptr)
    {
        m_App = new SandBoxAndriod();
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