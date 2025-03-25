#include "pch.h"
#include "MaterialBindingCache.h"
#include "Material.h"
#include "BlackPearl/RHI/RHIBindingSet.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/Core.h"
#include "MaterialManager.h"
namespace BlackPearl {
    extern MaterialManager* g_materialManager;


    std::unordered_map<uint32_t, MaterialBindingItem> GMaterialBindingSetCache;

    MaterialBindingCache::MaterialBindingCache()
    {
    }

    BindingSetHandle MaterialBindingCache::CreateMaterialBindingSet(DeviceHandle device, const Material* material, BindingLayoutHandle bindingLayout)
	{
       
        BindingSetDesc bindingSetDesc;
        bindingSetDesc.trackLiveness = false;// m_TrackLiveness;

        for (const auto& item : material->materialTemplate->GetBindingDesc())
        {
            BindingSetItem setItem;

            switch (item.resource)
            {
            case MaterialResource::ConstantBuffer:
                setItem = BindingSetItem::ConstantBuffer(
                    item.slot,
                    material->materialConstants);
                break;

            case MaterialResource::Sampler:
                setItem = BindingSetItem::Sampler(
                    item.slot,
                    material->GetSampler());
                break;

            case MaterialResource::DiffuseTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->GetTextureMaps()->diffuseTextureMap);
                break;

            case MaterialResource::SpecularTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->GetTextureMaps()->specularTextureMap);
                break;

            case MaterialResource::NormalTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->GetTextureMaps()->normalTextureMap);
                break;

            case MaterialResource::EmissiveTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->GetTextureMaps()->emissionTextureMap);
                break;

            case MaterialResource::OcclusionTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->GetTextureMaps()->aoMap);
                break;

            case MaterialResource::TransmissionTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->GetTextureMaps()->transmissionTexture);
                break;

            default:
                GE_ASSERT(0, "MaterialBindingCache: unknown MaterialResource value");
              //  GE_CORE_INFO("MaterialBindingCache: unknown MaterialResource value {0}" ,item.resource);
                return nullptr;
            }

            bindingSetDesc.bindings.push_back(setItem);
        }
        return device->createBindingSet(bindingSetDesc, bindingLayout);
	}

    BindingLayoutHandle MaterialBindingCache::CreateMaterialBindingLayout(
        DeviceHandle device,
        const Material* material)
    {

        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.registerSpace = material->materialTemplate->GetRegisterSpace();

        for (const auto& item : material->materialTemplate->GetBindingDesc())
        {
            RHIBindingLayoutItem layoutItem{};
            layoutItem.slot = item.slot;

            switch (item.resource)
            {
            case MaterialResource::ConstantBuffer:
                layoutItem.type = RHIResourceType::RT_ConstantBuffer;
                break;
            case MaterialResource::DiffuseTexture:
            case MaterialResource::SpecularTexture:
            case MaterialResource::NormalTexture:
            case MaterialResource::EmissiveTexture:
            case MaterialResource::OcclusionTexture:
            case MaterialResource::TransmissionTexture:
                layoutItem.type = RHIResourceType::RT_Texture_SRV;
                break;
            case MaterialResource::Sampler:
                layoutItem.type = RHIResourceType::RT_Sampler;
                break;
            default:
                // GE_CORE_ERROR("MaterialBindingCache: unknown MaterialResource value {0}", item.resource);
                 //log::error("MaterialBindingCache: unknown MaterialResource value (%d)", item.resource);
                GE_CORE_ERROR("MaterialBindingCache: unknown MaterialResource value");
                return nullptr;
            }

            layoutDesc.bindings.push_back(layoutItem);
        }

        return device->createBindingLayout(layoutDesc);
    }

    BindingSetItem MaterialBindingCache::GetTextureBindingSetItem(uint32_t slot, const TextureHandle& texture) 
    {
        return BindingSetItem::Texture_SRV(slot, texture? texture.Get() : g_materialManager->systemTextures.whiteTexture.Get());
    }

	//BindingSetItem MaterialBindingCache::GetTextureBindingSetItem(uint32_t slot, const std::shared_ptr<LoadedTexture>& texture) const
	//{
	//	return BindingSetItem::Texture_SRV(slot, texture && texture->texture ? texture->texture.Get() : m_FallbackTexture.Get());
	//}
    // same as opengl set uniform


	IBindingLayout* MaterialBindingCache::GetLayout() const
	{
		return m_BindingLayout;
	}

    MaterialBindingItem& MaterialBindingCache::GetOrCreateMaterialBindingSet(DeviceHandle device, const Material* material)
	{
		//std::lock_guard<std::mutex> lockGuard(m_Mutex);
        //TODO:: m_BindingSets ¼Ó hash
        uint32_t key = material->GetId();
        if (GMaterialBindingSetCache.find(key) != GMaterialBindingSetCache.end()) {
                return GMaterialBindingSetCache[key];
        }

        BindingLayoutHandle bindingLayout = CreateMaterialBindingLayout(device, material);
        BindingSetHandle bindingSet = CreateMaterialBindingSet(device, material, bindingLayout);

        GMaterialBindingSetCache[key] = MaterialBindingItem(bindingSet, bindingLayout);
		return GMaterialBindingSetCache[key];
	}

	void MaterialBindingCache::Clear()
	{
	}

}

