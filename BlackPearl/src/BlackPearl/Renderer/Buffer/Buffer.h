#pragma once
#include <initializer_list>
#include "BlackPearl/Core.h"
#include "BlackPearl/Renderer/Material/Texture.h"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
#include <unordered_map>
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
		False,
		True
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
		uint32_t Location;
		BufferElement() {
			
		}
		BufferElement(ElementDataType type, std::string name, bool normalized, uint32_t location)
			:Type(type), 
			Name(name), 
			Normalized(normalized),
			Offset(0), 
			ElementSize(GetDataSize(type)),
			Location(location){}

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
		inline BufferElement GetElement(uint32_t i) const { 
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
		uint32_t GetStride() const{ return m_Stride; }
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
		VertexBufferLayout& operator = (const VertexBufferLayout& rhs) {
			this->m_Elememts.resize(rhs.ElementSize());
			for (size_t i = 0; i < rhs.ElementSize(); i++)
			{
				this->m_Elememts[i] = rhs.m_Elememts[i];
			}
			this->m_Stride = rhs.m_Stride;
			return *this;
		}

	protected:
		std::vector<BufferElement> m_Elememts;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer {
	public:

		VertexBuffer(const std::vector<float>&vertices, uint32_t drawType = GL_STATIC_DRAW);
		VertexBuffer(const float* vertices, uint32_t size, uint32_t drawType = GL_STATIC_DRAW);
		VertexBuffer(const unsigned int* vertices, uint32_t size, uint32_t drawType = GL_STATIC_DRAW);
		~VertexBuffer();
		void Bind();
		void UnBind();
		void CleanUp();
		const float* GetVerticesFloat() const { return m_VerticesFloat; }
		const unsigned int* GetVerticesUInt() const { return m_VerticesUint; }
		void SetBufferLayout(const VertexBufferLayout& layout) { m_BufferLayout = layout; }
		VertexBufferLayout GetBufferLayout() const { return m_BufferLayout; }
		unsigned int GetVertexSize()const { return m_VertexSize; }
	private:
		unsigned int m_RendererID;
		unsigned int m_VertexSize;
		const float* m_VerticesFloat = nullptr;
		const unsigned int* m_VerticesUint = nullptr;
		VertexBufferLayout m_BufferLayout;//这里需要默认构造函数

	};

	class IndexBuffer {
	public:
		IndexBuffer(const std::vector<unsigned int>& indices, uint32_t drawType = GL_STATIC_DRAW);
		IndexBuffer(unsigned int *indices, unsigned int size, uint32_t drawType = GL_STATIC_DRAW);
		unsigned int GetIndicesSize()const { return m_IndiciesSize; }
		void Bind();
		void UnBind();
		void CleanUp();

	private:
		unsigned int m_RendererID;
		unsigned int m_IndiciesSize;

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
		/*no attachment*/
		FrameBuffer();
		
		/*initial with attachment*/
		FrameBuffer(const int imageWidth, const int imageHeight, std::initializer_list<Attachment> attachment,  unsigned int colorAttachmentPoint,bool disableColor,Texture::Type colorTextureType = Texture::Type::DiffuseMap);
		void AttachColorTexture(Texture::Type textureType, unsigned int attachmentPoints, unsigned int imageWidth, unsigned int imageHeight);
		void AttachColorTexture(std::shared_ptr<Texture> texture, unsigned int attachmentPoints);

		void AttachDepthTexture(const int imageWidth, int imageHeight);
		//void AttachCubeMapDepthTexture(const int imageWidth, int imageHeight);//use for point light shadow map
		void AttachCubeMapDepthTexture(std::shared_ptr<CubeMapTexture> cubeMap);//use for point light shadow map

		void AttachCubeMapColorTexture(unsigned int attachmentPoints,const int imageWidth, int imageHeight);//use for point light shadow map
		void AttachCubeMapColorTexture(unsigned int attachmentPoints, std::shared_ptr<CubeMapTexture> cubeMap);

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
		std::shared_ptr<Texture > GetDepthTexture() { return m_TextureDepthBuffer; }
		std::shared_ptr<CubeMapTexture> GetCubeMapDepthTexture() { return m_CubeMapDepthBuffer; }
		std::shared_ptr<CubeMapTexture> GetCubeMapColorTexture(unsigned int attachmentPoint) { return std::dynamic_pointer_cast<CubeMapTexture>(m_TextureColorBuffers[attachmentPoint]); }


		unsigned int GetWidth()const { return m_Width; }
		unsigned int GetHeight()const { return m_Height; }
		unsigned int GetRenderBufferID() const {
			return m_RenderBufferID;		
		}
		void SetViewPort(int width, int height);
	private:
		unsigned int m_Width, m_Height;
		unsigned int m_RendererID;
		

		//GL_COLOR_ATTACHMENTi 到 Texture的映射
		std::unordered_map<unsigned int,std::shared_ptr<Texture> > m_TextureColorBuffers;
		//std::shared_ptr<Texture> m_TextureColorBuffer;
		std::shared_ptr<Texture> m_TextureDepthBuffer;
		std::shared_ptr<CubeMapTexture> m_CubeMapDepthBuffer;
		std::shared_ptr<CubeMapTexture> m_CubeMapColorBuffer;

		unsigned int m_RenderBufferID;
	};

	class GBuffer
	{
	public:
		enum  Type {
			GI,
			RayTracing
		};
		GBuffer(const unsigned int imageWidth,const unsigned int imageHeight,Type type = Type::GI);
		void Bind();
		void UnBind();
		/************************ GI Texture (voxel cone tracing and light probe)*********/

		std::shared_ptr<Texture> GetPositionTexture()const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_PositionTexture; }
		std::shared_ptr<Texture> GetNormalTexture()const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_NormalTexture; }
		std::shared_ptr<Texture> GetNormalMapTexture()const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_NormalMapTexture; }
		std::shared_ptr<Texture> GetDiffuseRoughnessTexture()const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_DiffuseRoughnessTexture; }
		std::shared_ptr<Texture> GetSpecularMentallicTexture()const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_SpecularMentallicTexture; }
		std::shared_ptr<Texture> GetAmbientGIAOTexture()const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_AmbientGIAOTexture; }
		void InitGITextures();

		/************************ Raytracing Texture ************************************/
		std::shared_ptr<Texture> GetColorTexture(unsigned int idx);
		std::vector<std::shared_ptr<Texture>> GetColorTextures() { return m_ColorTextures; }

		void InitRayTracingTextures();
		~GBuffer()=default;

	private:
		/************************ GI Texture (voxel cone tracing and light probe)*********/
		std::shared_ptr<Texture> m_PositionTexture;
		std::shared_ptr<Texture> m_NormalTexture;
		std::shared_ptr<Texture> m_NormalMapTexture;

		/* u_Material.diffuse + u_Material.roughness */
		std::shared_ptr<Texture> m_DiffuseRoughnessTexture;
		/* u_Material.specular + u_Material.metallic */
		std::shared_ptr<Texture> m_SpecularMentallicTexture;
		/* 存储全局光照中的 diffuse 和specular (vec3 ambient =  (Kd*diffuse+specular) * ao;)的颜色*/
		std::shared_ptr<Texture> m_AmbientGIAOTexture;

		/************************ Raytracing Texture ************************************/
		std::vector<std::shared_ptr<Texture>>m_ColorTextures;

		unsigned int m_RendererID;
		unsigned int m_RenderBufferID;
		unsigned int m_Width, m_Height;
		Type m_Type;


	};
	class AtomicBuffer {
	public:
		AtomicBuffer();
		~AtomicBuffer() {
			CleanUp();
		}
		void Bind();
		void BindIndex(unsigned int index);
		void UnBind();
		void ResetValue(GLuint val);
		void CleanUp();
	private:
		unsigned int m_RendererID;
		
	};
	//SSBO
	class ShaderStorageBuffer {
	public:
		ShaderStorageBuffer(GLsizeiptr bytes, GLbitfield mapFlags);
		~ShaderStorageBuffer();
		GLuint GetRenderID() const {
			return m_RendererID;
		}
		void Bind();
		void BindIndex(unsigned int index);
		void UnBind();
		void CleanUp();
	private:
		GLuint m_RendererID;

	};
}