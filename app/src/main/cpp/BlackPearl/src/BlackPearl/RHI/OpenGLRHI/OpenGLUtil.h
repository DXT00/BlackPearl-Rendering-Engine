#pragma once
#include "BlackPearl/RHI/RHISampler.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
#include "BlackPearl/RHI/RHIInputLayout.h"
namespace BlackPearl {

    class OpenGLUtil
    {
    public:
        static GLint convertSamplerAddressMode(SamplerAddressMode mode);

        static GLenum convertCompareOp(ComparisonFunc op);

        static GLenum convertStencilOp(StencilOp op);

        static GLenum convertFillMode(RasterFillMode mode);

        static GLenum convertCullMode(RasterCullMode mode);

        static GLenum convertBlendValue(BlendFactor value);

        static GLenum convertBlendOp(BlendOp op);

        static GLenum convertColorMask(ColorMask mask);

        static GLenum convertTextureDimension(TextureDimension dim);

        static GLenum convertTextureAccess(TextureAccess access);

        static std::pair<GLenum, GLenum> convertTextureFormatAndDataType(Format format);

        static GLenum convertTextureInnerFormat(Format format);

        static 	GLint convertTextureFilter(FilterMode filter);

        static GLint convertTextureWarp(SamplerAddressMode warp);

        static uint32_t convertInputElementDataType(const ElementDataType& type);

        //static uint32_t ShaderDataTypeToBufferType(const ElementDataType& type);

    };
}
