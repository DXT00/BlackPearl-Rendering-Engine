//
// Created by DXT00 on 2024/8/8.
//

#ifndef BLACKPEARL_SANDBOXANDRIOD_H
#define BLACKPEARL_SANDBOXANDRIOD_H
#include "BlackPearl.h"


class SandBoxAndriod{
public:
    SandBoxAndriod();

    static void add();
    static BlackPearl::Application* GetInstance();
    static void DestroyInstance();
    virtual ~SandBoxAndriod() {};
    void Run(){}
private:
    static BlackPearl::Application* m_App;
};


#endif //BLACKPEARL_SANDBOXANDRIOD_H
