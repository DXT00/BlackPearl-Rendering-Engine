#pragma once
#include "RHIDefinitions.h"
#include "RHITexture.h"
#include "RHIBuffer.h"
#include "RHIBindingSet.h"

namespace BlackPearl {

    struct FramebufferAttachment
    {
        ITexture* texture = nullptr;
        //for opengl renderbuffer
        IBuffer* rbo = nullptr;

        ERenderTargetLoadAction LoadAction = ERenderTargetLoadAction::ENoAction;
        ERenderTargetStoreAction StoreAction = ERenderTargetStoreAction::ENoAction;

        TextureSubresourceSet subresources = TextureSubresourceSet(0, 1, 0, 1);
        Format format = Format::UNKNOWN;
        bool isReadOnly = false;

        FramebufferAttachment(ITexture* _texture, ERenderTargetLoadAction InLoadAction = ERenderTargetLoadAction::ELoad, ERenderTargetStoreAction InStoreAction = ERenderTargetStoreAction::EStore) {
            texture = _texture;
            LoadAction = InLoadAction;
            StoreAction = InStoreAction;
            format = (texture) ? texture->getDesc().format : Format::UNKNOWN;
        }
        FramebufferAttachment(IBuffer* _rbo, ERenderTargetLoadAction InLoadAction = ERenderTargetLoadAction::ELoad, ERenderTargetStoreAction InStoreAction = ERenderTargetStoreAction::EStore) {
            rbo = _rbo;
            LoadAction = InLoadAction;
            StoreAction = InStoreAction;
            format = (texture) ? texture->getDesc().format : Format::UNKNOWN;
        }
        FramebufferAttachment() = default;
        FramebufferAttachment& operator=(const FramebufferAttachment& other) {
            if (this != &other) {
                texture = other.texture;
                rbo = other.rbo;
                LoadAction = other.LoadAction;
                StoreAction = other.StoreAction;
                subresources = other.subresources;
                format = other.format;
                isReadOnly = other.isReadOnly;
            } 
            return *this;
        }
        //for opengl renderbuffer
        constexpr FramebufferAttachment& setRenderBuffer(IBuffer* b) { rbo = b; return *this; }

        constexpr FramebufferAttachment& setTexture(ITexture* t) { texture = t; return *this; }
        constexpr FramebufferAttachment& setSubresources(TextureSubresourceSet value) { subresources = value; return *this; }
        constexpr FramebufferAttachment& setArraySlice(uint32_t index) { subresources.baseArraySlice = index; subresources.numArraySlices = 1; return *this; }
        constexpr FramebufferAttachment& setArraySliceRange(uint32_t index, uint32_t count) { subresources.baseArraySlice = index; subresources.numArraySlices = count; return *this; }
        constexpr FramebufferAttachment& setMipLevel(uint32_t level) { subresources.baseMipLevel = level; subresources.numMipLevels = 1; return *this; }
        constexpr FramebufferAttachment& setFormat(Format f) { format = f; return *this; }
        constexpr FramebufferAttachment& setReadOnly(bool ro) { isReadOnly = ro; return *this; }

        [[nodiscard]] bool valid() const { return texture != nullptr; }
    };


    struct FramebufferDesc
    {
        std::vector<FramebufferAttachment> colorAttachments;
        FramebufferAttachment depthAttachment;
        FramebufferAttachment shadingRateAttachment;
        FramebufferAttachment rboAttachment;
        FramebufferAttachment cubeMapAttachment;

        FramebufferDesc& addColorAttachment(const FramebufferAttachment& a) { colorAttachments.push_back(a); return *this; }
        FramebufferDesc& addColorAttachment(ITexture* texture) { 
           
            colorAttachments.push_back(FramebufferAttachment(texture));
            return *this; 
        }
        FramebufferDesc& addColorAttachment(ITexture* texture, TextureSubresourceSet subresources) { colorAttachments.push_back(FramebufferAttachment().setTexture(texture).setSubresources(subresources)); return *this; }
        FramebufferDesc& setDepthAttachment(const FramebufferAttachment& d) { depthAttachment = d; return *this; }
        FramebufferDesc& setDepthAttachment(ITexture* texture) { depthAttachment = FramebufferAttachment().setTexture(texture); return *this; }
        FramebufferDesc& setDepthAttachment(ITexture* texture, TextureSubresourceSet subresources) { depthAttachment = FramebufferAttachment().setTexture(texture).setSubresources(subresources); return *this; }
        FramebufferDesc& setShadingRateAttachment(const FramebufferAttachment& d) { shadingRateAttachment = d; return *this; }
        FramebufferDesc& setShadingRateAttachment(ITexture* texture) { shadingRateAttachment = FramebufferAttachment().setTexture(texture); return *this; }
        FramebufferDesc& setShadingRateAttachment(ITexture* texture, TextureSubresourceSet subresources) { shadingRateAttachment = FramebufferAttachment().setTexture(texture).setSubresources(subresources); return *this; }
       
        uint32_t getRenderTargetCnt() const
        {
            uint32_t cnt = colorAttachments.size();
            if (depthAttachment.valid()) {
                cnt++;
            }
            if (shadingRateAttachment.valid()) {
                cnt++;
            } 
            if (rboAttachment.valid()) {
                cnt++;
            }
            if (cubeMapAttachment.valid()) {
                cnt++;
            }
            return cnt;
        }
        //for opengl renderbuffer rbo
        // https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/05%20Framebuffers/

        FramebufferDesc& setRenderBufferAttachment(const FramebufferAttachment& d) { rboAttachment = d; return *this; }
        FramebufferDesc& setCubeMapAttachment(const FramebufferAttachment& d) { cubeMapAttachment = d; return *this; }

    };

    struct FramebufferInfo
    {
        std::vector<Format> colorFormats;
        Format depthFormat = Format::UNKNOWN;
        uint32_t sampleCount = 1;
        uint32_t sampleQuality = 0;

        FramebufferInfo() = default;
        FramebufferInfo(const FramebufferDesc& desc);

        bool operator==(const FramebufferInfo& other) const
        {
            return formatsEqual(colorFormats, other.colorFormats)
                && depthFormat == other.depthFormat
                && sampleCount == other.sampleCount
                && sampleQuality == other.sampleQuality;
        }
        bool operator!=(const FramebufferInfo& other) const { return !(*this == other); }

    private:
        static bool formatsEqual(const std::vector<Format>& a, const std::vector<Format>& b)
        {
            if (a.size() != b.size()) return false;
            for (size_t i = 0; i < a.size(); i++) if (a[i] != b[i]) return false;
            return true;
        }
    };
    // An extended version of FramebufferInfo that also contains the 'width' and 'height' members.
// It is provided mostly for backward compatibility and convenience reasons, as previously these members
// were available in the regular FramebufferInfo structure.
    struct FramebufferInfoEx : public FramebufferInfo
    {
        uint32_t width = 0;
        uint32_t height = 0;

        FramebufferInfoEx() = default;
        FramebufferInfoEx(const FramebufferDesc& desc);

        [[nodiscard]] RHIViewport getViewport(float minZ = 0.f, float maxZ = 1.f) const
        {
            return RHIViewport(0.f, float(width), 0.f, float(height), minZ, maxZ);
        }
    };

    class IFramebuffer : public IResource
    {
    public:
        [[nodiscard]] virtual const FramebufferDesc& getDesc() const = 0;
        [[nodiscard]] virtual const FramebufferInfoEx& getFramebufferInfo() const = 0;

        //for opengl: delete later 
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
       
    };
    typedef RefCountPtr<IFramebuffer> FramebufferHandle;

}

