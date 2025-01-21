#pragma once
#include "BlackPearl/RHI/RHISampler.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
namespace BlackPearl {

    class OpenGLUtil
    {
    public:
        static GLint OpenGLUtil::convertSamplerAddressMode(SamplerAddressMode mode);
    };
}
