#pragma once
#include <initializer_list>
#include "BlackPearl/Core.h"
//#include "BlackPearl/RHI/OpenGLRHI/OpenGLTexture.h"
#include "BlackPearl/RHI/RHITexture.h"
#include <glad/glad.h>
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

		VertexBuffer(const std::vector<float>&vertices, bool Interleaved = true, bool divisor = false, uint32_t perInstance = 0, uint32_t drawType = GL_STATIC_DRAW);
		VertexBuffer(const float* vertices, uint32_t size, bool Interleaved = true, bool divisor = false, uint32_t perInstance = 0,uint32_t drawType = GL_STATIC_DRAW);
		VertexBuffer(const unsigned int* vertices, uint32_t size, bool Interleaved = true, bool divisor = false, uint32_t perInstance = 0, uint32_t drawType = GL_STATIC_DRAW);
		VertexBuffer(void* vertices, uint32_t size, bool Interleaved = true, bool divisor = false, uint32_t perInstance = 0, uint32_t drawType = GL_STATIC_DRAW);

		~VertexBuffer();
		void Bind();
		void UpdateData(void* vertices, uint32_t size, uint32_t drawType = GL_STATIC_DRAW);
		void UnBind();
		void CleanUp();
		const float* GetVerticesFloat() const { return m_VerticesFloat; }
		const uint32_t* GetVerticesUInt() const { return m_VerticesUint; }
		const void* GetVerticesVoid() const { return m_VerticesVoidData; }

		void SetBufferLayout(const VertexBufferLayout& layout) { m_BufferLayout = layout; }
		VertexBufferLayout GetBufferLayout() const { return m_BufferLayout; }
		uint32_t GetVertexSize()const { return m_VertexSize; }
		bool GetDivisor() const { return m_Divisor; }
		bool GetInterleaved() const { return m_Interleaved; }

		uint32_t GetDivPerInstance() const { return m_DivPerInstance; }
	private:
		uint32_t m_RendererID;
		uint32_t m_VertexSize;
		const float* m_VerticesFloat = nullptr;
		const uint32_t* m_VerticesUint = nullptr;
		const void* m_VerticesVoidData = nullptr;
		bool m_Interleaved;

		bool m_Divisor = false;
		uint32_t m_DivPerInstance = 0;
		VertexBufferLayout m_BufferLayout;//������ҪĬ�Ϲ��캯��
	};

	class IndexBuffer {
	public:
		IndexBuffer(const std::vector<unsigned int>& indices, uint32_t drawType = GL_STATIC_DRAW);
		IndexBuffer(unsigned int *indices, unsigned int size, uint32_t drawType = GL_STATIC_DRAW);
		~IndexBuffer();
		uint32_t GetIndicesSize() const { return m_IndiciesSize; }
		const unsigned int* GetIndicies()const { return m_Indicies; }

		void Bind();
		void UnBind();

	private:
		unsigned int m_RendererID;
		unsigned int m_IndiciesSize;
		const unsigned int* m_Indicies;

	};

	struct IndirectCommand {
		GLuint  count; //index or vertex cnt
		GLuint  instanceCnt;
		GLuint  firstIndex;
		GLuint  startVertex;
		GLuint  startInstance;
	};

	class IndirectBuffer {
	public:
		IndirectBuffer(const std::vector<IndirectCommand>& commands, uint32_t drawType = GL_DYNAMIC_DRAW);
		~IndirectBuffer();
		uint32_t GetID() const { return m_RendererID; }
		void UpdateCommands(const std::vector<IndirectCommand>& cmds);
		void Bind();
		void UnBind();
		const std::vector<IndirectCommand>& GetCommands() const { return m_Commands; }


	private:
		unsigned int m_RendererID;
		std::vector<IndirectCommand> m_Commands;
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
		FrameBuffer(const int imageWidth, const int imageHeight, std::initializer_list<Attachment> attachment,  unsigned int colorAttachmentPoint,bool disableColor, TextureType colorTextureType = TextureType::DiffuseMap);
		void AttachColorTexture(TextureType textureType, unsigned int attachmentPoints, unsigned int imageWidth, unsigned int imageHeight);
		void AttachColorTexture(TextureHandle texture, unsigned int attachmentPoints);

		void AttachDepthTexture(const int imageWidth, int imageHeight);
		void AttachDepthTexture(TextureHandle texture, int mipmapLevel);

		//void AttachCubeMapDepthTexture(const int imageWidth, int imageHeight);//use for point light shadow map
		void AttachCubeMapDepthTexture(TextureHandle cubeMap);//use for point light shadow map

		void AttachCubeMapColorTexture(unsigned int attachmentPoints,const int imageWidth, int imageHeight);//use for point light shadow map
		void AttachCubeMapColorTexture(unsigned int attachmentPoints, TextureHandle cubeMap);

		void AttachRenderBuffer(const int imageWidth, int imageHeight);
		void DisableColorBuffer();
		void Bind();
		void UnBind();
		void BindRenderBuffer();

		
		void CleanUp();

		TextureHandle GetColorTexture(unsigned int attachmentPoint) {
			GE_ASSERT(m_TextureColorBuffers[attachmentPoint], "attachmentPoint "+ std::to_string( attachmentPoint) +"has no ColorTexture")
			return m_TextureColorBuffers[attachmentPoint]; 
		}
		TextureHandle GetDepthTexture() { return m_TextureDepthBuffer; }
		TextureHandle GetCubeMapDepthTexture() { return m_CubeMapDepthBuffer; }
		TextureHandle GetCubeMapColorTexture(unsigned int attachmentPoint) { return m_TextureColorBuffers[attachmentPoint]; }


		unsigned int GetWidth()const { return m_Width; }
		unsigned int GetHeight()const { return m_Height; }
		unsigned int GetRenderBufferID() const {
			return m_RenderBufferID;		
		}
		void SetViewPort(int width, int height);
	private:
		unsigned int m_Width, m_Height;
		unsigned int m_RendererID;
		

		//GL_COLOR_ATTACHMENTi �� Texture��ӳ��
		std::unordered_map<unsigned int, TextureHandle> m_TextureColorBuffers;
		//TextureHandlem_TextureColorBuffer;
		TextureHandle m_TextureDepthBuffer;
		TextureHandle m_CubeMapDepthBuffer;
		TextureHandle m_CubeMapColorBuffer;

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

		TextureHandle GetPositionTexture()const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_PositionTexture; }
		TextureHandle GetNormalTexture()const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_NormalTexture; }
		TextureHandle GetNormalMapTexture()const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_NormalMapTexture; }
		TextureHandle GetDiffuseRoughnessTexture() const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_DiffuseRoughnessTexture; }
		TextureHandle GetSpecularMentallicTexture()const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_SpecularMentallicTexture; }
		TextureHandle GetAmbientGIAOTexture()const { GE_ASSERT(m_Type == Type::GI, "is not GI Gbuffer!"); return m_AmbientGIAOTexture; }
		void InitGITextures();

		/************************ Raytracing Texture ************************************/
		TextureHandle GetColorTexture(unsigned int idx);
		std::vector<TextureHandle> GetColorTextures() { return m_ColorTextures; }

		void InitRayTracingTextures();
		~GBuffer()=default;

	private:
		/************************ GI Texture (voxel cone tracing and light probe)*********/
		TextureHandle m_PositionTexture;
		TextureHandle m_NormalTexture;
		TextureHandle m_NormalMapTexture;

		/* u_Material.diffuse + u_Material.roughness */
		TextureHandle m_DiffuseRoughnessTexture;
		/* u_Material.specular + u_Material.metallic */
		TextureHandle m_SpecularMentallicTexture;
		/* �洢ȫ�ֹ����е� diffuse ��specular (vec3 ambient =  (Kd*diffuse+specular) * ao;)����ɫ*/
		TextureHandle m_AmbientGIAOTexture;

		/************************ Raytracing Texture ************************************/
		std::vector<TextureHandle> m_ColorTextures;

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
		ShaderStorageBuffer(uint32_t size, uint32_t drawType, void* data);

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