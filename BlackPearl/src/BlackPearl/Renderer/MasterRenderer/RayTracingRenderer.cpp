#include "pch.h"
#include "RayTracingRenderer.h"
#include "BlackPearl/Common/CommonFunc.h"
#include "BlackPearl/Math/Math.h"
namespace BlackPearl {

	RayTracingRenderer::RayTracingRenderer() {

		m_BasicShader.reset(DBG_NEW Shader("assets/shaders/raytracing/rayTracingBasic.glsl"));
		m_BasicSystemShader.reset(DBG_NEW Shader("assets/shaders/raytracing/rayTracingSystemT.glsl"));
		m_MaterialShader.reset(DBG_NEW Shader("assets/shaders/raytracing/rayTracingMaterial.glsl"));
		m_GroupShader.reset(DBG_NEW Shader("assets/shaders/raytracing/rayTracingGenData_04.glsl"));

		m_ScreenShader.reset(DBG_NEW Shader("assets/shaders/raytracing/ScreenQuad.glsl"));
		m_GBuffers[0].reset(DBG_NEW GBuffer(Configuration::WindowWidth, Configuration::WindowHeight, GBuffer::RayTracing));
		m_GBuffers[1].reset(DBG_NEW GBuffer(Configuration::WindowWidth, Configuration::WindowHeight, GBuffer::RayTracing));

		m_BVHNodeShader.reset(DBG_NEW Shader("assets/shaders/raytracing/rayTracingBVHNode.glsl"));
		m_SceneBuilder.reset(DBG_NEW SceneBuilder());

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

	void RayTracingRenderer::RenderMaterialSpheres(MainCamera* mainCamera)
	{
		m_MaterialShader->Bind();
		m_MaterialShader->SetUniform1i("origin_curRayNum", 0);
		m_MaterialShader->SetUniform1i("dir_tMax", 1);
		m_MaterialShader->SetUniform1i("color_time", 2);
		m_MaterialShader->SetUniform1i("RTXRst", 3);
		m_MaterialShader->SetUniform1f("u_rayNumMax", m_RayNumMax);
		m_MaterialShader->SetUniformVec2f("u_Screen", glm::vec2(Configuration::WindowWidth, Configuration::WindowHeight));

		m_MaterialShader->SetUniformVec3f("u_CameraUp", mainCamera->Up());
		m_MaterialShader->SetUniformVec3f("u_CameraFront", mainCamera->Front());
		m_MaterialShader->SetUniformVec3f("u_CameraRight", mainCamera->Right());
		m_MaterialShader->SetUniform1f("u_CameraFov", mainCamera->Fov());

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

			m_MaterialShader->SetUniform1f("u_rdSeed[0]", Math::Rand_F());
			m_MaterialShader->SetUniform1f("u_rdSeed[1]", Math::Rand_F());
			m_MaterialShader->SetUniform1f("u_rdSeed[2]", Math::Rand_F());
			m_MaterialShader->SetUniform1f("u_rdSeed[3]", Math::Rand_F());

			m_MaterialShader->Bind();
			DrawObject(m_Quad, m_MaterialShader);
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

	void RayTracingRenderer::InitGroupData(Group* group)
	{
		GE_ASSERT(group, "group is null");
		
		m_GenData.reset(DBG_NEW GenData(group->GetRoot()));
		m_SceneData = m_GenData->GetSceneData();
		m_MaterialData = m_GenData->GetMatData();
		m_TextureData = m_GenData->GetTexData();
		m_PackData = m_GenData->GetPackData();

		m_SceneDataTex.reset(DBG_NEW TextureImage2D(m_SceneData, m_SceneData.size(), 1,\
			GL_NEAREST, GL_NEAREST, GL_R32F, GL_RED, GL_CLAMP_TO_EDGE, GL_FLOAT, GL_READ_WRITE));
		m_TexDataTex.reset(DBG_NEW TextureImage2D(m_TextureData, m_TextureData.size(), 1, \
			GL_NEAREST, GL_NEAREST, GL_R32F, GL_RED, GL_CLAMP_TO_EDGE, GL_FLOAT, GL_READ_WRITE));
		m_MaterialDataTex.reset(DBG_NEW TextureImage2D(m_MaterialData, m_MaterialData.size(), 1, \
		GL_NEAREST, GL_NEAREST, GL_R32F, GL_RED, GL_CLAMP_TO_EDGE, GL_FLOAT, GL_READ_WRITE));

		m_PackDataTex.reset(DBG_NEW TextureImage2D(m_PackData, m_PackData.size(), 1, \
			GL_NEAREST, GL_NEAREST, GL_R32F, GL_RED, GL_CLAMP_TO_EDGE, GL_FLOAT, GL_READ_WRITE));
	
	}

	void RayTracingRenderer::InitScene(std::shared_ptr<GenData> scene)
	{
		m_SceneData = scene->GetSceneData();
		m_MaterialData = scene->GetMatData();
		m_TextureData = scene->GetTexData();
		m_PackData = scene->GetPackData();

		m_SceneDataTex.reset(DBG_NEW TextureImage2D(m_SceneData, m_SceneData.size(), 1, \
			GL_NEAREST, GL_NEAREST, GL_R32F, GL_RED, GL_CLAMP_TO_EDGE, GL_FLOAT, GL_READ_WRITE));
		m_TexDataTex.reset(DBG_NEW TextureImage2D(m_TextureData, m_TextureData.size(), 1, \
			GL_NEAREST, GL_NEAREST, GL_R32F, GL_RED, GL_CLAMP_TO_EDGE, GL_FLOAT, GL_READ_WRITE));
		m_MaterialDataTex.reset(DBG_NEW TextureImage2D(m_MaterialData, m_MaterialData.size(), 1, \
			GL_NEAREST, GL_NEAREST, GL_R32F, GL_RED, GL_CLAMP_TO_EDGE, GL_FLOAT, GL_READ_WRITE));

		m_PackDataTex.reset(DBG_NEW TextureImage2D(m_PackData, m_PackData.size(), 1, \
			GL_NEAREST, GL_NEAREST, GL_R32F, GL_RED, GL_CLAMP_TO_EDGE, GL_FLOAT, GL_READ_WRITE));
	}

	void RayTracingRenderer::RenderGroup(MainCamera* mainCamera, Object* group)
	{
		m_GroupShader->Bind();
		m_GroupShader->SetUniform1i("origin_curRayNum", 0);
		m_GroupShader->SetUniform1i("dir_tMax", 1);
		m_GroupShader->SetUniform1i("color_time", 2);
		m_GroupShader->SetUniform1i("RTXRst", 3);
		m_GroupShader->SetUniform1f("u_rayNumMax", m_RayNumMax);
		m_GroupShader->SetUniformVec2f("u_Screen", glm::vec2(Configuration::WindowWidth, Configuration::WindowHeight));

		m_GroupShader->SetUniformVec3f("u_CameraUp", mainCamera->Up());
		m_GroupShader->SetUniformVec3f("u_CameraFront", mainCamera->Front());
		m_GroupShader->SetUniformVec3f("u_CameraRight", mainCamera->Right());
		m_GroupShader->SetUniform1f("u_CameraFov", mainCamera->Fov());
		
		m_GroupShader->SetUniform1i("SceneData", 4);
		m_GroupShader->SetUniform1i("MatData", 5);
		m_GroupShader->SetUniform1i("TexData", 6);
		m_GroupShader->SetUniform1i("PackData", 7);

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

			m_SceneDataTex->Bind(4);
			m_MaterialDataTex->Bind(5);
			m_TexDataTex->Bind(6);
			m_PackDataTex->Bind(7);

		



			m_GroupShader->SetUniform1f("u_rdSeed[0]", Math::Rand_F());
			m_GroupShader->SetUniform1f("u_rdSeed[1]", Math::Rand_F());
			m_GroupShader->SetUniform1f("u_rdSeed[2]", Math::Rand_F());
			m_GroupShader->SetUniform1f("u_rdSeed[3]", Math::Rand_F());

			m_GroupShader->Bind();
			DrawObject(m_Quad, m_GroupShader);
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

	void RayTracingRenderer::RenderBVHNode(MainCamera* mainCamera, Object* bvh_node) {

		m_GroupShader->Bind();
		m_GroupShader->SetUniform1i("origin_curRayNum", 0);
		m_GroupShader->SetUniform1i("dir_tMax", 1);
		m_GroupShader->SetUniform1i("color_time", 2);
		m_GroupShader->SetUniform1i("RTXRst", 3);
		m_GroupShader->SetUniform1f("u_rayNumMax", m_RayNumMax);
		m_GroupShader->SetUniformVec2f("u_Screen", glm::vec2(Configuration::WindowWidth, Configuration::WindowHeight));

		m_GroupShader->SetUniformVec3f("u_CameraUp", mainCamera->Up());
		m_GroupShader->SetUniformVec3f("u_CameraFront", mainCamera->Front());
		m_GroupShader->SetUniformVec3f("u_CameraRight", mainCamera->Right());
		m_GroupShader->SetUniform1f("u_CameraFov", mainCamera->Fov());

		m_GroupShader->SetUniform1i("SceneData", 4);
		m_GroupShader->SetUniform1i("MatData", 5);
		m_GroupShader->SetUniform1i("TexData", 6);
		m_GroupShader->SetUniform1i("PackData", 7);

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

			m_SceneDataTex->Bind(4);
			m_MaterialDataTex->Bind(5);
			m_TexDataTex->Bind(6);
			m_PackDataTex->Bind(7);





			m_GroupShader->SetUniform1f("u_rdSeed[0]", Math::Rand_F());
			m_GroupShader->SetUniform1f("u_rdSeed[1]", Math::Rand_F());
			m_GroupShader->SetUniform1f("u_rdSeed[2]", Math::Rand_F());
			m_GroupShader->SetUniform1f("u_rdSeed[3]", Math::Rand_F());

			m_GroupShader->Bind();
			DrawObject(m_Quad, m_GroupShader);
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

