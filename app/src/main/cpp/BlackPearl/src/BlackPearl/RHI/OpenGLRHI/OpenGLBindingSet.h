#pragma once
#include "../RHIBindingSet.h"
#include "OpenGLContext.h"
#include "OpenGLBuffer.h"
namespace BlackPearl {
	class BindingSet : public RefCounter<IBindingSet>
	{
    public:
        virtual const BindingSetDesc* getDesc() const override { return &desc; };  // returns nullptr for descriptor tables
        virtual IBindingLayout* getLayout() const override { return layout; }


        BindingSetDesc desc;
        BindingLayoutHandle layout;
        // TODO: move pool to the context instead
  /*      VkDescriptorPool descriptorPool;
        VkDescriptorSet descriptorSet;*/

        std::vector<ResourceHandle> resources;
        std::vector<Buffer*> volatileConstantBuffers;

        std::vector<uint16_t> bindingsThatNeedTransitions;

        explicit BindingSet(const OpenGLContext& context)
            : m_Context(context)
        {
        }

        ~BindingSet() override;


    private:
        const OpenGLContext& m_Context;
    };
	


}