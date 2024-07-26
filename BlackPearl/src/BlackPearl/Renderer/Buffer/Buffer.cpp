#include "pch.h"
#include "glad/glad.h"
#include "Buffer.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/RHISampler.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include <memory>


namespace BlackPearl {
	extern DeviceManager* g_deviceManager;

	//------------------------VertexBuffer-----------------//
	/* 
	darwType = GL_STATIC_DRAW / GL_DYNAMIC_DRAW (buffer can be read or written)
	*/
	VertexBuffer::VertexBuffer(const std::vector<float>& vertices, bool Interleaved, bool divisor, uint32_t perInstance , uint32_t drawType)
	{
		m_VerticesFloat = (&vertices[0]);
#ifdef GE_API_OPENGL
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], drawType);
#endif

		m_VertexSize = vertices.size() * sizeof(float);
		m_Interleaved = Interleaved;
		if (divisor) {
			m_Divisor = true;
			m_DivPerInstance = perInstance;
		}
	}
	VertexBuffer::VertexBuffer(const float* vertices, uint32_t size, bool Interleaved , bool divisor, uint32_t perInstance, uint32_t drawType)
	{
		m_VerticesFloat = vertices;
		//TODO:: 使用RHIBuffer
#ifdef GE_API_OPENGL
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, drawType);
#endif
		m_VertexSize = size;
		m_Interleaved = Interleaved;
		if (divisor) {
			m_Divisor = true;
			m_DivPerInstance = perInstance;
		}
	}
	VertexBuffer::VertexBuffer(const unsigned int* vertices, uint32_t size, bool Interleaved, bool divisor, uint32_t perInstance, uint32_t drawType)
	{
		m_VerticesUint = vertices;
#ifdef GE_API_OPENGL
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, drawType);
#endif
		m_VertexSize = size;
		m_Interleaved = Interleaved;
		if (divisor) {
			m_Divisor = true;
			m_DivPerInstance = perInstance;
		}

	}
	VertexBuffer::VertexBuffer(void* vertices, uint32_t size, bool Interleaved, bool divisor, uint32_t perInstance, uint32_t drawType)
	{
		m_VerticesVoidData = vertices;
#ifdef GE_API_OPENGL
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, drawType);
#endif
		m_VertexSize = size;
		m_Interleaved = Interleaved;
		if (divisor) {
			m_Divisor = true;
			m_DivPerInstance = perInstance;
		}

	}
	VertexBuffer::~VertexBuffer()
	{	
		GE_SAVE_DELETE(m_VerticesFloat);
		GE_SAVE_DELETE(m_VerticesUint);
		CleanUp();
	}
	void VertexBuffer::Bind() {

#ifdef GE_API_OPENGL
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
#endif
	}

	void VertexBuffer::UpdateData(void* vertices, uint32_t size, uint32_t drawType)
	{
		m_VerticesVoidData = vertices;
#ifdef GE_API_OPENGL
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, drawType);
#endif
		m_VertexSize = size;
	}

	void VertexBuffer::UnBind() {
#ifdef GE_API_OPENGL
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

	}
	void VertexBuffer::CleanUp()
	{
#ifdef GE_API_OPENGL
		glDeleteBuffers(1, &m_RendererID);
#endif
	}
	//------------------------IndexBuffer-----------------//
	IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices, uint32_t drawType)
	{
#ifdef GE_API_OPENGL
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], drawType);
#endif
		m_IndiciesSize = indices.size() * sizeof(unsigned int);
		m_Indicies = indices.data();
	}
	IndexBuffer::IndexBuffer(unsigned int * indices, unsigned int size, uint32_t drawType)
	{
#ifdef GE_API_OPENGL
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, drawType);
#endif
		m_IndiciesSize = size;
		m_Indicies = indices;
	}
	IndexBuffer::~IndexBuffer()
	{
#ifdef GE_API_OPENGL
		glDeleteBuffers(1, &m_RendererID);
#endif
	}
	void IndexBuffer::Bind()
	{
#ifdef GE_API_OPENGL
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
#endif

	}

	void IndexBuffer::UnBind()
	{
#ifdef GE_API_OPENGL
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif

	}
	
	//---------------------VertexBufferLayout----------------//
	void VertexBufferLayout::CalculateStrideAndOffset()
	{
		uint32_t strides = 0;
		for (BufferElement &element : m_Elememts) {
			element.Offset = strides;
			strides += element.ElementSize;
		}
		m_Stride = strides;
	}


	FrameBuffer::FrameBuffer()
	{

		glGenFramebuffers(1, &m_RendererID);
	}

	//------------------------FrameBuffer-----------------//
	//note: framebuffer has no memory, imageWidth, imageHeight is the width and height of the attachment! 不同attachment有不同的width和height
	FrameBuffer::FrameBuffer(const int imageWidth,int imageHeight,std::initializer_list<Attachment> attachment, unsigned int colorAttachmentPoint,bool disableColor, Texture::Type colorTextureType)
	{
		/* m_Width,m_Height for voxel cone tracing */
		m_Width = imageWidth;
		m_Height = imageHeight;
		GLint previousFrameBuffer;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFrameBuffer);//获取之前绑定的Framebuffer

		glGenFramebuffers(1, &m_RendererID);
		Bind();
		if (disableColor) {
			DisableColorBuffer();
		}
		for (Attachment attach:attachment)
		{
			if (attach == Attachment::ColorTexture)
				AttachColorTexture(colorTextureType, colorAttachmentPoint,imageWidth,imageHeight);
			else if (attach == Attachment::DepthTexture)
				AttachDepthTexture(imageWidth, imageHeight);
		/*	else if (attach == Attachment::CubeMapDepthTexture)
				AttachCubeMapDepthTexture(imageWidth, imageHeight);*/
			else if (attach == Attachment::CubeMapColorTexture)
				AttachCubeMapColorTexture(colorAttachmentPoint,imageWidth, imageHeight);
			else if (attach == Attachment::RenderBuffer)
				AttachRenderBuffer(imageWidth, imageHeight);

		}
		
		/*if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			GE_CORE_ERROR("Framebuffer is not complete!");*/

		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");

		UnBind();
		//绑定回原来的FrameBuffer
		glBindFramebuffer(GL_FRAMEBUFFER, previousFrameBuffer);

	}

	void FrameBuffer::AttachColorTexture(Texture::Type textureType,unsigned int attachmentPoints,unsigned int imageWidth,unsigned int imageHeight)
	{
		GE_CORE_WARN(" 注意Texture是否是默认的格式！");

		m_TextureColorBuffers[attachmentPoints].reset(DBG_NEW Texture(textureType, imageWidth, imageHeight,false, GL_NEAREST, GL_NEAREST, GL_RGB16F, GL_RGBA, GL_REPEAT,GL_FLOAT));
		
		//将它附加到当前绑定的帧缓冲对象
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+attachmentPoints, GL_TEXTURE_2D, m_TextureColorBuffers[attachmentPoints]->GetRendererID(), 0);
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}

	void FrameBuffer::AttachColorTexture(std::shared_ptr<Texture> texture, unsigned int attachmentPoints)
	{
		//将它附加到当前绑定的帧缓冲对象
		m_TextureColorBuffers[attachmentPoints] = texture;
		Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentPoints, GL_TEXTURE_2D, (GLuint)texture->GetRendererID(), 0);
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}
	//TODO:: 通过device->createFramebuffer(fboDesc);创建
	void FrameBuffer::AttachDepthTexture(const int imageWidth, int imageHeight)
	{	
		GE_CORE_WARN(" 注意Texture是否是默认的格式！");


		TextureDesc desc;
		desc.type = TextureType::DepthMap;
		desc.width = imageWidth;
		desc.height = imageHeight;
		desc.minFilter = FilterMode::Linear;
		desc.magFilter = FilterMode::Linear;
		desc.wrap = SamplerAddressMode::ClampToEdge;
		desc.format = Format::D32;
		desc.generateMipmap = true;
		//texture->diffuseTextureMap = g_deviceManager->GetDevice()->createTexture(desc);
		auto texture = g_deviceManager->GetDevice()->createTexture(desc);

		//m_TextureDepthBuffer.reset(DBG_NEW BlackPearl::Texture(Texture::Type::DepthMap, imageWidth, imageHeight,false, GL_NEAREST, GL_NEAREST, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_CLAMP_TO_EDGE, GL_FLOAT));
		m_TextureDepthBuffer.reset(static_cast<Texture*>(texture.Get()));
		Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureDepthBuffer->GetRendererID(), 0);

		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}

	void FrameBuffer::AttachDepthTexture(std::shared_ptr<ITexture> texture, int mipmapLevel) {
		m_TextureDepthBuffer.reset(static_cast<Texture*>(texture.get()));
		//Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureDepthBuffer->GetRendererID(), mipmapLevel);
		GLenum statue = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}



	//TODO::注意CubeMap的格式
	void FrameBuffer::AttachCubeMapDepthTexture(std::shared_ptr<CubeMapTexture> cubeMap)
	{

		m_CubeMapDepthBuffer = cubeMap;// .reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, imageWidth, imageHeight, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT));
		Bind();
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_CubeMapDepthBuffer->GetRendererID(), 0);
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}

	//TODO::这个接口有问题
	void FrameBuffer::AttachCubeMapColorTexture(unsigned int attachmentPoints,const int imageWidth, int imageHeight) {
		//GE_CORE_ERROR("不能调用，后续完善!");
		GE_CORE_WARN(" 注意Texture是否是默认的格式！");

		m_TextureColorBuffers[attachmentPoints].reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, imageWidth, imageHeight, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT));
	//	GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
		//Bind();

	}
	void FrameBuffer::AttachCubeMapColorTexture(unsigned int attachmentPoints, std::shared_ptr<CubeMapTexture> cubeMap)
	{
		m_TextureColorBuffers[attachmentPoints] = cubeMap;
		//GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}
	void FrameBuffer::AttachRenderBuffer(const int imageWidth, int imageHeight)
	{
		//  create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		//	unsigned int renderBuffer;
		// The depth buffer
		glGenRenderbuffers(1, &m_RenderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);

		// Use a single rbo for both depth and stencil buffer.
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, imageWidth, imageHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);

	}

	void FrameBuffer::DisableColorBuffer()
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	void FrameBuffer::Bind()
	{
		//glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		//glViewport(0, 0, width, height);

	}
	void FrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);

	}

	void FrameBuffer::BindRenderBuffer()
	{	
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
	}
	

	void FrameBuffer::CleanUp()
	{
		
		glDeleteRenderbuffers(1, &m_RenderBufferID);
		glDeleteFramebuffers(1,&m_RendererID);

	}
	void FrameBuffer::SetViewPort(int width, int height)
	{
		m_Width = width;
		m_Height = height;
		glViewport(0, 0, m_Width, m_Height);
	}

	/*----------------------------    GBuffer   --------------------------------*/
	GBuffer::GBuffer(const unsigned int imageWidth, const unsigned int imageHeight, Type type)
	{

		m_Width  = imageWidth;
		m_Height = imageHeight;
		m_Type = type;

		
		glGenFramebuffers(1, &m_RendererID);
		Bind();
		
		switch (type) {
		case Type::GI: {
			InitGITextures();
			break;
		}
		case Type::RayTracing: {
			InitRayTracingTextures();
			break;
		}
		default: {
			GE_CORE_ERROR("No such GBuffer Type!");
			break;
		}

		}

	
		UnBind();
	}
	void GBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	}
	void GBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	// device->createTexture
	void GBuffer::InitGITextures()
	{

		TextureDesc desc;
		desc.type = TextureType::DepthMap;
		desc.width = m_Width;
		desc.height = m_Width;
		desc.minFilter = FilterMode::Nearest;
		desc.magFilter = FilterMode::Nearest;
		desc.wrap = SamplerAddressMode::ClampToEdge;
		desc.format = Format::RGBA16_FLOAT;
		desc.generateMipmap = true;
		//texture->diffuseTextureMap = g_deviceManager->GetDevice()->createTexture(desc);
		m_PositionTexture = g_deviceManager->GetDevice()->createTexture(desc);

		//m_PositionTexture RGB-position A--isPBRObject+objectId -->voxel cone tracing
		m_PositionTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA16F, GL_RGBA, -1, GL_FLOAT));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (GLuint)m_PositionTexture->GetRendererID(), 0);
		//m_NormalTexture RGB-normal A--isSkyBox -->voxel cone tracing
		m_NormalTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA16F, GL_RGBA, -1, GL_FLOAT));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, (GLuint)m_NormalTexture->GetRendererID(), 0);

		m_DiffuseRoughnessTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA16F, GL_RGBA, -1, GL_FLOAT));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, (GLuint)m_DiffuseRoughnessTexture->GetRendererID(), 0);

		m_SpecularMentallicTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA16F, GL_RGBA, -1, GL_FLOAT));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, (GLuint)m_SpecularMentallicTexture->GetRendererID(), 0);

		//m_AmbientGI RBG-ambienGI AO-isPBRObject -->lightprobe
		m_AmbientGIAOTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA16F, GL_RGBA, -1, GL_FLOAT));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, (GLuint)m_AmbientGIAOTexture->GetRendererID(), 0);

		m_NormalMapTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGB16F, GL_RGB, -1, GL_FLOAT));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, (GLuint)m_NormalMapTexture->GetRendererID(), 0);


		GLuint attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 ,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5 };
		glDrawBuffers(6, attachments);
		// - Create and attach depth buffer (renderbuffer)
		glGenRenderbuffers(1, &m_RenderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
		// - Finally check if framebuffer is complete
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");

	}
	TextureHandle GBuffer::GetColorTexture(unsigned int idx)
	{
		GE_ASSERT(idx<m_ColorTextures.size() && m_ColorTextures[idx],"fail to get texture")
		return m_ColorTextures[idx];
	}
	void GBuffer::InitRayTracingTextures()
	{

		const size_t colorBufferNum = 4;
		for (size_t i = 0; i < colorBufferNum; i++) {

			std::shared_ptr<Texture> texture;
			texture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, (GLuint)texture->GetRendererID(), 0);
			m_ColorTextures.push_back(texture);

		}
	
		const GLenum attachments[colorBufferNum] = {
			GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
		};
		glDrawBuffers(colorBufferNum, attachments);
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");



	}
	/*----------------------------    AtomicBuffer   --------------------------------*/

	AtomicBuffer::AtomicBuffer()
	{
		GLuint initVal = 0;
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_RendererID);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &initVal, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	}
	void AtomicBuffer::ResetValue(GLuint val) {
		Bind();
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &val); //reset counter to zero
		UnBind();
	}

	
	void AtomicBuffer::Bind()
	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_RendererID);
	}
	void AtomicBuffer::BindIndex(unsigned int index)
	{
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, index, m_RendererID);
	}
	void AtomicBuffer::UnBind()
	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}
	void AtomicBuffer::CleanUp()
	{
		glDeleteBuffers(1, &m_RendererID);
	}
	/*----------------------------    SSBO   --------------------------------*/
	ShaderStorageBuffer::ShaderStorageBuffer(GLsizeiptr bytes,GLbitfield mapFlags)
	{
		GLuint initVal = 0;

		//glGenBuffers(1, &m_RendererID);
		glCreateBuffers(1, &m_RendererID);
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glNamedBufferStorage(m_RendererID, bytes, nullptr, mapFlags);
		//glBufferData(GL_SHADER_STORAGE_BUFFER, bytes, nullptr, GL_STATIC_COPY);
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}


	ShaderStorageBuffer::ShaderStorageBuffer(uint32_t size, uint32_t drawType, void* data)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, drawType);
	}

	ShaderStorageBuffer::~ShaderStorageBuffer()
	{
		CleanUp();
	}

	
	void ShaderStorageBuffer::Bind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);

	}
	void ShaderStorageBuffer::BindIndex(unsigned int index)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_RendererID);
	}
	void ShaderStorageBuffer::UnBind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	}
	void ShaderStorageBuffer::CleanUp()
	{
		glDeleteBuffers(1, &m_RendererID);

	}

	/*----------------------------    IndirectBuffer   --------------------------------*/

	IndirectBuffer::IndirectBuffer(const std::vector<IndirectCommand>& commands, uint32_t drawType)
	{
		//feed the draw command data to the gpu
		glGenBuffers(1, &m_RendererID);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_RendererID);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(IndirectCommand), &commands[0], drawType);
		m_Commands = commands;
	}
	IndirectBuffer::~IndirectBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void IndirectBuffer::UpdateCommands(const std::vector<IndirectCommand>& cmds)
	{
		m_Commands = cmds;
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_RendererID);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, m_Commands.size() * sizeof(IndirectCommand), &m_Commands[0], GL_DYNAMIC_DRAW);

	}

	void IndirectBuffer::Bind()
	{
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_RendererID);
		

	}
	void IndirectBuffer::UnBind()
	{
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	}
}