#pragma once
#if GE_API_VULKAN

#include<vector>
#include "../RefCountPtr.h"
#include "../RHIShader.h"
#include "../RHIResources.h"
#include "../RHIShaderLibrary.h"
#include "VkPipeline.h"

#include "VkContext.h"
#include <vulkan/vulkan_core.h>
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
            : desc(ShaderType::All)
            , m_Context(context)
        { }

        ~EShader() override;
        const ShaderDesc& getDesc() const override { return desc; }
        void getBytecode(const void** ppBytecode, size_t* pSize) const override;

    private:
        const VulkanContext& m_Context;
    };

    class ShaderTable : public RefCounter<IShaderTable>
    {
    public:
        RefCountPtr<RayTracingPipeline> pipeline;

        int rayGenerationShader = -1;
        std::vector<uint32_t> missShaders;
        std::vector<uint32_t> callableShaders;
        std::vector<uint32_t> hitGroups;

        uint32_t version = 0;

        ShaderTable(const VulkanContext& context, RayTracingPipeline* _pipeline)
            : pipeline(_pipeline)
            , m_Context(context)
        { }

        void setRayGenerationShader(const char* exportName, IBindingSet* bindings = nullptr) override;
        int addMissShader(const char* exportName, IBindingSet* bindings = nullptr) override;
        int addHitGroup(const char* exportName, IBindingSet* bindings = nullptr) override;
        int addCallableShader(const char* exportName, IBindingSet* bindings = nullptr) override;
        void clearMissShaders() override;
        void clearHitShaders() override;
        void clearCallableShaders() override;
        IRayTracingPipeline* getPipeline() override { return pipeline; }
        uint32_t getNumEntries() const;

    private:
        const VulkanContext& m_Context;

        bool verifyShaderGroupExists(const char* exportName, int shaderGroupIndex) const;
    };


    class ShaderLibrary : public RefCounter<IShaderLibrary>
    {
    public:
       VkShaderModule shaderModule;

        explicit ShaderLibrary(const VulkanContext& context)
            : m_Context(context)
        { }

        virtual ~ShaderLibrary() override;
        void getBytecode(const void** ppBytecode, size_t* pSize) const override;
        ShaderHandle getShader(const char* entryName, ShaderType shaderType) override;
    private:
        const VulkanContext& m_Context;
    };
}

#endif
