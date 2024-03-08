#pragma once
#include<vector>
#include "../RefCountPtr.h"
#include "../RHIShader.h"
#include "../RHIResources.h"

#include "VkContext.h"
#include <vulkan/vulkan.h>
namespace BlackPearl {
    //先用EShader定义，防止和OpenGL定义的Shader重复
    class EShader : public RefCounter<IShader>
    {
    public:
        ShaderDesc desc;

        VkShaderModule shaderModule;
        VkShaderStageFlagBits stageFlagBits{};

        // Shader specializations are just references to the original shader module
        // plus the specialization constant array.
        ResourceHandle baseShader; // Could be a Shader or ShaderLibrary
        std::vector<ShaderSpecialization> specializationConstants;

        explicit EShader(const VulkanContext& context)
            : desc(ShaderType::None)
            , m_Context(context)
        { }

        ~EShader() override;
        const ShaderDesc& getDesc() const override { return desc; }
        void getBytecode(const void** ppBytecode, size_t* pSize) const override;

    private:
        const VulkanContext& m_Context;
    };
	
}


