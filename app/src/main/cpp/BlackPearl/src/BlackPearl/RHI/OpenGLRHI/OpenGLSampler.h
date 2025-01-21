#include "../RHISampler.h"
#include "OpenGLState.h"

namespace BlackPearl {
    class Sampler :public RefCounter<ISampler> {
    public:

    public:
        SamplerDesc desc;
        //todo:: ��desc���ظ��ģ�����Ҫɾ��
        FOpenGLSamplerState* samplerState;
        Sampler() {

        }

        /*explicit Sampler(const VulkanContext& context)
            : m_Context(context)
        { }*/

        virtual ~Sampler() override;
        const SamplerDesc& getDesc() const override { return desc; }

    private:
        //const VulkanContext& m_Context;
    };

}