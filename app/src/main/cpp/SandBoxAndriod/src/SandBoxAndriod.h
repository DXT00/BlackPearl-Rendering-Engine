//
// Created by DXT00 on 2024/8/8.
//

#ifndef BLACKPEARL_SANDBOXANDRIOD_H
#define BLACKPEARL_SANDBOXANDRIOD_H
#include "BlackPearl.h"
#include "BlackPearl/Application.h"

class SandBoxAndriod : public BlackPearl::Application {
public:
    SandBoxAndriod()
    : BlackPearl::Application(0, BlackPearl::DynamicRHI::Type::OpenGL, ""){}

    static SandBoxAndriod* GetInstance();
    static void DestroyInstance();
    virtual ~SandBoxAndriod() = default;
private:
    static SandBoxAndriod *m_App;
};


#endif //BLACKPEARL_SANDBOXANDRIOD_H
