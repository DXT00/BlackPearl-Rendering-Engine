#include "pch.h"
#include "MaterialBindingCache.h"

namespace BlackPearl {
	BindingSetHandle MaterialBindingCache::CreateMaterialBindingSet(const Material* material)
	{
		return BindingSetHandle();
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

