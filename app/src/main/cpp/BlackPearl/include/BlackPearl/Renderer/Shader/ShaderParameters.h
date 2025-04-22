#pragma once
#include <vector>
#include "BlackPearl\RHI\RHIBindingLayout.h"
#include "BlackPearl\RHI\RHIBindingSet.h"
namespace BlackPearl {
    class ShaderParameters
    {
    public:
        // struct ShaderBindings {
        std::vector<BindingLayoutHandle> bindingLayouts;
        std::vector<BindingSetHandle> bindingSets;
        // };

        // IInputLayout* inputLayout;
        // ShaderBindings shaderbinding;
         //s[ShaderType::NUM_COMPILE_SHADER_STAGES]

    };
}
