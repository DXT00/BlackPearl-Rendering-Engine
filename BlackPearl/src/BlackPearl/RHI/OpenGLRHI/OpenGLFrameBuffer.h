#pragma once
#include "BlackPearl/RHI/RHIFrameBuffer.h"
namespace BlackPearl {
    //TODO:: 命名为Framebuffer, 通过cmake include
    class OpenGLFramebuffer : public RefCounter<IFramebuffer>
    {
    public:
        FramebufferDesc desc;
        FramebufferInfoEx framebufferInfo;


        std::vector<ResourceHandle> resources;

        bool managed = true;

        explicit OpenGLFramebuffer();
        ~OpenGLFramebuffer() override;
        const FramebufferDesc& getDesc() const override { return desc; }
        const FramebufferInfoEx& getFramebufferInfo() const override { return framebufferInfo; }

        virtual void Bind() override;
        virtual void Unbind() override;

        void BindCubeMapColorAttachments(int attachmentId, int face);

    private:
        unsigned int m_Fbo;

    private:
        void _BindColorAttachments();

        void _BindDepthAttachments();
        void _BindRenderbuffer();

    };

}
