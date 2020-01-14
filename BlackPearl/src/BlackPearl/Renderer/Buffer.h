#pragma once
#include <initializer_list>
#include "BlackPearl/Core.h"
#include "BlackPearl/Renderer/Material/Texture.h"
#include "BlackPearl/Renderer/Material/DepthTexture.h"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
#include <unordered_map>
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

		enum Attachment {
			ColorTexture,
			DepthTexture,
			CubeMapDepthTexture,
			CubeMapColorTexture,
			RenderBuffer

		};
		FrameBuffer(const int imageWidth, const int imageHeight, std::initializer_list<Attachment> attachment,  unsigned int colorAttachmentPoint,bool disableColor,Texture::Type colorTextureType = Texture::Type::DiffuseMap);
		void AttachColorTexture(Texture::Type textureType, unsigned int attachmentPoints, unsigned int imageWidth, unsigned int imageHeight);
		void AttachColorTexture(std::shared_ptr<Texture> texture, unsigned int attachmentPoints);

		void AttachDepthTexture(const int imageWidth, int imageHeight);
		void AttachCubeMapDepthTexture(const int imageWidth, int imageHeight);//use for point light shadow map
		void AttachCubeMapColorTexture(unsigned int attachmentPoints,const int imageWidth, int imageHeight);//use for point light shadow map

		void AttachRenderBuffer(const int imageWidth, int imageHeight);
		void DisableColorBuffer();
		void Bind();
		void UnBind();
		void BindRenderBuffer();

		void BindColorTexture(unsigned int attachmentPoints);
		void UnBindTexture(unsigned int attachmentPoints);
		void CleanUp();

		std::shared_ptr<Texture> GetColorTexture(unsigned int attachmentPoint) { 
			GE_ASSERT(m_TextureColorBuffers[attachmentPoint], "attachmentPoint "+ std::to_string( attachmentPoint) +"has no ColorTexture")
			return m_TextureColorBuffers[attachmentPoint]; 
		}
		std::shared_ptr<BlackPearl::DepthTexture> GetDepthTexture() { return m_TextureDepthBuffer; }
		std::shared_ptr<CubeMapTexture> GetCubeMapDepthTexture() { return m_CubeMapDepthBuffer; }
		std::shared_ptr<CubeMapTexture> GetCubeMapColorTexture(unsigned int attachmentPoint) { return std::dynamic_pointer_cast<CubeMapTexture>(m_TextureColorBuffers[attachmentPoint]); }


		unsigned int GetWidth()const { return m_Width; }
		unsigned int GetHeight()const { return m_Height; }
		unsigned int GetRenderBufferID() {
			return m_RenderBufferID;		
		}
		void SetViewPort(int width, int height);
	private:
		unsigned int m_Width, m_Height;
		unsigned int m_RendererID;
		

		//GL_COLOR_ATTACHMENTi 到 Texture的映射
		std::unordered_map<unsigned int,std::shared_ptr<Texture> > m_TextureColorBuffers;
		//std::shared_ptr<Texture> m_TextureColorBuffer;
		std::shared_ptr<BlackPearl::DepthTexture> m_TextureDepthBuffer;
		std::shared_ptr<CubeMapTexture> m_CubeMapDepthBuffer;
		std::shared_ptr<CubeMapTexture> m_CubeMapColorBuffer;

		unsigned int m_RenderBufferID;


	};
}