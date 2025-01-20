#include "pch.h"
#include "glad/glad.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLBuffer.h"
#include "OpenGLTexture.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Core.h"
#include "OpenGLDevice.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
namespace BlackPearl {
    // GL_MAX_DRAW_BUFFERS value
    GLint GMaxOpenGLDrawBuffers = 0;
    OpenGLFramebuffer::OpenGLFramebuffer()
    {
        glGenFramebuffers(1, &m_Fbo);

    }
    OpenGLFramebuffer::~OpenGLFramebuffer()
    {
    }
    void OpenGLFramebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
        _BindColorAttachments();
        _BindDepthAttachments();
        _BindRenderbuffer();

    }
    void OpenGLFramebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
    }

    void OpenGLFramebuffer::BindCubeMapColorAttachments(int attachmentId, int face)
    {
        const auto& attach = desc.cubeMapAttachment;
        Texture* texture = static_cast<Texture*>(attach.texture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentId, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture->GetRendererID(), 0);
    }


    void OpenGLFramebuffer::_BindColorAttachments()
    {
        for (int i = 0; i < desc.colorAttachments.size();i++) {
            //将它附加到当前绑定的帧缓冲对象
            const auto& attach = desc.colorAttachments[i];
            Texture* texture = static_cast<Texture*>(attach.texture);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->GetRendererID(), 0);
            GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
        }
    }

    void OpenGLFramebuffer::_BindDepthAttachments()
    {
        const auto& attach = desc.depthAttachment;
        Texture* texture = static_cast<Texture*>(attach.texture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->GetRendererID(), 0);
        GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
    }
    
    void OpenGLFramebuffer::_BindRenderbuffer()
    {
        OpenGLRenderBuffer* renderbuffer = static_cast<OpenGLRenderBuffer*>(desc.rboAttachment.rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer->rbo);

        // Use a single rbo for both depth and stencil buffer.
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, renderbuffer->width, renderbuffer->height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer->rbo);

        GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }


    void Device::BindPendingFramebuffer(FOpenGLContextState& ContextState) {
        assert((GMaxRHIFeatureLevel >= ERHIFeatureLevel::SM5) || !PendingState.bFramebufferSetupInvalid);

        if (ContextState.Framebuffer != PendingState.Framebuffer)
        {
            if (PendingState.Framebuffer)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, PendingState.Framebuffer);

                FOpenGL::ReadBuffer(PendingState.FirstNonzeroRenderTarget >= 0 ? GL_COLOR_ATTACHMENT0 + PendingState.FirstNonzeroRenderTarget : GL_NONE);
                GLenum DrawFramebuffers[c_MaxRenderTargets];
                const GLint MaxDrawBuffers = GMaxOpenGLDrawBuffers;

                for (int32_t RenderTargetIndex = 0; RenderTargetIndex < MaxDrawBuffers; ++RenderTargetIndex)
                {
                    DrawFramebuffers[RenderTargetIndex] = PendingState.RenderTargets[RenderTargetIndex] ? GL_COLOR_ATTACHMENT0 + RenderTargetIndex : GL_NONE;
                }
                FOpenGL::DrawBuffers(MaxDrawBuffers, DrawFramebuffers);
            }
            else
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                FOpenGL::ReadBuffer(GL_BACK);
                FOpenGL::DrawBuffer(GL_BACK);
            }

            ContextState.Framebuffer = PendingState.Framebuffer;
        }
    }

    void Device::BindPendingComputeShaderState(FOpenGLContextState& ContextState, IShader* ComputeShader)
    {
    }

    void Device::UpdateRasterizerStateInOpenGLContext(FOpenGLContextState& ContextState)
    {
    }
    void Device::UpdateDepthStencilStateInOpenGLContext(FOpenGLContextState& ContextState)
    {
    }
    void Device::UpdateScissorRectInOpenGLContext(FOpenGLContextState& ContextState)
    {
        if (ContextState.bScissorEnabled != PendingState.bScissorEnabled)
        {
            if (PendingState.bScissorEnabled)
            {
                glEnable(GL_SCISSOR_TEST);
            }
            else
            {
                glDisable(GL_SCISSOR_TEST);
            }
            ContextState.bScissorEnabled = PendingState.bScissorEnabled;
        }

        if (PendingState.bScissorEnabled &&
            ContextState.Scissor != PendingState.Scissor)
        {
            assert(PendingState.Scissor.minX <= PendingState.Scissor.maxX);
            assert(PendingState.Scissor.minY <= PendingState.Scissor.maxY);
            glScissor(PendingState.Scissor.minX, PendingState.Scissor.minY, PendingState.Scissor.maxX - PendingState.Scissor.minX, PendingState.Scissor.maxY - PendingState.Scissor.minY);
            ContextState.Scissor = PendingState.Scissor;
        }
    }
    void Device::UpdateViewportInOpenGLContext(FOpenGLContextState& ContextState)
    {
        if (ContextState.Viewport != PendingState.Viewport)
        {
            //@todo the viewport defined by glViewport does not clip, unlike the viewport in d3d
            // Set the scissor rect to the viewport unless it is explicitly set smaller to emulate d3d.
            glViewport(
                PendingState.Viewport.minX,
                PendingState.Viewport.minY,
                PendingState.Viewport.maxX - PendingState.Viewport.minX,
                PendingState.Viewport.maxY - PendingState.Viewport.minY);

            ContextState.Viewport = PendingState.Viewport;
        }

        if (ContextState.DepthMinZ != PendingState.DepthMinZ || ContextState.DepthMaxZ != PendingState.DepthMaxZ)
        {
            FOpenGL::DepthRange(PendingState.DepthMinZ, PendingState.DepthMaxZ);
            ContextState.DepthMinZ = PendingState.DepthMinZ;
            ContextState.DepthMaxZ = PendingState.DepthMaxZ;
        }
    }
}