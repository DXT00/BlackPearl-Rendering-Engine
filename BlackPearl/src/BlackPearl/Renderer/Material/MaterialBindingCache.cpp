#include "pch.h"
#include "MaterialBindingCache.h"

namespace BlackPearl {
	BindingSetHandle MaterialBindingCache::CreateMaterialBindingSet(const Material* material)
	{
        assert(0);
    /*    nvrhi::BindingSetDesc bindingSetDesc;
        bindingSetDesc.trackLiveness = m_TrackLiveness;

        for (const auto& item : m_BindingDesc)
        {
            nvrhi::BindingSetItem setItem;

            switch (item.resource)
            {
            case MaterialResource::ConstantBuffer:
                setItem = nvrhi::BindingSetItem::ConstantBuffer(
                    item.slot,
                    material->materialConstants);
                break;

            case MaterialResource::Sampler:
                setItem = nvrhi::BindingSetItem::Sampler(
                    item.slot,
                    m_Sampler);
                break;

            case MaterialResource::DiffuseTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->baseOrDiffuseTexture);
                break;

            case MaterialResource::SpecularTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->metalRoughOrSpecularTexture);
                break;

            case MaterialResource::NormalTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->normalTexture);
                break;

            case MaterialResource::EmissiveTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->emissiveTexture);
                break;

            case MaterialResource::OcclusionTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->occlusionTexture);
                break;

            case MaterialResource::TransmissionTexture:
                setItem = GetTextureBindingSetItem(item.slot, material->transmissionTexture);
                break;

            default:
                log::error("MaterialBindingCache: unknown MaterialResource value (%d)", item.resource);
                return nullptr;
            }

            bindingSetDesc.bindings.push_back(setItem);
        }*/

        return m_Device->createBindingSet(bindingSetDesc, m_BindingLayout);
	}

	BindingSetItem MaterialBindingCache::GetTextureBindingSetItem(uint32_t slot, const std::shared_ptr<LoadedTexture>& texture) const
	{
		return BindingSetItem::Texture_SRV(slot, texture && texture->texture ? texture->texture.Get() : m_FallbackTexture.Get());
	}

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

