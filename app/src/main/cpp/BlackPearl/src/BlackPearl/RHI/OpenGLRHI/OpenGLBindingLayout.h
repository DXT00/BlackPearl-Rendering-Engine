#pragma once
#include "../RHIBindingLayoutDesc.h"
#include "OpenGLContext.h"

namespace BlackPearl {

	struct GLBindingItem {
		uint32_t slot;
		RHIResourceType resoursType;
		ShaderType shaderStage;
	};
	class BindingLayout : public RefCounter<IBindingLayout>
	{
	public:
		RHIBindingLayoutDesc desc;
		RHIBindlessLayoutDesc bindlessDesc;
		bool isBindless;

		/* std::vector<VkDescriptorSetLayoutBinding> vulkanLayoutBindings;

		 VkDescriptorSetLayout descriptorSetLayout;*/

		 // descriptor pool size information per binding set
		std::vector<GLBindingItem> glLayoutBindings;

		BindingLayout(const OpenGLContext& context, const RHIBindingLayoutDesc& desc);
		BindingLayout(const OpenGLContext& context, const RHIBindlessLayoutDesc& desc);

		~BindingLayout() override;
		const RHIBindingLayoutDesc* getDesc() const override { return isBindless ? nullptr : &desc; }
		const RHIBindlessLayoutDesc* getBindlessDesc() const override { return isBindless ? &bindlessDesc : nullptr; }

		// generate the descriptor set layout
		//VkResult bake();

	private:
		const OpenGLContext& m_Context;
	};


}