#include "pch.h"
#if GE_API_VULKAN

#include "VkShader.h"

namespace BlackPearl {
	EShader::~EShader()
	{
		//if (shaderModule && !baseShader) // do not destroy the module if this is a derived specialization shader or a library entry
		//{
		//	m_Context.device.destroyShaderModule(shaderModule, m_Context.allocationCallbacks);
		//	shaderModule = vk::ShaderModule();
		//}
	}
	void EShader::getBytecode(const void** ppBytecode, size_t* pSize) const {
		// we don't save these for vulkan
		if (ppBytecode) *ppBytecode = nullptr;
		if (pSize) *pSize = 0;
	}
	void ShaderTable::setRayGenerationShader(const char* exportName, IBindingSet* bindings) {}
	int ShaderTable::addMissShader(const char* exportName, IBindingSet* bindings) { return 0; }
	int ShaderTable::addHitGroup(const char* exportName, IBindingSet* bindings ) { return 0; }
	int ShaderTable::addCallableShader(const char* exportName, IBindingSet* bindings) { return 0; }
	void ShaderTable::clearMissShaders() {}
	void ShaderTable::clearHitShaders() {}
	void ShaderTable::clearCallableShaders()  {}
	uint32_t ShaderTable::getNumEntries() const { return 0; }
}
#endif