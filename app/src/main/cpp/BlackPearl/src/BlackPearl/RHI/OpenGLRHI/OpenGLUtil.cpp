#include "pch.h"
#include "OpenGLUtil.h"
#ifdef GE_PLATFORM_ANDRIOD
#include "../OpenGLRHI/OpenGLDriver/OpenGLES.h"
#else
#include "../OpenGLRHI/OpenGLDriver/OpenGL.h"
#endif
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


    GLenum OpenGLUtil::convertCompareOp(ComparisonFunc op)
	{
		switch (op)
		{
		case ComparisonFunc::Never:
			return GL_LESS;

		case ComparisonFunc::Less:
			return GL_LEQUAL;

		case ComparisonFunc::Equal:
			return GL_EQUAL;

		case ComparisonFunc::LessOrEqual:
			return GL_LEQUAL;

		case ComparisonFunc::Greater:
			return GL_GREATER;

		case ComparisonFunc::NotEqual:
			return GL_NOTEQUAL;

		case ComparisonFunc::GreaterOrEqual:
			return GL_GEQUAL;

		case ComparisonFunc::Always:
			return GL_EQUAL;

		default:
			GE_INVALID_ENUM();
			return GL_LESS;
		}
	}

    GLenum OpenGLUtil::convertStencilOp(StencilOp op)
    {
        switch (op)
        {
            case StencilOp::Keep:
                return GL_KEEP;

            case StencilOp::Zero:
                return GL_ZERO;

            case StencilOp::Replace:
                return GL_REPLACE;

            case StencilOp::IncrementAndClamp:
                return GL_INCR;

            case StencilOp::DecrementAndClamp:
                return GL_DECR;

            case StencilOp::Invert:
                return GL_INVERT;

            case StencilOp::IncrementAndWrap:
                return GL_INCR_WRAP;

            case StencilOp::DecrementAndWrap:
                return GL_DECR_WRAP;

            default:
                GE_INVALID_ENUM();
                return GL_KEEP;
        }
    }
    GLenum OpenGLUtil::convertFillMode(RasterFillMode mode)
    {
        switch (mode)
        {
            case RasterFillMode::Fill:
                return GL_FILL;

            case RasterFillMode::Line:
                return GL_LINE;

            default:
                assert(0);
                return GL_FILL;
        }
    }

    GLenum OpenGLUtil::convertCullMode(RasterCullMode mode)
    {
        switch (mode)
        {
            case RasterCullMode::Back:
                return GL_BACK;

            case RasterCullMode::Front:
                return GL_FRONT;

            case RasterCullMode::None:
                return GL_NONE;

            default:
                assert(0);
                return GL_NONE;
        }
    }

	GLenum OpenGLUtil::convertBlendValue(BlendFactor value)
	{
		switch (value)
		{
		case BlendFactor::Zero:
			return GL_ZERO;

		case BlendFactor::One:
			return GL_ONE;

		case BlendFactor::SrcColor:
			return GL_SRC_COLOR;

		case BlendFactor::OneMinusSrcColor:
			return GL_ONE_MINUS_SRC_COLOR;

		case BlendFactor::SrcAlpha:
			return GL_SRC_ALPHA;

		case BlendFactor::OneMinusSrcAlpha:
			return GL_ONE_MINUS_SRC_ALPHA;

		case BlendFactor::DstAlpha:
			return GL_DST_ALPHA;

		case BlendFactor::OneMinusDstAlpha:
			return GL_ONE_MINUS_DST_ALPHA;

		case BlendFactor::DstColor:
			return GL_DST_COLOR;

		case BlendFactor::OneMinusDstColor:
			return GL_ONE_MINUS_DST_COLOR;

		case BlendFactor::SrcAlphaSaturate:
			return GL_SRC_ALPHA_SATURATE;

		case BlendFactor::ConstantColor:
			return GL_CONSTANT_COLOR;

		case BlendFactor::OneMinusConstantColor:
			return GL_ONE_MINUS_CONSTANT_COLOR;

		case BlendFactor::Src1Color:
			return GL_SRC1_COLOR;

		case BlendFactor::OneMinusSrc1Color:
			return GL_ONE_MINUS_SRC1_COLOR;

		case BlendFactor::Src1Alpha:
			return GL_SRC1_ALPHA;

		case BlendFactor::OneMinusSrc1Alpha:
			return GL_ONE_MINUS_SRC1_ALPHA;

		default:
			assert(0);
			return GL_ONE;
		}
	}

	GLenum OpenGLUtil::convertBlendOp(BlendOp op)
	{
		switch (op)
		{
		case BlendOp::Add:
			return GL_FUNC_ADD;

		case BlendOp::Subrtact:
			return GL_FUNC_SUBTRACT;

		case BlendOp::ReverseSubtract:
			return GL_FUNC_REVERSE_SUBTRACT;

		case BlendOp::Min:
			return GL_MIN;

		case BlendOp::Max:
			return GL_MAX;

		default:
			assert(0);
			return GL_FUNC_ADD;
		}
	}

	GLenum OpenGLUtil::convertColorMask(ColorMask mask)
	{
		return GLenum(uint8_t(mask));
	}

}