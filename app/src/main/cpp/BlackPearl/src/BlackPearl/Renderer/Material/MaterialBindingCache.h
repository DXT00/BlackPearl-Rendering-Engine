#pragma once
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/RHI/RHIBindingLayout.h"
#include "BlackPearl/Renderer/SceneType.h"
#include "MaterialResouceBinding.h"
#include <mutex>


namespace BlackPearl {

    class Material;
    

    struct MaterialBindingItem {
        MaterialBindingItem() {
            bindingSet = nullptr;
            bindingLayout = nullptr;
        }
        MaterialBindingItem(BindingSetHandle set, BindingLayoutHandle layout) {
            bindingSet = set;
            bindingLayout = layout;
        }
        BindingSetHandle bindingSet;
        BindingLayoutHandle bindingLayout;
    };


    class MaterialBindingCache
    {
    private:
        MaterialBindingCache();
        BindingLayoutHandle m_BindingLayout;
        //TODO:: 获取hash值
        //mateiralId --> bindingSet
        std::unordered_map<uint32_t, BindingSetHandle> m_BindingSets;
        ShaderType m_ShaderType;
        std::vector<MaterialResourceBinding> m_BindingDesc;
        // 这里要一个系统默认贴图
        static TextureHandle m_FallbackTexture;
        //SamplerHandle m_Sampler;
        std::mutex m_Mutex;
       // bool m_TrackLiveness;

        static BindingSetHandle CreateMaterialBindingSet(DeviceHandle device, const Material* material, BindingLayoutHandle layout);
        static BindingLayoutHandle CreateMaterialBindingLayout(DeviceHandle device, const Material* material);

       // BindingSetItem GetTextureBindingSetItem(uint32_t slot, const std::shared_ptr<LoadedTexture>& texture) const;
        static BindingSetItem GetTextureBindingSetItem(uint32_t slot, const TextureHandle& texture);


    public:
       

        IBindingLayout* GetLayout() const;
        void Clear();

        static MaterialBindingItem& GetOrCreateMaterialBindingSet(DeviceHandle device,const Material* material);

    };

}
// 自定义 std::hash<MyClass>
namespace std {
   /* template <>
    struct hash<BlackPearl::Material> {
        size_t operator()(const BlackPearl::Material& obj) const {
            return std::hash<int>()(obj.GetId()) ^ (std::hash<std::string>()(obj.name) << 1);
        }
    };*/

    template <>
    struct hash<BlackPearl::BindingSetHandle> {
        size_t operator()(const BlackPearl::BindingSetHandle& obj) const {
            return std::hash<size_t>()(obj.Get()->getDesc()->bindings.size());
        }
    };
}