#pragma once
#include <initializer_list>
#include "BlackPearl/Core.h"
#include "BlackPearl/Renderer/Material/Texture.h"
#include "BlackPearl/Renderer/Material/DepthTexture.h"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
namespace BlackPearl {

	enum class ElementDataType {
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
		False,
		True
	};


	static uint32_t GetDataSize(ElementDataType type) {

		switch (type) {
		case ElementDataType::Int:      return sizeof(int);
		case ElementDataType::Int2:     return 2 * sizeof(int);
		case ElementDataType::Int3:     return 3 * sizeof(int);
		case ElementDataType::Int4:     return 4 * sizeof(int);

		case ElementDataType::Float:    return sizeof(float);
		case ElementDataType::Float2:   return 2 * sizeof(float);
		case ElementDataType::Float3:   return 3 * sizeof(float);
		case ElementDataType::Float4:   return 4 * sizeof(float);

		case ElementDataType::Mat3:		return 3 * 3 * sizeof(float);
		case ElementDataType::Mat4:		return 4 * 4 * sizeof(float);
		case ElementDataType::False:	return 1;
		case ElementDataType::True:		return 1;


		}
		GE_ASSERT(false, "Unknown ElementDataType!")
			return 0;
	}


	struct BufferElement {
		ElementDataType Type;
		std::string Name;
		bool Normalized;
		uint32_t Offset;
		uint32_t ElementSize;

		BufferElement(ElementDataType type, std::string name, bool normalized)
			:Type(type), Name(name), Normalized(normalized), Offset(0), ElementSize(GetDataSize(type)) {}

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
			}
			GE_ASSERT(false, "Unknown ElementDataType!")
				return 0;
		}

		//uint32_t GetType() {

		//	switch (Type) {
		//	case ElementDataType::Int:      return GL_INT;
		//	case ElementDataType::Int2:     return GL_INT;
		//	case ElementDataType::Int3:     return GL_INT;
		//	case ElementDataType::Int4:     return GL_INT;
		//	case ElementDataType::Float:    return GL_FLOAT;
		//	case ElementDataType::Float2:   return GL_FLOAT;
		//	case ElementDataType::Float3:   return GL_FLOAT;
		//	case ElementDataType::Float4:   return GL_FLOAT;
		//	case ElementDataType::Mat3:		return GL_FLOAT;
		//	case ElementDataType::Mat4:		return GL_FLOAT;
		//	case ElementDataType::False:	return GL_FALSE;
		//	case ElementDataType::True:		return GL_TRUE;
		//	}

		//}


	};

	class VertexBufferLayout {
	public:
		VertexBufferLayout() { GE_CORE_INFO("VertexBufferLayout defult constructor!") }
		VertexBufferLayout(std::initializer_list<BufferElement> elements)
			:m_Elememts(elements) {
			CalculateStrideAndOffset();
		};
		void CalculateStrideAndOffset();

		inline std::vector<BufferElement> GetElements() const { return m_Elememts; }
		inline uint32_t GetStride() { return m_Stride; }
	private:
		std::vector<BufferElement> m_Elememts;
		uint32_t m_Stride = 0;
	};






	class VertexBuffer {
	public:

		VertexBuffer(const std::vector<float>&vertices);
		VertexBuffer(float*vertices, uint32_t size);
		void Bind();
		void UnBind();
		void CleanUp();

		void SetBufferLayout(const VertexBufferLayout& layout) { m_BufferLayout = layout; }
		VertexBufferLayout &GetBufferLayout() { return m_BufferLayout; }

	private:
		unsigned int m_RendererID;
		VertexBufferLayout m_BufferLayout;//这里需要默认构造函数

	};


	class IndexBuffer {
	public:
		IndexBuffer(const std::vector<unsigned int>& indices);
		IndexBuffer(unsigned int *indices, uint32_t size);

		void Bind();
		void UnBind();
		void CleanUp();

	private:
		unsigned int m_RendererID;


	};

	class FrameBuffer {
	public:
		
		enum Attachment{
			ColorTexture,
			DepthTexture,
			CubeMapDepthTexture,
			RenderBuffer

		};
		FrameBuffer(const int width, const int height,std::initializer_list<Attachment> attachment,bool disableColor, Texture::Type colorTextureType=Texture::Type::DiffuseMap);
		void AttachColorTexture(Texture::Type textureType);
		void AttachDepthTexture();
		void AttachCubeMapDepthTexture();//use for point light shadow map
		void AttachRenderBuffer();
		void DisableColorBuffer();
		void Bind();
		//switch back to default framebuffer
		void UnBind();

		void BindColorTexture();
		void UnBindTexture();
		void CleanUp();

		std::shared_ptr<Texture> GetColorTexture() { return m_TextureColorBuffer; }
		std::shared_ptr<BlackPearl::DepthTexture> GetDepthTexture() { return m_TextureDepthBuffer; }
		std::shared_ptr<CubeMapTexture> GetCubeMapDepthTexture() { return m_CubeMapDepthBuffer; }
		unsigned int GetWidth()const { return m_Width; }
		unsigned int GetHeight()const { return m_Height; }

	private:
		unsigned int m_Width, m_Height;
		unsigned int m_RendererID;
		//unsigned int m_TextureColorBufferID;
		//unsigned int m_TextureDepthBufferID;
		std::shared_ptr<Texture> m_TextureColorBuffer;
		std::shared_ptr<BlackPearl::DepthTexture> m_TextureDepthBuffer;
		std::shared_ptr<CubeMapTexture> m_CubeMapDepthBuffer;
		unsigned int m_RenderBufferID;


	};
}