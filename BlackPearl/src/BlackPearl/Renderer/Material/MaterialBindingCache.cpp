#include "pch.h"
#include "MaterialBindingCache.h"
#include "BlackPearl/RHI/RHIBindingSet.h"

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
                GE_CORE_INFO("MaterialBindingCache: unknown MaterialResource value (%d)", item.resource);
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

	MaterialBindingCache::MaterialBindingCache(IDevice* device, ShaderType shaderType, uint32_t registerSpace, const std::vector<MaterialResourceBinding>& bindings, ISampler* sampler, ITexture* fallbackTexture, bool trackLiveness)
	{
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

