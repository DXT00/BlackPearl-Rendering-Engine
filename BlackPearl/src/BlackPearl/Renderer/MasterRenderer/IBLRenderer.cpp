#include "pch.h"
#include "IBLRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
namespace BlackPearl {
	int IBLRenderer::s_Width = 512;
	int IBLRenderer::s_Height = 512;
	IBLRenderer::IBLRenderer()
	{

		m_FrameBuffer.reset(DBG_NEW FrameBuffer(s_Width, s_Height,
			{ FrameBuffer::Attachment::CubeMapColorTexture,FrameBuffer::Attachment::RenderBuffer }, false));

		//Load Hdr environment map
		m_HdrMapToCubeShader.reset(DBG_NEW Shader("assets/shaders/ibl/hdrMapToCube.glsl"));
		//m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/sIBL-Collection/sIBL_Collection/backlot/Diffuse10.hdr")); //TODO::
		//m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/sIBL-Collection/sIBL_Collection/apartment/Apartment_Reflection.hdr")); //TODO::
		m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/Desert_Highway/Road_to_MonumentValley_Ref.hdr")); //TODO::

	}
	void IBLRenderer::Init(Object * cubeObj)
	{
		GE_ASSERT(cubeObj, "m_CubeObj is nullptr!");
		m_CubeObj = cubeObj;

		float aspect = (float)s_Width / (float)s_Height;
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 10.0f);
		glm::mat4 captureProjectionViews[] =
		{
			captureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			captureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			captureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			captureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			captureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			captureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
		m_HdrMapToCubeShader->Bind();
		glActiveTexture(GL_TEXTURE0 + 1);
		m_HdrTexture->Bind();
		m_HdrMapToCubeShader->SetUniform1i("hdrTexture", 1);


		//Draw CubeMap from hdrMap

		glViewport(0, 0, s_Width, s_Height);
		m_FrameBuffer->Bind();
		for (unsigned int i = 0; i < 6; i++)
		{
			m_HdrMapToCubeShader->SetUniformMat4f("u_CubeMapProjectionView", captureProjectionViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,m_FrameBuffer->GetCubeMapColorTexture()->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_CubeObj->GetComponent<MeshRenderer>()->SetShaders(m_HdrMapToCubeShader);

			DrawObject(m_CubeObj, m_HdrMapToCubeShader);
		}
		m_FrameBuffer->UnBind();

		// initialize static shader uniforms before rendering
		m_IsInitialize = true;
	}
	IBLRenderer::~IBLRenderer()
	{
	}
	void IBLRenderer::Render(std::vector<Object*> objs)
	{
		GE_ASSERT(m_IsInitialize, "Please Call IBLRenderer::Init() first!");


	}
}