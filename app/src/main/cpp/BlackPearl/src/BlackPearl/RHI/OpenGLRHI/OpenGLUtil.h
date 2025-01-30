#pragma once
#include "BlackPearl/RHI/RHISampler.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
namespace BlackPearl {

    class OpenGLUtil
    {
    public:
        static GLint convertSamplerAddressMode(SamplerAddressMode mode);

        static GLenum convertCompareOp(ComparisonFunc op);

        static GLenum convertStencilOp(StencilOp op);

        static GLenum convertFillMode(RasterFillMode mode);

        static GLenum convertCullMode(RasterCullMode mode);
    };
}
