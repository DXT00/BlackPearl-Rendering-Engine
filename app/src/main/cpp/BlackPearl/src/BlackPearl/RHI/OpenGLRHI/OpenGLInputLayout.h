#pragma once
#include "../RHIInputLayout.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLThirdParty.h"


namespace BlackPearl {
	class InputLayout :public RefCounter<IInputLayout>
	{
    public:
		InputLayout(const VertexBufferLayout& _layout) {
			layout = _layout;
		}
		VertexBufferLayout layout;

        std::vector<VertexAttributeDesc> inputDesc;

      /*  std::vector<VkVertexInputBindingDescription> bindingDesc;
        std::vector<VkVertexInputAttributeDescription> attributeDesc;*/

        uint32_t getNumAttributes() const override;
        const VertexAttributeDesc* getAttributeDesc(uint32_t index) const override;
	};


	static uint32_t ShaderDataTypeToBufferType(const ElementDataType& type) {

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
	// use for GetVertexAttributeDesc
	static uint32_t FormatToBufferType(const Format& format) {

		switch (format) {
		case Format::RGB32_FLOAT:      return GL_FLOAT;
		case Format::RG32_FLOAT:       return GL_FLOAT;
		case Format::RGBA32_FLOAT:     return GL_FLOAT;
		case Format::RGBA8_SNORM:      return GL_INT;
		
		}
		GE_ASSERT(false, "Unknown ShaderDataType!")
			return 0;
	}


}

