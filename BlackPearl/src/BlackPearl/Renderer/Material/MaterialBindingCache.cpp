#include "pch.h"
#include "MaterialBindingCache.h"
#include "BlackPearl/RHI/RHIBindingSet.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/Core.h"
namespace BlackPearl {
	BindingSetHandle MaterialBindingCache::CreateMaterialBindingSet(const Material* material)
	{
       
        BindingSetDesc bindingSetDesc;
        bindingSetDesc.trackLiveness = m_TrackLiveness;

        for (const auto& item : m_BindingDesc)
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
                    m_Sampler);
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

        return m_Device->createBindingSet(bindingSetDesc, m_BindingLayout);
	}

    BindingSetItem MaterialBindingCache::GetTextureBindingSetItem(uint32_t slot, const TextureHandle& texture) const
    {
        return BindingSetItem::Texture_SRV(slot, texture? texture.Get() : m_FallbackTexture.Get());
    }

	//BindingSetItem MaterialBindingCache::GetTextureBindingSetItem(uint32_t slot, const std::shared_ptr<LoadedTexture>& texture) const
	//{
	//	return BindingSetItem::Texture_SRV(slot, texture && texture->texture ? texture->texture.Get() : m_FallbackTexture.Get());
	//}
    // same as opengl set uniform
	MaterialBindingCache::MaterialBindingCache(IDevice* device, ShaderType shaderType, uint32_t registerSpace, const std::vector<MaterialResourceBinding>& bindings, ISampler* sampler, ITexture* fallbackTexture, bool trackLiveness)
	{
        m_Device = device;

        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = shaderType;
        layoutDesc.registerSpace = registerSpace;

        for (const auto& item : bindings)
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
                return;
            }

            layoutDesc.bindings.push_back(layoutItem);
        }

        m_BindingLayout = m_Device->createBindingLayout(layoutDesc);
	}

	IBindingLayout* MaterialBindingCache::GetLayout() const
	{
		return nullptr;
	}

	IBindingSet* MaterialBindingCache::GetMaterialBindingSet(const Material* material)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		BindingSetHandle& bindingSet = m_BindingSets[material];

		if (bindingSet)
			return bindingSet;

		bindingSet = CreateMaterialBindingSet(material);

		return bindingSet;
	}

	void MaterialBindingCache::Clear()
	{
	}

}

