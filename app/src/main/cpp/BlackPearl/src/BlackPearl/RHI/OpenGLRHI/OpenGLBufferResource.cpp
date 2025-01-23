#include "pch.h"
#include "glad/glad.h"
#include "OpenGLBufferResource.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/RHISampler.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLTexture.h"
#include <memory>


namespace BlackPearl {
	extern DeviceManager* g_deviceManager;

	VertexBuffer::VertexBuffer(const BufferDesc& _desc)
	:Buffer(_desc) {
		glGenBuffers(1, &rendererID);
	}
	
	//------------------------VertexBuffer-----------------//
	/* 
	darwType = GL_STATIC_DRAW / GL_DYNAMIC_DRAW (buffer can be read or written)
	*/
	VertexBuffer::VertexBuffer(const BufferDesc& _desc, const std::vector<float>& vertices, bool Interleaved, bool divisor, uint32_t perInstance , uint32_t drawType)
	:Buffer(_desc){
		m_VerticesFloat = (&vertices[0]);
#ifdef GE_API_OPENGL
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], drawType);
#endif

		m_VertexSize = vertices.size() * sizeof(float);
		m_Interleaved = Interleaved;
		if (divisor) {
			m_Divisor = true;
			m_DivPerInstance = perInstance;
		}
	}
	VertexBuffer::VertexBuffer(const BufferDesc& _desc, const float* vertices, uint32_t size, bool Interleaved , bool divisor, uint32_t perInstance, uint32_t drawType)
		:Buffer(_desc) {
		m_VerticesFloat = vertices;
		//TODO:: 使用RHIBuffer
#ifdef GE_API_OPENGL
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, drawType);
#endif
		m_VertexSize = size;
		m_Interleaved = Interleaved;
		if (divisor) {
			m_Divisor = true;
			m_DivPerInstance = perInstance;
		}
	}
	VertexBuffer::VertexBuffer(const BufferDesc& _desc, const unsigned int* vertices, uint32_t size, bool Interleaved, bool divisor, uint32_t perInstance, uint32_t drawType)
		:Buffer(_desc) {
		m_VerticesUint = vertices;
#ifdef GE_API_OPENGL
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, drawType);
#endif
		m_VertexSize = size;
		m_Interleaved = Interleaved;
		if (divisor) {
			m_Divisor = true;
			m_DivPerInstance = perInstance;
		}

	}
	VertexBuffer::VertexBuffer(const BufferDesc& _desc, void* vertices, uint32_t size, bool Interleaved, bool divisor, uint32_t perInstance, uint32_t drawType)
		:Buffer(_desc) {
		m_VerticesVoidData = vertices;
#ifdef GE_API_OPENGL
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
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
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
#endif
	}

	void VertexBuffer::UpdateData(void* vertices, uint32_t size, uint32_t drawType)
	{
		m_VerticesVoidData = vertices;
#ifdef GE_API_OPENGL
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
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
		glDeleteBuffers(1, &rendererID);
#endif
	}
	IndexBuffer::IndexBuffer(const BufferDesc& _desc)
		:Buffer(_desc) {
		glGenBuffers(1, &rendererID);
	}

	//------------------------IndexBuffer-----------------//
	IndexBuffer::IndexBuffer(const BufferDesc& _desc, const std::vector<unsigned int>& indices, uint32_t drawType)
		:Buffer(_desc) {
#ifdef GE_API_OPENGL
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], drawType);
#endif
		m_IndiciesSize = indices.size() * sizeof(unsigned int);
		m_Indicies = indices.data();
	}
	IndexBuffer::IndexBuffer(const BufferDesc& _desc, unsigned int * indices, unsigned int size, uint32_t drawType)
		:Buffer(_desc) {
#ifdef GE_API_OPENGL
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, drawType);
#endif
		m_IndiciesSize = size;
		m_Indicies = indices;
	}
	IndexBuffer::~IndexBuffer()
	{
#ifdef GE_API_OPENGL
		glDeleteBuffers(1, &rendererID);
#endif
	}
	void IndexBuffer::Bind()
	{
#ifdef GE_API_OPENGL
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
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


	Framebuffer::Framebuffer(const BufferDesc& _desc)
		:Buffer(_desc) {

		glGenFramebuffers(1, &rendererID);
	}

	//------------------------Framebuffer-----------------//
	//note: framebuffer has no memory, imageWidth, imageHeight is the width and height of the attachment! 不同attachment有不同的width和height
	Framebuffer::Framebuffer(const BufferDesc& _desc, const int imageWidth,int imageHeight,std::initializer_list<Attachment> attachment, unsigned int colorAttachmentPoint,bool disableColor, TextureType colorTextureType)
		:Buffer(_desc) {
		/* m_Width,m_Height for voxel cone tracing */
		m_Width = imageWidth;
		m_Height = imageHeight;
		GLint previousFrameBuffer;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFrameBuffer);//获取之前绑定的Framebuffer

		glGenFramebuffers(1, &rendererID);
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
		//绑定回原来的Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, previousFrameBuffer);

	}

	void Framebuffer::AttachColorTexture(TextureType textureType, unsigned int attachmentPoints,unsigned int imageWidth,unsigned int imageHeight)
	{
		GE_CORE_WARN(" 注意Texture是否是默认的格式！");

		TextureDesc desc;
		desc.type = textureType;
		desc.width = imageWidth;
		desc.height = imageHeight;
		desc.minFilter = FilterMode::Nearest;
		desc.magFilter = FilterMode::Nearest;
		desc.wrap = SamplerAddressMode::Repeat;
		desc.format = Format::RGBA16_FLOAT;
		desc.generateMipmap = true;
		g_deviceManager->GetDevice()->createTexture(desc);
		m_TextureColorBuffers[attachmentPoints] = g_deviceManager->GetDevice()->createTexture(desc);
		
		//将它附加到当前绑定的帧缓冲对象
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+attachmentPoints, GL_TEXTURE_2D, (GLuint)static_cast<Texture*>(m_TextureColorBuffers[attachmentPoints].Get())->GetRendererID(), 0);
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}

	void Framebuffer::AttachColorTexture(TextureHandle texture, unsigned int attachmentPoints)
	{
		//将它附加到当前绑定的帧缓冲对象
		m_TextureColorBuffers[attachmentPoints] = texture;
		Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentPoints, GL_TEXTURE_2D, (GLuint)static_cast<Texture*>(texture.Get())->GetRendererID(), 0);
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}
	//TODO:: 通过device->createFramebuffer(fboDesc);创建
	void Framebuffer::AttachDepthTexture(const int imageWidth, int imageHeight)
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
		m_TextureDepthBuffer = texture;
		Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, static_cast<Texture*>(m_TextureDepthBuffer.Get())->GetRendererID(), 0);

		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}

	void Framebuffer::AttachDepthTexture(TextureHandle texture, int mipmapLevel) {
		m_TextureDepthBuffer = texture;
		//Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, static_cast<Texture*>(m_TextureDepthBuffer.Get())->GetRendererID(), mipmapLevel);
		GLenum statue = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}



	//TODO::注意CubeMap的格式
	void Framebuffer::AttachCubeMapDepthTexture(TextureHandle cubeMap)
	{

		m_CubeMapDepthBuffer = cubeMap;// .reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, imageWidth, imageHeight, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT));
		Bind();
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, static_cast<Texture*>(m_CubeMapDepthBuffer.Get())->GetRendererID(), 0);
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}

	//TODO::这个接口有问题
	void Framebuffer::AttachCubeMapColorTexture(unsigned int attachmentPoints,const int imageWidth, int imageHeight) {
		//GE_CORE_ERROR("不能调用，后续完善!");
		GE_CORE_WARN(" 注意Texture是否是默认的格式！");
		TextureDesc desc;
		desc.type = TextureType::CubeMap;
		desc.width = imageWidth;
		desc.height = imageHeight;
		desc.minFilter = FilterMode::Linear_Mip_Linear;
		desc.magFilter = FilterMode::Linear;
		desc.wrap = SamplerAddressMode::ClampToEdge;
		desc.format = Format::RGB16_FLOAT;
		desc.generateMipmap = false;
		//texture->diffuseTextureMap = g_deviceManager->GetDevice()->createTexture(desc);
		m_TextureColorBuffers[attachmentPoints] = g_deviceManager->GetDevice()->createTexture(desc);
		//m_TextureColorBuffers[attachmentPoints].reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, imageWidth, imageHeight, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT));
	//	GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
		//Bind();

	}
	void Framebuffer::AttachCubeMapColorTexture(unsigned int attachmentPoints, TextureHandle cubeMap)
	{
		m_TextureColorBuffers[attachmentPoints] = cubeMap;
		//GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
	}
	void Framebuffer::AttachRenderBuffer(const int imageWidth, int imageHeight)
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

	void Framebuffer::DisableColorBuffer()
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	void Framebuffer::Bind()
	{
		//glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
		//glViewport(0, 0, width, height);

	}
	void Framebuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);

	}

	void Framebuffer::BindRenderBuffer()
	{	
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
	}
	

	void Framebuffer::CleanUp()
	{
		
		glDeleteRenderbuffers(1, &m_RenderBufferID);
		glDeleteFramebuffers(1,&rendererID);

	}
	void Framebuffer::SetViewPort(int width, int height)
	{
		m_Width = width;
		m_Height = height;
		glViewport(0, 0, m_Width, m_Height);
	}

	/*----------------------------    GBuffer   --------------------------------*/
	GBuffer::GBuffer(const BufferDesc& _desc, const unsigned int imageWidth, const unsigned int imageHeight, Type type)
		:Buffer(_desc) {

		m_Width  = imageWidth;
		m_Height = imageHeight;
		m_Type = type;

		
		glGenFramebuffers(1, &rendererID);
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
		glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

	}
	void GBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	// device->createTexture
	void GBuffer::InitGITextures()
	{

		TextureDesc desc;
		desc.type = TextureType::DiffuseMap;
		desc.width = m_Width;
		desc.height = m_Height;
		desc.minFilter = FilterMode::Nearest;
		desc.magFilter = FilterMode::Nearest;
		desc.wrap = SamplerAddressMode::ClampToEdge;
		desc.format = Format::RGBA16_FLOAT;
		desc.generateMipmap = false;
		//texture->diffuseTextureMap = g_deviceManager->GetDevice()->createTexture(desc);
		m_PositionTexture = g_deviceManager->GetDevice()->createTexture(desc);

		//m_PositionTexture RGB-position A--isPBRObject+objectId -->voxel cone tracing
		//m_PositionTexture.reset(DBG_NEW Texture(TextureType::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA16F, GL_RGBA, -1, GL_FLOAT));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (GLuint)static_cast<Texture*>(m_PositionTexture.Get())->GetRendererID(), 0);
		//m_NormalTexture RGB-normal A--isSkyBox -->voxel cone tracing
		//m_NormalTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA16F, GL_RGBA, -1, GL_FLOAT));
		m_NormalTexture = g_deviceManager->GetDevice()->createTexture(desc);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, (GLuint)static_cast<Texture*>(m_NormalTexture.Get())->GetRendererID(), 0);

		m_DiffuseRoughnessTexture = g_deviceManager->GetDevice()->createTexture(desc);
		//m_DiffuseRoughnessTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA16F, GL_RGBA, -1, GL_FLOAT));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, (GLuint)static_cast<Texture*>(m_DiffuseRoughnessTexture.Get())->GetRendererID(), 0);


		m_SpecularMentallicTexture = g_deviceManager->GetDevice()->createTexture(desc);
		//m_SpecularMentallicTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA16F, GL_RGBA, -1, GL_FLOAT));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, (GLuint)static_cast<Texture*>(m_SpecularMentallicTexture.Get())->GetRendererID(), 0);

		//m_AmbientGI RBG-ambienGI AO-isPBRObject -->lightprobe
		m_AmbientGIAOTexture = g_deviceManager->GetDevice()->createTexture(desc);
		//m_AmbientGIAOTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA16F, GL_RGBA, -1, GL_FLOAT));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, (GLuint)static_cast<Texture*>(m_AmbientGIAOTexture.Get())->GetRendererID(), 0);

		desc.format = Format::RGB16_FLOAT;
		m_NormalMapTexture = g_deviceManager->GetDevice()->createTexture(desc);
		//m_NormalMapTexture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGB16F, GL_RGB, -1, GL_FLOAT));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, (GLuint)static_cast<Texture*>(m_NormalMapTexture.Get())->GetRendererID(), 0);


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

			TextureHandle texture;
			TextureDesc desc;
			desc.type = TextureType::DiffuseMap;
			desc.width = m_Width;
			desc.height = m_Height;
			desc.minFilter = FilterMode::Nearest;
			desc.magFilter = FilterMode::Nearest;
			desc.wrap = SamplerAddressMode::ClampToEdge;
			desc.format = Format::RGBA32_FLOAT;
			desc.generateMipmap = false;
			//texture->diffuseTextureMap = g_deviceManager->GetDevice()->createTexture(desc);
			texture = g_deviceManager->GetDevice()->createTexture(desc);


			//texture.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, m_Width, m_Height, false, GL_NEAREST, GL_NEAREST, GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, (GLuint)static_cast<Texture*>(texture.Get())->GetRendererID(), 0);
			m_ColorTextures.push_back(texture);

		}
	
		const GLenum attachments[colorBufferNum] = {
			GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
		};
		glDrawBuffers(colorBufferNum, attachments);
		GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");



	}
	/*----------------------------    AtomicBuffer   --------------------------------*/
	
	AtomicBuffer::AtomicBuffer(const BufferDesc& _desc)
		:Buffer(_desc) {
		GLuint initVal = 0;
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, rendererID);
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
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, rendererID);
	}
	void AtomicBuffer::BindIndex(unsigned int index)
	{
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, index, rendererID);
	}
	void AtomicBuffer::UnBind()
	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}
	void AtomicBuffer::CleanUp()
	{
		glDeleteBuffers(1, &rendererID);
	}


	ShaderStorageBuffer::ShaderStorageBuffer(const BufferDesc& _desc)
		:Buffer(_desc) {
		glCreateBuffers(1, &rendererID);

	}
	/*----------------------------    SSBO   --------------------------------*/
	ShaderStorageBuffer::ShaderStorageBuffer(const BufferDesc& _desc, GLsizeiptr bytes,GLbitfield mapFlags)
		:Buffer(_desc) {
		GLuint initVal = 0;

		//glGenBuffers(1, &rendererID);
		glCreateBuffers(1, &rendererID);
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererID);
		glNamedBufferStorage(rendererID, bytes, nullptr, mapFlags);
		//glBufferData(GL_SHADER_STORAGE_BUFFER, bytes, nullptr, GL_STATIC_COPY);
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}


	ShaderStorageBuffer::ShaderStorageBuffer(const BufferDesc& _desc, uint32_t size, uint32_t drawType, void* data)
		:Buffer(_desc) {
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, drawType);
	}

	ShaderStorageBuffer::~ShaderStorageBuffer()
	{
		CleanUp();
	}

	
	void ShaderStorageBuffer::Bind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererID);

	}
	void ShaderStorageBuffer::BindIndex(unsigned int index)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, rendererID);
	}
	void ShaderStorageBuffer::UnBind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	}
	void ShaderStorageBuffer::CleanUp()
	{
		glDeleteBuffers(1, &rendererID);

	}

	/*----------------------------    IndirectBuffer   --------------------------------*/

	IndirectBuffer::IndirectBuffer(const BufferDesc& _desc)
	:Buffer(_desc) {
		glGenBuffers(1, &rendererID);
	}

	IndirectBuffer::IndirectBuffer(const BufferDesc& _desc, const std::vector<IndirectCommand>& commands, uint32_t drawType)
		:Buffer(_desc) {
		//feed the draw command data to the gpu
		glGenBuffers(1, &rendererID);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, rendererID);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(IndirectCommand), &commands[0], drawType);
		m_Commands = commands;
	}
	IndirectBuffer::~IndirectBuffer()
	{
		glDeleteBuffers(1, &rendererID);
	}

	void IndirectBuffer::UpdateCommands(const std::vector<IndirectCommand>& cmds)
	{
		m_Commands = cmds;
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, rendererID);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, m_Commands.size() * sizeof(IndirectCommand), &m_Commands[0], GL_DYNAMIC_DRAW);

	}

	void IndirectBuffer::Bind()
	{
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, rendererID);
		

	}
	void IndirectBuffer::UnBind()
	{
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	}
}