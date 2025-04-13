#pragma once

#include "RHIDefinitions.h"
#include "RefCountPtr.h"
#include "RHIResources.h"
#include "BlackPearl/Core.h"
#include <string>

namespace BlackPearl {

	enum class ElementDataType {
		Char,
		Char3,
		Int,
		Int2,
		Int3,
		Int4,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Uint32,
		Uint32_2,
		Uint32_3,
		Uint32_4,


		False,
		True
	};
	/*
	
	*/
	enum class LayoutType {
		// [position0, position1, ..., positionN, normal0, normal1, ..., normalN, uv0, uv1, ..., uvN]
		OneVBO_NoInterleave,

		// [position0, normal0, uv0, position1, normal1, uv1 ..., positionN, normalN, uvN]
		OneVBO_Interleave,  

		// [position0, position1, ..., positionN]
		// [normal0, normal1, ........., normalN]
		// [uv0, uv1, ....................., uvN]

		MultiVBO_NoInterleave, 
	};
	
	static uint32_t GetDataSize(ElementDataType type) {

		switch (type) {
		case ElementDataType::Char:      return sizeof(char);
		case ElementDataType::Char3:     return 3 * sizeof(char);
		case ElementDataType::Int:      return sizeof(int);
		case ElementDataType::Int2:     return 2 * sizeof(int);
		case ElementDataType::Int3:     return 3 * sizeof(int);
		case ElementDataType::Int4:     return 4 * sizeof(int);

		case ElementDataType::Float:    return sizeof(float);
		case ElementDataType::Float2:   return 2 * sizeof(float);
		case ElementDataType::Float3:   return 3 * sizeof(float);
		case ElementDataType::Float4:   return 4 * sizeof(float);


		case ElementDataType::Uint32:    return sizeof(uint32_t);
		case ElementDataType::Uint32_2:   return 2 * sizeof(uint32_t);
		case ElementDataType::Uint32_3:   return 3 * sizeof(uint32_t);
		case ElementDataType::Uint32_4:   return 4 * sizeof(uint32_t);

		case ElementDataType::Mat3:		return 3 * 3 * sizeof(float);
		case ElementDataType::Mat4:		return 4 * 4 * sizeof(float);
		case ElementDataType::False:	return 1;
		case ElementDataType::True:		return 1;


		}
		
		assert("Unknown ElementDataType!");
	    return 0;
	}


	struct BufferElement {
		ElementDataType Type;
		std::string Name;
		bool Normalized;
		uint32_t Offset;
		uint32_t ElementSize;
		uint32_t Location;
		BufferElement() {

		}
		BufferElement(ElementDataType type, std::string name, bool normalized, uint32_t location)
			:Type(type),
			Name(name),
			Normalized(normalized),
			Offset(0),
			ElementSize(GetDataSize(type)),
			Location(location) {
		}

		uint32_t GetElementCount() {
			switch (Type) {
			case ElementDataType::Int:      return 1;
			case ElementDataType::Int2:     return 2;
			case ElementDataType::Int3:     return 3;
			case ElementDataType::Int4:     return 4;
			case ElementDataType::Float:    return 1;
			case ElementDataType::Float2:   return 2;
			case ElementDataType::Float3:   return 3;
			case ElementDataType::Float4:   return 4;
			case ElementDataType::Mat3:		return 3 * 3;
			case ElementDataType::Mat4:		return 4 * 4;
			case ElementDataType::False:	return 1;
			case ElementDataType::True:		return 1;

			case ElementDataType::Uint32:    return 1;
			case ElementDataType::Uint32_2:  return 2 ;
			case ElementDataType::Uint32_3:  return 3 ;
			case ElementDataType::Uint32_4:  return 4 ;

			}
			GE_ASSERT(0, "Unknown ElementDataType!");
				return 0;
		}
		void operator=(const BufferElement& rhs) {
			Type = rhs.Type;
			Name = rhs.Name;
			Normalized = rhs.Normalized;
			Offset = rhs.Offset;
			ElementSize = rhs.ElementSize;
			Location = rhs.Location;
			//return *this;
		}
	};

	class VertexBufferLayout {
	public:
		VertexBufferLayout() { //GE_CORE_INFO("VertexBufferLayout defult constructor!")
		}
		VertexBufferLayout(std::initializer_list<BufferElement> elements)
			:m_Elememts(elements) {
			CalculateStrideAndOffset();
			UpdateDesc();

		};
		void CalculateStrideAndOffset();

		inline std::vector<BufferElement> GetElements() const { return m_Elememts; }
		BufferElement GetElement(uint32_t i) const {
			GE_ASSERT((i < ElementSize()), "i exceed max elements size");
			return m_Elememts[i];
		}

		bool HasElement(uint32_t i) {
			if (i < ElementSize())
				return true;
			return false;
		}
		void AddElement(const BufferElement& element) {
			m_Elememts.push_back(element);
			CalculateStrideAndOffset();
			UpdateDesc();
		}
		uint32_t ElementSize() const { return m_Elememts.size(); }
		uint32_t GetStride() const
		{
			if (m_LayoutType == LayoutType::OneVBO_NoInterleave) {//�����ݽ������У��޼����
				return 0;
			}
			else if (m_LayoutType == LayoutType::MultiVBO_NoInterleave) {
				return 0;
			}
			else
				return m_Stride; 
	
		}
		//for directx
		virtual void UpdateDesc() {}

		VertexBufferLayout(const VertexBufferLayout& rhs) {
			m_Elememts.resize(rhs.ElementSize());
			for (size_t i = 0; i < rhs.ElementSize(); i++)
			{
				m_Elememts[i] = rhs.m_Elememts[i];
			}
			m_Stride = rhs.m_Stride;

		}
		VertexBufferLayout& operator=(const VertexBufferLayout& rhs) {
			this->m_Elememts.resize(rhs.ElementSize());
			for (size_t i = 0; i < rhs.ElementSize(); i++)
			{
				this->m_Elememts[i] = rhs.m_Elememts[i];
			}
			this->m_Stride = rhs.m_Stride;
			return *this;
		}
		LayoutType m_LayoutType = LayoutType::OneVBO_NoInterleave;;
	protected:
		std::vector<BufferElement> m_Elememts;
		uint32_t m_Stride = 0;
	};

    struct VertexAttributeDesc
    {
		std::string name;
        //Format format = Format::UNKNOWN;
		ElementDataType elementType = ElementDataType::Float3;

		uint32_t elementCnt = 3;
		uint32_t elementSizeByte = 3 * sizeof(float);

        uint32_t streamIndex = 0;
        uint32_t arraySize = 1;
        uint32_t location = 0;
        uint32_t offset = 0;
        // note: for most APIs, all strides for a given bufferIndex must be identical
        uint32_t elementStride = 0;
        bool isInstanced = false;
        bool divisor = false;
        bool bNormalized = true;
		bool bShouldConvertToFloat = true;

     /*   VertexAttributeDesc& setName(const std::string& value) { name = value; return *this; }
        constexpr VertexAttributeDesc& setFormat(Format value) { format = value; return *this; }
        constexpr VertexAttributeDesc& setArraySize(uint32_t value) { arraySize = value; return *this; }
        constexpr VertexAttributeDesc& setBufferIndex(uint32_t value) { bufferIndex = value; return *this; }
        constexpr VertexAttributeDesc& setOffset(uint32_t value) { offset = value; return *this; }
        constexpr VertexAttributeDesc& setElementStride(uint32_t value) { elementStride = value; return *this; }
        constexpr VertexAttributeDesc& setIsInstanced(bool value) { isInstanced = value; return *this; }*/
    };

    class IInputLayout : public IResource
    {
    public:
        [[nodiscard]] virtual uint32_t getNumAttributes() const = 0;
        [[nodiscard]] virtual const VertexAttributeDesc* getAttributeDesc(uint32_t index) const = 0;
    };

    typedef RefCountPtr<IInputLayout> InputLayoutHandle;


}
