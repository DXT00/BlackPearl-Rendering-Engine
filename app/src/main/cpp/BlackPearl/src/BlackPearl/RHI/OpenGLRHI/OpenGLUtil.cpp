#include "pch.h"
#include "RHI/OpenGLRHI/OpenGLUtil.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLDrv.h"

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
//Android gles not support Dual-Source Blending https://lumverse.feishu.cn/docx/WRcAdjLx7oea5hxe1gvcgRtzngY
#ifdef GE_PLATFORM_WINDOWS
		case BlendFactor::Src1Color:
			return GL_SRC1_COLOR;

		case BlendFactor::OneMinusSrc1Color:
			return GL_ONE_MINUS_SRC1_COLOR;

		case BlendFactor::Src1Alpha:
			return GL_SRC1_ALPHA;

		case BlendFactor::OneMinusSrc1Alpha:
			return GL_ONE_MINUS_SRC1_ALPHA;
#endif
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

	GLenum OpenGLUtil::convertTextureDimension(TextureDimension dim)
	{
		switch (dim)
		{
		case TextureDimension::Texture1D:
			return GL_TEXTURE_1D;
		case TextureDimension::Texture1DArray:
			return GL_TEXTURE_1D_ARRAY;
		case TextureDimension::Texture2D:
			return GL_TEXTURE_2D;
		case TextureDimension::Texture2DArray:
			return GL_TEXTURE_2D_ARRAY;
		case TextureDimension::TextureCube:
			return GL_TEXTURE_CUBE_MAP;
		case TextureDimension::TextureCubeArray:
			return GL_TEXTURE_CUBE_MAP_ARRAY;
		case TextureDimension::Texture2DMS:
			return GL_TEXTURE_2D_MULTISAMPLE;
		case TextureDimension::Texture2DMSArray:
			return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
		case TextureDimension::Texture3D:
			return GL_TEXTURE_3D;
		case TextureDimension::RenderBuffer:
			return GL_RENDERBUFFER;
		default:
		{
			GE_ASSERT(0, "Unknown texture dimension");
			break;

		}
		}
	}


	//return <format, dataType>
	std::pair<GLenum, GLenum> OpenGLUtil::convertTextureFormatAndDataType(Format format)
	{
		//https://www.khronos.org/opengl/wiki/OpenGL_Type
		//GL_UNSIGNED_BYTE --> 8 bit
		//GL_BYTE --> 8 bit

		//GL_UNSIGNED_INT --> 32bit
		// GL_INT --> 32bit

		//GL_UNSIGNED_SHORT -->16bit
		//GL_SHORT --> 16bit

		switch (format)
		{
		case Format::R8_UNORM:
			return std::make_pair<GLenum, GLenum>(GL_RED, GL_UNSIGNED_BYTE);
		case Format::R32_FLOAT:
			return std::make_pair<GLenum, GLenum>(GL_RED, GL_FLOAT);

		case Format::RG16_FLOAT:
			return std::make_pair<GLenum, GLenum>(GL_RG, GL_FLOAT);

		case Format::RGB8_UNORM:
			return std::make_pair<GLenum, GLenum>(GL_RGB, GL_UNSIGNED_BYTE);
		case Format::RGB8_FLOAT:
		case Format::RGB32_FLOAT:
		case Format::RGB16_FLOAT:
			return std::make_pair<GLenum, GLenum>(GL_RGB, GL_FLOAT);

		case Format::RGBA8_UNORM:
			return std::make_pair<GLenum, GLenum>(GL_RGBA, GL_UNSIGNED_BYTE);
		case Format::BGRA8_UNORM:
			return std::make_pair<GLenum, GLenum>(GL_BGRA, GL_UNSIGNED_BYTE);
		case Format::RGBA32_FLOAT:
		case Format::RGBA16_FLOAT:
			return std::make_pair<GLenum, GLenum>(GL_RGBA, GL_FLOAT);


		case Format::D16:
		case Format::D32:
			return std::make_pair<GLenum, GLenum>(GL_DEPTH_COMPONENT, GL_FLOAT);

		default:
			GE_ASSERT(0, "unsupport now");
			break;
		}
		return std::make_pair<GLenum, GLenum>(GL_RGBA, GL_UNSIGNED_BYTE);

	}

	GLenum OpenGLUtil::convertTextureInnerFormat(Format format)
	{
		switch (format)
		{

		case Format::R8_UNORM:
			return GL_RED;
		case Format::R32_FLOAT:
			return GL_R32F;
		case Format::RG16_FLOAT:
			return GL_RG16F;

		case Format::RGB8_UNORM:
			return GL_RGB8;

		case Format::RGB8_FLOAT:
			return GL_RGB8;

		case Format::RGB32_FLOAT:
			return GL_RGB32F;

		case Format::RGB16_FLOAT:
			return GL_RGB16F;

		case Format::RGBA8_UNORM:
			return GL_RGBA8;

		case Format::RGBA32_FLOAT:
			return GL_RGBA32F;
		case Format::RGBA16_FLOAT:
			return GL_RGBA16F;

		// gles depth attachment support: https://lumverse.feishu.cn/docx/F9TKd7UhLozzU5xOGqic0zgPnie#share-LDoZdXbgzobiSLxmJYccXljWnNm
		case Format::D16:
			return GL_DEPTH_COMPONENT16;
        case Format::D24:
            return GL_DEPTH_COMPONENT24;  //only for pc ,gles 3+
#ifdef GE_PLATFORM_WINDOWS
		case Format::D32: //only for pc opengl
			return GL_DEPTH_COMPONENT32;
#elif defined(GE_PLATFORM_ANDROID)
       case Format::D32_FLOAT: //only for anfroid gles
           return GL_DEPTH_COMPONENT32F;
#endif

		default:
			GE_ASSERT(0, "unsupport now");
			break;
		}
		return GL_RGBA;
	}

	GLint OpenGLUtil::convertTextureFilter(FilterMode filter)
	{
		switch (filter)
		{
		case BlackPearl::FilterMode::Linear:
			return GL_LINEAR;
		case BlackPearl::FilterMode::Linear_Mip_Linear:
			return GL_LINEAR_MIPMAP_LINEAR;
		case BlackPearl::FilterMode::Nearest:
			return GL_NEAREST;
		case BlackPearl::FilterMode::Nearest_Mip_Nearnest:
			return GL_NEAREST_MIPMAP_NEAREST;

		default:
			assert(0);
			break;
		}
		return GL_LINEAR;

	}

	GLint OpenGLUtil::convertTextureWarp(SamplerAddressMode warp)
	{
		switch (warp) {
		case SamplerAddressMode::ClampToEdge:
			return GL_CLAMP_TO_EDGE;
		case SamplerAddressMode::ClampToBorder:{
        // texture warp for gles version: https://lumverse.feishu.cn/docx/F9TKd7UhLozzU5xOGqic0zgPnie
#ifdef GE_PLATFORM_ANDROID
            if(FOpenGLES::IsES32Usable()){
                return GL_CLAMP_TO_BORDER;
            }else{
                return GL_CLAMP_TO_EDGE;
            }
#elif defined(GE_PLATFORM_WINDOWS)
            return GL_CLAMP_TO_BORDER;
#endif
        }
		case SamplerAddressMode::Repeat:
			return GL_REPEAT;

		default: {
			GE_CORE_ERROR("Unknown warp type");
			assert(0);

		}
		}

		return GL_CLAMP_TO_EDGE;

	}

	uint32_t OpenGLUtil::convertInputElementDataType(const ElementDataType& type)
	{
		switch (type) {
		case ElementDataType::Int:      return GL_INT;
		case ElementDataType::Int2:     return GL_INT;
		case ElementDataType::Int3:     return GL_INT;
		case ElementDataType::Int4:     return GL_INT;
		case ElementDataType::Float:    return GL_FLOAT;
		case ElementDataType::Float2:   return GL_FLOAT;
		case ElementDataType::Float3:   return GL_FLOAT;
		case ElementDataType::Float4:   return GL_FLOAT;
		case ElementDataType::Mat3:		return GL_FLOAT;
		case ElementDataType::Mat4:		return GL_FLOAT;
		case ElementDataType::False:	return GL_FALSE;
		case ElementDataType::True:		return GL_TRUE;
		}
		GE_ASSERT(false, "Unknown ShaderDataType!")
			return 0;
	}


	GLenum OpenGLUtil::convertTextureAccess(TextureAccess access)
	{
		switch (access)
		{
		case TextureAccess::ReadOnly:
			return GL_READ_ONLY;
		case TextureAccess::WriteOnly:
			return GL_WRITE_ONLY;
		case TextureAccess::ReadWrite:
			return GL_READ_WRITE;
		default:
			break;
		}
	}

}