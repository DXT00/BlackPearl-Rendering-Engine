#include "pch.h"
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

}
