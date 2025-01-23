#pragma once
#include <initializer_list>
#include "BlackPearl/Core.h"
//#include "BlackPearl/RHI/OpenGLRHI/OpenGLTexture.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "OpenGLBuffer.h"
#include <glad/glad.h>
#include <unordered_map>
namespace BlackPearl {

	class VertexBuffer : public Buffer {
	public:
		VertexBuffer(const BufferDesc& _desc);
		VertexBuffer(const BufferDesc& _desc, const std::vector<float>&vertices, bool Interleaved = true, bool divisor = false, uint32_t perInstance = 0, uint32_t drawType = GL_STATIC_DRAW);
		VertexBuffer(const BufferDesc& _desc, const float* vertices, uint32_t size, bool Interleaved = true, bool divisor = false, uint32_t perInstance = 0,uint32_t drawType = GL_STATIC_DRAW);
		VertexBuffer(const BufferDesc& _desc, const unsigned int* vertices, uint32_t size, bool Interleaved = true, bool divisor = false, uint32_t perInstance = 0, uint32_t drawType = GL_STATIC_DRAW);
		VertexBuffer(const BufferDesc& _desc, void* vertices, uint32_t size, bool Interleaved = true, bool divisor = false, uint32_t perInstance = 0, uint32_t drawType = GL_STATIC_DRAW);

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
		//uint32_t m_RendererID;
		uint32_t m_VertexSize;
		const float* m_VerticesFloat = nullptr;
		const uint32_t* m_VerticesUint = nullptr;
		const void* m_VerticesVoidData = nullptr;
		bool m_Interleaved;

		bool m_Divisor = false;
		uint32_t m_DivPerInstance = 0;
		VertexBufferLayout m_BufferLayout;//这里需要默认构造函数
	};

	class IndexBuffer : public Buffer {
	public:
		IndexBuffer(const BufferDesc& _desc);
		IndexBuffer(const BufferDesc& _desc, const std::vector<unsigned int>& indices, uint32_t drawType = GL_STATIC_DRAW);
		IndexBuffer(const BufferDesc& _desc, unsigned int *indices, unsigned int size, uint32_t drawType = GL_STATIC_DRAW);
		~IndexBuffer();
		uint32_t GetIndicesSize() const { return m_IndiciesSize; }
		const unsigned int* GetIndicies()const { return m_Indicies; }

		void Bind();
		void UnBind();

	private:
		//unsigned int m_RendererID;
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

	class IndirectBuffer : public Buffer {
	public:
		IndirectBuffer(const BufferDesc& _desc);
		IndirectBuffer(const BufferDesc& _desc, const std::vector<IndirectCommand>& commands, uint32_t drawType = GL_DYNAMIC_DRAW);
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


	class Framebuffer : public Buffer {
	public:

		enum Attachment {
			ColorTexture,
			DepthTexture,
			CubeMapDepthTexture,
			CubeMapColorTexture,
			RenderBuffer

		};
		/*no attachment*/
		Framebuffer(const BufferDesc& _desc);
		
		/*initial with attachment*/
		Framebuffer(const BufferDesc& _desc, const int imageWidth, const int imageHeight, std::initializer_list<Attachment> attachment,  unsigned int colorAttachmentPoint,bool disableColor, TextureType colorTextureType = TextureType::DiffuseMap);
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
		//unsigned int m_RendererID;
		

		//GL_COLOR_ATTACHMENTi 到 Texture的映射
		std::unordered_map<unsigned int, TextureHandle> m_TextureColorBuffers;
		//TextureHandlem_TextureColorBuffer;
		TextureHandle m_TextureDepthBuffer;
		TextureHandle m_CubeMapDepthBuffer;
		TextureHandle m_CubeMapColorBuffer;

		unsigned int m_RenderBufferID;
	};

	class GBuffer : public Buffer
	{
	public:
		enum  Type {
			GI,
			RayTracing
		};
		GBuffer(const BufferDesc& _desc, const unsigned int imageWidth,const unsigned int imageHeight,Type type = Type::GI);
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
		/* 存储全局光照中的 diffuse 和specular (vec3 ambient =  (Kd*diffuse+specular) * ao;)的颜色*/
		TextureHandle m_AmbientGIAOTexture;

		/************************ Raytracing Texture ************************************/
		std::vector<TextureHandle> m_ColorTextures;

		//unsigned int m_RendererID;
		unsigned int m_RenderBufferID;
		unsigned int m_Width, m_Height;
		Type m_Type;


	};
	class AtomicBuffer : public Buffer {
	public:
		AtomicBuffer(const BufferDesc& _desc);
		~AtomicBuffer() {
			CleanUp();
		}
		void Bind();
		void BindIndex(unsigned int index);
		void UnBind();
		void ResetValue(GLuint val);
		void CleanUp();
	private:
		//unsigned int m_RendererID;
		
	};
	//SSBO
	class ShaderStorageBuffer : public Buffer {
	public:
		ShaderStorageBuffer::ShaderStorageBuffer(const BufferDesc& _desc);
			
		ShaderStorageBuffer(const BufferDesc& _desc, GLsizeiptr bytes, GLbitfield mapFlags);
		ShaderStorageBuffer(const BufferDesc& _desc, uint32_t size, uint32_t drawType, void* data);

		~ShaderStorageBuffer();
		GLuint GetRenderID() const {
			return rendererID;
		}
		void Bind();
		void BindIndex(unsigned int index);
		void UnBind();
		void CleanUp();
	private:
		//GLuint m_RendererID;

	};
}