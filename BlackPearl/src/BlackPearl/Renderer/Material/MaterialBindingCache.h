#pragma once
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include <mutex>
namespace BlackPearl {

    struct LoadedTexture
    {
        TextureHandle texture;
       // TextureAlphaMode alphaMode = TextureAlphaMode::UNKNOWN;
        uint32_t originalBitsPerPixel = 0;
       // DescriptorHandle bindlessDescriptor;
        std::string path;
        std::string mimeType;
    };


    enum class MaterialResource
    {
        ConstantBuffer,
        Sampler,
        DiffuseTexture,
        SpecularTexture,
        NormalTexture,
        EmissiveTexture,
        OcclusionTexture,
        TransmissionTexture
    };

    struct MaterialResourceBinding
    {
        MaterialResource resource;
        uint32_t slot; // type depends on resource
    };

    class MaterialBindingCache
    {
    private:
        DeviceHandle m_Device;
        BindingLayoutHandle m_BindingLayout;
        std::unordered_map<const Material*, BindingSetHandle> m_BindingSets;
        ShaderType m_ShaderType;
        std::vector<MaterialResourceBinding> m_BindingDesc;
        TextureHandle m_FallbackTexture;
        SamplerHandle m_Sampler;
        std::mutex m_Mutex;
        bool m_TrackLiveness;

        BindingSetHandle CreateMaterialBindingSet(const Material* material);
        BindingSetItem GetTextureBindingSetItem(uint32_t slot, const std::shared_ptr<LoadedTexture>& texture) const;

    public:
        MaterialBindingCache(
            IDevice* device,
            ShaderType shaderType,
            uint32_t registerSpace,
            const std::vector<MaterialResourceBinding>& bindings,
            ISampler* sampler,
            ITexture* fallbackTexture,
            bool trackLiveness = true);

        IBindingLayout* GetLayout() const;
        IBindingSet* GetMaterialBindingSet(const Material* material);
        void Clear();
    };

}