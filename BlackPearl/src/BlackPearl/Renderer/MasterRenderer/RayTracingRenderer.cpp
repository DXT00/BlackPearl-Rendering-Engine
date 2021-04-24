#include "pch.h"
#include "RayTracingRenderer.h"
#include "BlackPearl/Common/CommonFunc.h"
#include "BlackPearl/Math/Math.h"
namespace BlackPearl {

	RayTracingRenderer::RayTracingRenderer() {

		m_BasicShader.reset(DBG_NEW Shader("assets/shaders/raytracing/rayTracingBasic.glsl"));
		m_BasicSystemShader.reset(DBG_NEW Shader("assets/shaders/raytracing/rayTracingSystemT.glsl"));
		m_ScreenShader.reset(DBG_NEW Shader("assets/shaders/raytracing/ScreenQuad.glsl"));
		m_GBuffers[0].reset(DBG_NEW GBuffer(Configuration::WindowWidth, Configuration::WindowHeight, GBuffer::RayTracing));
		m_GBuffers[1].reset(DBG_NEW GBuffer(Configuration::WindowWidth, Configuration::WindowHeight, GBuffer::RayTracing));

	}

	void RayTracingRenderer::Init(Object* quad)
	{
		GE_ASSERT(quad, "quad is null");
		m_Quad = quad;
	}

	void RayTracingRenderer::Render()
	{
		m_BasicShader->Bind();
		m_BasicShader->SetUniform1i("origin_curRayNum", 0);
		m_BasicShader->SetUniform1i("dir_tMax", 1);
		m_BasicShader->SetUniform1i("color_time", 2);
		m_BasicShader->SetUniform1i("RTXRst", 3);
		m_BasicShader->SetUniform1f("u_rayNumMax", m_RayNumMax);
	
		for (int i = 0; i < m_LoopNum; i++)
		{
			glActiveTexture(GL_TEXTURE0);
			m_GBuffers[m_ReadBuffer]->GetColorTexture(0)->Bind();
			glActiveTexture(GL_TEXTURE1);
			m_GBuffers[m_ReadBuffer]->GetColorTexture(1)->Bind();
			glActiveTexture(GL_TEXTURE2);
			m_GBuffers[m_ReadBuffer]->GetColorTexture(2)->Bind();
			glActiveTexture(GL_TEXTURE3);
			m_GBuffers[m_ReadBuffer]->GetColorTexture(3)->Bind();
			
			m_GBuffers[m_WriteBuffer]->Bind();
			m_BasicShader->Bind();

			DrawObject(m_Quad, m_BasicShader);
			m_ReadBuffer = m_WriteBuffer;
			m_WriteBuffer = !m_ReadBuffer;

		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	CommonFunc::ShowGBuffer(2, 2, m_Quad, m_GBuffers[m_ReadBuffer], m_GBuffers[m_ReadBuffer]->GetColorTextures());

		m_ScreenShader->Bind();
		m_ScreenShader->SetUniform1i("u_FinalScreenTexture", 5);
		glActiveTexture(GL_TEXTURE5);
		m_GBuffers[m_ReadBuffer]->GetColorTexture(3)->Bind();
		
		DrawObject(m_Quad, m_ScreenShader);


	}

	void RayTracingRenderer::RenderSpheres(MainCamera* mainCamera)
	{
		m_BasicSystemShader->Bind();
		m_BasicSystemShader->SetUniform1i("origin_curRayNum", 0);
		m_BasicSystemShader->SetUniform1i("dir_tMax", 1);
		m_BasicSystemShader->SetUniform1i("color_time", 2);
		m_BasicSystemShader->SetUniform1i("RTXRst", 3);
		m_BasicSystemShader->SetUniform1f("u_rayNumMax", m_RayNumMax);
		m_BasicSystemShader->SetUniformVec2f("u_Screen", glm::vec2(Configuration::WindowWidth, Configuration::WindowHeight));
		
		m_BasicSystemShader->SetUniformVec3f("u_CameraUp", mainCamera->Up());
		m_BasicSystemShader->SetUniformVec3f("u_CameraFront", mainCamera->Front());
		m_BasicSystemShader->SetUniformVec3f("u_CameraRight", mainCamera->Right());
		m_BasicSystemShader->SetUniform1f("u_CameraFov", mainCamera->Fov());

		for (int i = 0; i < m_LoopNum; i++)
		{			
			m_GBuffers[m_WriteBuffer]->Bind();

			glActiveTexture(GL_TEXTURE0);
			m_GBuffers[m_ReadBuffer]->GetColorTexture(0)->Bind();
			glActiveTexture(GL_TEXTURE1);
			m_GBuffers[m_ReadBuffer]->GetColorTexture(1)->Bind();
			glActiveTexture(GL_TEXTURE2);
			m_GBuffers[m_ReadBuffer]->GetColorTexture(2)->Bind();
			glActiveTexture(GL_TEXTURE3);
			m_GBuffers[m_ReadBuffer]->GetColorTexture(3)->Bind();

			
			

			m_BasicSystemShader->SetUniform1f("u_rdSeed[0]", Math::Rand_F());
			m_BasicSystemShader->SetUniform1f("u_rdSeed[1]", Math::Rand_F());
			m_BasicSystemShader->SetUniform1f("u_rdSeed[2]", Math::Rand_F());
			m_BasicSystemShader->SetUniform1f("u_rdSeed[3]", Math::Rand_F());

			m_BasicSystemShader->Bind();
			DrawObject(m_Quad, m_BasicSystemShader);
			m_ReadBuffer = m_WriteBuffer;
			m_WriteBuffer = !m_ReadBuffer;

		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//	CommonFunc::ShowGBuffer(2, 2, m_Quad, m_GBuffers[m_ReadBuffer], m_GBuffers[m_ReadBuffer]->GetColorTextures());

		m_ScreenShader->Bind();
		m_ScreenShader->SetUniform1i("u_FinalScreenTexture", 5);
		glActiveTexture(GL_TEXTURE5);
		m_GBuffers[m_ReadBuffer]->GetColorTexture(3)->Bind();

		DrawObject(m_Quad, m_ScreenShader);
	}

	
	

}

