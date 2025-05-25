#include "pch.h"
#include "Renderer/MasterRenderer/GBufferRenderer.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"

#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/LightComponent/DirectionLight.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"
#include "BlackPearl/Component/LightProbeComponent/LightProbeComponent.h"
#include "glm/gtc/random.hpp"
#include <glm\ext\scalar_common.hpp>
#include "BlackPearl/Timestep/TimeCounter.h"
#include "BlackPearl/Math/Math.h"
#include <chrono>
#include "glm/glm.hpp"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLTexture.h"
#ifdef GE_API_OPENGL
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#endif
namespace BlackPearl {


	GBufferRenderer::GBufferRenderer(IDevice* device)
    : BasicRenderer(device)
	{
		/*m_GBuffer.reset(DBG_NEW GBuffer(m_TextureWidth, m_TexxtureHeight));
		TextureDesc desc;
		desc.type = TextureType::None;
		desc.width = m_TextureWidth;
		desc.height = m_TexxtureHeight;
		desc.minFilter = FilterMode::Linear;
		desc.magFilter = FilterMode::Linear;
		desc.wrap = SamplerAddressMode::ClampToEdge;
		desc.format = Format::RGBA16_FLOAT;
		desc.generateMipmap = true;*/

		//m_SSRTestTexture = device->createTexture(desc);
		//m_SSRTestTexture.reset(DBG_NEW Texture(Texture::Type::None, m_TextureWidth, m_TexxtureHeight, false, GL_LINEAR, GL_LINEAR, GL_RGBA16F, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));


	
		//m_HDRPostProcessTexture.reset(DBG_NEW Texture(Texture::Type::None, m_TextureWidth, m_TexxtureHeight, false, GL_LINEAR, GL_LINEAR, GL_RGBA16F, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));
		
		//m_HDRPostProcessTexture = device->createTexture(desc);
		
		/*m_LightPassFrameBuffer.reset(DBG_NEW FrameBuffer());
		m_LightPassFrameBuffer->Bind();
		m_LightPassFrameBuffer->AttachRenderBuffer(m_TextureWidth, m_TexxtureHeight);
		m_LightPassFrameBuffer->AttachColorTexture(m_HDRPostProcessTexture, 0);
		m_LightPassFrameBuffer->AttachColorTexture(m_SSRTestTexture, 1);
		m_LightPassFrameBuffer->UnBind();
*/

		


		//m_SkyboxRenderer = DBG_NEW SkyboxRenderer();

	}

	void GBufferRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
	{

        commandList->beginMarker("GBufferPass");
        GE_ERROR_JUDGE();



        SceneData* view = Renderer::GetSceneData();
        GE_ERROR_JUDGE();

        SceneData* preView = Renderer::GetPreSceneData();
        GE_ERROR_JUDGE();

        SetupView(commandList, view, preView);

        m_DrawStrategy->PrepareForView(scene, *view);
        GE_ERROR_JUDGE();

        RenderPassTemplate(commandList, targetFramebuffer, view, m_DrawStrategy, m_ShaderParameters);


        commandList->endMarker();
	}

	void GBufferRenderer::Init()
	{
        m_DrawStrategy = DBG_NEW InstancedOpaqueDrawStrategy();
        ShaderDesc desc = ShaderDesc(ShaderType::All);
        desc.debugName = "GbufferShader";
        m_GBufferShader = DBG_NEW MaterialShader("assets/shaders/glsl/gBuffer/gBuffer_pass.glsl");


        m_ShaderParameters[ShaderType::Pixel].bindingLayouts.push_back(m_ViewBindinglayout);
        m_ShaderParameters[ShaderType::Pixel].bindingSets.push_back(m_ViewBindingset);
        m_ShaderParameters->PixelShader  = m_GBufferShader->GetPixelShader();
        m_ShaderParameters->VertexShader = m_GBufferShader->GetVertexShader();

    }





	void GBufferRenderer::RenderSceneWithGBufferAndProbes(
		std::vector<Object*> staticObjects,
		std::vector<Object*> dynamicObjects,
		float timeInSecond,
		std::vector<Object*> backGroundObjs,
		Object* gBufferDebugQuad, 
		LightSources* lightSources,
		std::vector<Object*> diffuseProbes, std::vector<Object*> reflectionProbes,
		TextureHandle specularBrdfLUTTexture, Object* skyBox,
		MapManager* mapManager,
		TextureHandle depthTexture,
		bool enableSSR)
	{
//
//		GE_ASSERT(m_IsInitialized, "GBufferRenderer have not been initialized! ");
//#ifdef TIME_DEBUG
//		TimeCounter::Start();
//#endif
//		// Only the geometry pass updates the depth buffer
//		glDepthMask(GL_TRUE);
//
//		glEnable(GL_DEPTH_TEST);
//
//		glDisable(GL_BLEND);
//
//		/************************1. Geometry Pass: render scene's geometry/color data into gbuffer *****************/
//		/***********************************************************************************************************/
//		m_GBuffer->Bind();
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		
//
//
//
//		/************************1.5 probes : find the k near probes for each objects *****************/
//		/***********************************************************************************************************/
//		/*  �� DrawObjectͬʱ������ȫ�ֹ��գ� */
//		
//		for (Object* obj : staticObjects)
//		{
//
//			glm::vec3 pos = obj->GetComponent<Transform>()->GetPosition();
//			std::vector<unsigned int> kDiffuseProbesIdx;
//			//��ʼ��ʱ��obj�ƶ��ˣ�probe�ƶ��˶�Ҫ������kNearProbes!
//			if (!obj->GetComponent<MeshRenderer>()->GetIsDiffuseProbeCacheSet() ||
//				obj->GetComponent<Transform>()->GetPosition() != obj->GetComponent<Transform>()->GetLastPosition()||
//				mapManager->m_ProbeGridPosChanged)
//			{
//				kDiffuseProbesIdx = FindKnearAreaProbes(Math::ToFloat3(pos), diffuseProbes, m_K, mapManager);
//				obj->GetComponent<MeshRenderer>()->SetDiffuseProbeChache(kDiffuseProbesIdx);
//				obj->GetComponent<MeshRenderer>()->SetIsDiffuseProbeCacheSet(true);
//				mapManager->m_ProbeGridPosChanged = false;
//			}
//			else {
//				kDiffuseProbesIdx = obj->GetComponent<MeshRenderer>()->GetDiffuseProbeChache();
//			}
//
//			unsigned int k = kDiffuseProbesIdx.size();//kDiffuseProbes�ĸ����п���С��m_K
//			std::vector<float> distances;
//			float distancesSum = 0.0;
//			for (auto probeIdx : kDiffuseProbesIdx)
//			{
//				distances.push_back(glm::length(diffuseProbes[probeIdx]->GetComponent<Transform>()->GetPosition() - pos));
//				distancesSum += glm::length(diffuseProbes[probeIdx]->GetComponent<Transform>()->GetPosition() - pos);
//			}
//
//
//			m_GBufferShader->Bind();
//			m_GBufferShader->SetUniform1i("u_Kprobes", k);
//
//			m_GBufferShader->SetUniform1i("u_BrdfLUTMap", 0);
//			glActiveTexture(GL_TEXTURE0);
//			specularBrdfLUTTexture->Bind();
//
//			
//
//
//
//			for (int i = 0; i < k; i++)
//			{
//				m_GBufferShader->SetUniform1f("u_ProbeWeight[" + std::to_string(i) + "]", (float)distances[i] / distancesSum);
//
//				//m_IBLShader->SetUniform1i("u_IrradianceMap[" + std::to_string(i) + "]", textureK);
//				//m_IBLShader->SetUniform1i("u_Image", textureK);
//				for (int sh = 0; sh < 9; sh++)
//				{
//					int index = sh + 9 * i;
//					m_GBufferShader->SetUniformVec3f("u_SHCoeffs[" + std::to_string(index) + "]", math::float3(
//						diffuseProbes[kDiffuseProbesIdx[i]]->GetComponent<LightProbe>()->GetCoeffis()[sh][0],
//						diffuseProbes[kDiffuseProbesIdx[i]]->GetComponent<LightProbe>()->GetCoeffis()[sh][1],
//						diffuseProbes[kDiffuseProbesIdx[i]]->GetComponent<LightProbe>()->GetCoeffis()[sh][2])
//					);
//				}
//
//				
//
//
//			}
//
//			
//			Object* reflectProbe = obj->GetChildByFrontName("LightProbe_ks");
//			if (reflectProbe != nullptr) {
//				m_GBufferShader->SetUniform1i("u_PrefilterMap", 1);
//				glActiveTexture(GL_TEXTURE1);
//				reflectProbe->GetComponent<LightProbe>()->GetSpecularPrefilterCubeMap()->Bind();
//			}
//		
//			m_GBufferShader->Bind();
//			DrawObject(obj, m_GBufferShader);
//		}


		

		/*********************************������Ⱦ light objects ******************************************************** /
		/************ 2.5. Copy content of geometry's depth buffer to default framebuffer's depth buffer****************/
//#ifdef TIME_DEBUG
//		TimeCounter::Start();
//#endif
//		glDepthMask(GL_TRUE);
//		glEnable(GL_DEPTH_TEST);
//
//		m_GBuffer->Bind();
//		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
//		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
//		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
//		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
//		glBlitFramebuffer(0, 0, m_TextureWidth, m_TexxtureHeight, 0, 0, m_TextureWidth, m_TexxtureHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//
//		glDisable(GL_BLEND);
//
//		if (skyBox != nullptr) {
//			glDepthFunc(GL_LEQUAL);
//			m_SkyboxRenderer->Render(skyBox, timeInSecond);
//
//			//m_SkyboxRenderer->Render(skyBox);
//			//DrawObject(skyBox);
//			glDepthFunc(GL_LESS);
//		}
//
//		//DrawObjects(backGroundObjs);
//
//		DrawLightSources(lightSources);

		/* Debug ... ������Χ�򿴿� */
		//glEnable(GL_BLEND);
		//glDisable(GL_CULL_FACE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//std::vector<math::float3> color = {
		//	{1.0,0.0,0.0},
		//	{1.0,1.0,0.0},
		//	{1.0,0.0,1.0},
		//	{0.5,0.5,1.0}
		//};
		////int k = 0;
		//for (Object* pointLight : lightSources->GetPointLights())
		//{
		//	float radius = CalculateSphereRadius(pointLight);
		//	m_SurroundSphere->GetComponent<Transform>()->SetScale({ radius,radius,radius });
		//	m_SurroundSphere->GetComponent<Transform>()->SetPosition(pointLight->GetComponent<Transform>()->GetPosition());

		//	m_SphereDeBugShader->Bind();
		//	m_SphereDeBugShader->SetUniformVec3f("randomColor", pointLight->GetComponent<PointLight>()->GetLightProps().diffuse);
		//	//GE_ASSERT(k <= 4, "k>4!");
		//	DrawObject(m_SurroundSphere, m_SphereDeBugShader);


		//}


		//frameBuffer->CleanUp();
#ifdef TIME_DEBUG
		TimeCounter::End("Forward Rendering");
#endif


	}


}
