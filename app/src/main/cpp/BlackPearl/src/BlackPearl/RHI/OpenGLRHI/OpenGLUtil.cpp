#include "pch.h"
#include "OpenGLUtil.h"
namespace BlackPearl {

    GLint OpenGLUtil::convertSamplerAddressMode(SamplerAddressMode mode)
    {
        switch (mode)
        {
        case SamplerAddressMode::ClampToEdge:
            return GL_CLAMP_TO_EDGE;

        case SamplerAddressMode::Repeat:
            return  GL_MIRRORED_REPEAT;

        case SamplerAddressMode::ClampToBorder:
            return  UGL_CLAMP_TO_BORDER;

        case SamplerAddressMode::MirroredRepeat:
            return  GL_MIRRORED_REPEAT;

        case SamplerAddressMode::MirrorClampToEdge:
            return  GL_MIRRORED_REPEAT;


        default:
            GE_INVALID_ENUM()
                return  GL_MIRRORED_REPEAT;
        }
    }
}