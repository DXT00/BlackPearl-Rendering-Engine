#include "pch.h"

#include "DepthRenderer.h"
#include "BlackPearl/Renderer/DeviceManager.h"

namespace BlackPearl {

	extern DeviceManager* g_deviceManager;

	DepthRenderer::DepthRenderer()
	{
	}

	DepthRenderer::~DepthRenderer()
	{
	}

	void DepthRenderer::Init()
	{
		m_DepthShader = std::make_shared<Shader>("assets/shaders/occlusionCulling/depthMap.glsl");
		m_HizShader = std::make_shared<Shader>("assets/shaders/occlusionCulling/hiz.glsl");





		IDevice* device = g_deviceManager->GetDevice();
		TextureDesc desc;
		desc.type = TextureType::DepthMap;
		desc.width = Configuration::WindowWidth;
		desc.height = Configuration::WindowHeight;
		desc.minFilter = FilterMode::Nearest_Mip_Nearnest;
		desc.magFilter = FilterMode::Nearest;
		desc.wrap = SamplerAddressMode::ClampToEdge;
		desc.format = Format::D32;
		desc.generateMipmap = true;


		m_HizFrameBuffer.reset(DBG_NEW FrameBuffer());
		m_DepthTexture = device->createTexture(desc);
		//m_DepthTexture.reset(DBG_NEW Texture(Texture::Type::DepthMap, Configuration::WindowWidth, Configuration::WindowHeight, true/*isDepth*/, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_CLAMP_TO_EDGE, GL_FLOAT, true/*genmipmap*/));
		m_HizFrameBuffer->Bind();
		m_HizFrameBuffer->AttachDepthTexture(m_DepthTexture, 0);
		glDrawBuffer(GL_NONE); // No color buffer is drawn to.
		glReadBuffer(GL_NONE);
		m_HizFrameBuffer->UnBind();

	}

	void DepthRenderer::RenderDepthMap(Object* Quad, const std::vector<Object*> backgroundObjs)
	{
		m_DepthShader->Bind();

		glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_HizFrameBuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		//m_HizFrameBuffer->AttachDepthTexture(m_DepthTexture, 0);
		//render scene to depth buffer
		for (size_t i = 0; i < backgroundObjs.size(); i++)
		{
			auto obj = backgroundObjs[i];
			DrawObject(obj, m_DepthShader);
		}

		m_HizFrameBuffer->UnBind();

		glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//CommonFunc::ShowFrameBuffer(1, 1, Quad, m_HizFrameBuffer, { m_HizFrameBuffer->GetDepthTexture() });
		//CommonFunc::ShowFrameBuffer(glm::vec4(0, 0, Configuration::WindowWidth, Configuration::WindowHeight), Quad, m_HizFrameBuffer, m_HizFrameBuffer->GetDepthTexture(), true/*isMipmap*/, 0);



		/**************************** Gnererate Hiz depth map *******************************/
		m_HizShader->Bind();
		m_HizFrameBuffer->Bind();


		m_MipmapLevel = 1 + (int)log2f(fmaxf(Configuration::WindowWidth, Configuration::WindowHeight));
		int curWidth = Configuration::WindowWidth;
		int curHeight = Configuration::WindowHeight;

		// disable color buffer as we will render only a depth image
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthFunc(GL_ALWAYS);


		glActiveTexture(GL_TEXTURE0);
		m_HizFrameBuffer->GetDepthTexture()->Bind();
		m_HizShader->SetUniform1i("u_LastMip", 0);


		for (size_t i = 1; i < m_MipmapLevel; i++)
		{
			m_HizShader->Bind();
			m_HizShader->SetUniformVec2i("u_LastMipSize", glm::ivec2(curWidth, curHeight));

			curWidth /= 2;
			curHeight /= 2;
			curWidth = curWidth > 0 ? curWidth : 1;
			curHeight = curHeight > 0 ? curHeight : 1;
			glViewport(0, 0, curWidth, curHeight);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, i - 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i - 1);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, static_cast<Texture*>(m_DepthTexture.Get())->GetRendererID(), i);
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
			m_HizShader->SetUniform1i("u_LastMipLevel", i - 1);
			//glActiveTexture(GL_TEXTURE0);
			//m_HizFrameBuffer->GetDepthTexture()->Bind();
			//m_HizShader->SetUniform1i("u_LastMip", 0);

			DrawObject(Quad, m_HizShader);

		}

		//reset mipmap level range, and bind lod 0
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_MipmapLevel - 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, static_cast<Texture*>(m_DepthTexture.Get())->GetRendererID(), 0);


		glDepthFunc(GL_LEQUAL);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);




		m_HizFrameBuffer->UnBind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

