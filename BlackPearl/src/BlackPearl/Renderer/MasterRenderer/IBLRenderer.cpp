#include "pch.h"
#include "IBLRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
namespace BlackPearl {
	int IBLRenderer::s_Width = 512;
	int IBLRenderer::s_Height = 512;
	IBLRenderer::IBLRenderer()
	{

		m_FrameBuffer.reset(DBG_NEW FrameBuffer(s_Width, s_Height,
			{ FrameBuffer::Attachment::CubeMapColorTexture,FrameBuffer::Attachment::RenderBuffer }, 0,false));

		//Load Hdr environment map
		m_HdrMapToCubeShader.reset(DBG_NEW Shader("assets/shaders/ibl/hdrMapToCube.glsl"));
		m_IBLShader.reset(DBG_NEW Shader("assets/shaders/ibl/ibl.glsl"));
		//	m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/sIBL-Collection/sIBL_Collection/backlot/Diffuse10.hdr")); //TODO::
		m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/sIBL-Collection/sIBL_Collection/OfficeEden/Eden_REF.hdr")); //TODO::																																  //m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/sIBL-Collection/sIBL_Collection/apartment/Apartment_Reflection.hdr")); //TODO::
		//m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/Desert_Highway/Road_to_MonumentValley_Ref.hdr")); //TODO::
		m_IrradianceShader.reset(DBG_NEW Shader("assets/shaders/ibl/irradianceConvolution.glsl"));
	}
	void IBLRenderer::Init(Object * cubeObj)
	{
		GE_ASSERT(cubeObj, "m_CubeObj is nullptr!");
		m_CubeObj = cubeObj;
		RenderHdrMapToEnvironmentCubeMap();
		RenderIrradianceMap();
		m_IsInitialize = true;
	}
	IBLRenderer::~IBLRenderer()
	{
	}
	void IBLRenderer::RenderHdrMapToEnvironmentCubeMap() {


		float aspect = (float)s_Width / (float)s_Height;
		m_CaptureProjection = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 10.0f);
		m_CaptureProjectionViews =
		{
			m_CaptureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			m_CaptureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			m_CaptureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			m_CaptureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			m_CaptureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			m_CaptureProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
		m_HdrMapToCubeShader->Bind();
		glActiveTexture(GL_TEXTURE0 + 1);
		m_HdrTexture->Bind();
		m_HdrMapToCubeShader->SetUniform1i("hdrTexture", 1);

		//Draw CubeMap from hdrMap
		glViewport(0, 0, s_Width, s_Height);
		m_FrameBuffer->Bind();

		/*
		面向立方体六个面设置六个不同的视图矩阵，
		给定投影矩阵的 fov 为 90 度以捕捉整个面，
		并渲染立方体六次，将结果存储在浮点帧缓冲中
		*/
		for (unsigned int i = 0; i < 6; i++)
		{
			m_HdrMapToCubeShader->SetUniformMat4f("u_CubeMapProjectionView", m_CaptureProjectionViews[i]);
			//将CubeMap的六个面附加到FrameBuffer的GL_COLOR_ATTACHMENT0中
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_FrameBuffer->GetCubeMapColorTexture(0)->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_CubeObj->GetComponent<MeshRenderer>()->SetShaders(m_HdrMapToCubeShader);

			DrawObject(m_CubeObj, m_HdrMapToCubeShader);
		}
		m_HdrCubeMapID = m_FrameBuffer->GetCubeMapColorTexture(0)->GetRendererID();
		m_FrameBuffer->UnBind();
	}
	void IBLRenderer::RenderIrradianceMap()
	{
		

		
		glGenTextures(1, &m_IrradianceCubeMapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceCubeMapID);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		m_FrameBuffer->Bind();
		glBindRenderbuffer(GL_RENDERBUFFER, m_FrameBuffer->GetRenderBufferID());
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);


		m_IrradianceShader->Bind();
		m_IrradianceShader->SetUniform1i("u_EnvironmentMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_HdrCubeMapID);
		// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
		// -----------------------------------------------------------------------------


		glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
		m_FrameBuffer->Bind();

		for (unsigned int i = 0; i < 6; ++i)
		{
			m_IrradianceShader->SetUniformMat4f("u_CubeMapProjectionView", m_CaptureProjectionViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_IrradianceCubeMapID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			DrawObject(m_CubeObj, m_IrradianceShader);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void IBLRenderer::RenderSpheres(Object * sphere)
	{
		int rows = 7;
		int cols = 7;
		int spacing = 2;
		m_IBLShader->Bind();
		m_IBLShader->SetUniform1i("u_IrradianceMap", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceCubeMapID);


		m_IBLShader->SetUniform1f("u_ao", 1.0f);
		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {

				sphere->GetComponent<Transform>()->SetPosition({
					(col - (cols / 2)) * spacing,
					(row - (rows / 2)) * spacing,
					0.0f });
				m_IBLShader->SetUniform1f("u_metallic", 2.0*(float)row / (float)rows);
				m_IBLShader->SetUniform1f("u_roughness", glm::clamp((float)col / (float)cols, 0.05f, 1.0f));
				sphere->GetComponent<MeshRenderer>()->SetShaders(m_IBLShader);
				DrawObject(sphere, m_IBLShader);

			}
		}
	}
	void IBLRenderer::Render(std::vector<Object*> objs)
	{
	}
}