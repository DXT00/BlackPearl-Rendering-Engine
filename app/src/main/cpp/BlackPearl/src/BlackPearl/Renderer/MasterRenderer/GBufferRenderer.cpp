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
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
#endif
#include "RHI/RHIGlobals.h"
#include "Core/AssetManager.h"
#include "Renderer/RenderGraph/RenderGraph.h"

namespace BlackPearl {


	GBufferRenderer::GBufferRenderer(IDevice* device)
    : BasicRenderer(device)
	{
		
	}

    void GBufferRenderer::Init()
    {
        std::vector<std::string> extends;
        std::vector<std::string> macros;
        //glSurfaceView.setEGLContextClientVersion(3);
#ifdef GE_PLATFORM_ANDROID
        std::string ExtensionsString;
        const char* GlGetStringOutput = (const char*)glGetString(GL_EXTENSIONS);
        if (GlGetStringOutput)
        {
            ExtensionsString += std::string(GlGetStringOutput);
            ExtensionsString += (" ");
        }
//        bool bSupportsShaderMRTFramebufferFetch = ExtensionsString.find(("GL_EXT_shader_framebuffer_fetch"))  != std::string::npos||
//                                             ExtensionsString.find(("GL_NV_shader_framebuffer_fetch")) != std::string::npos;
//
//        bool bSupportsPixelLocalStorage = ExtensionsString.find(("GL_EXT_shader_pixel_local_storage")) != std::string::npos;
//        bool  bSupportsShaderDepthStencilFetch = ExtensionsString.find(
//                ("GL_ARM_shader_framebuffer_fetch_depth_stencil")) != std::string::npos;
        AssetManager::StoreGLSLShader( ExtensionsString, "ExtensionsString.glsl");


        EOpenGLCurrentContext ContextType = (EOpenGLCurrentContext) AndroidEGL::GetInstance()->GetCurrentContextType();

        if (ContextType == CONTEXT_Rendering) {
                GE_CORE_INFO("Current context: CONTEXT_Rendering");
        } else if (ContextType == CONTEXT_Shared) {
            GE_CORE_INFO("Current context: CONTEXT_Shared");

        } else {
            GE_CORE_INFO("Current context: Invalid context");

        }
        //= FOpenGL::SupportsPixelLocalStorage();  = bSupportsPixelLocalStorage
       if (RenderGraph::SupportPLS())
        {
            extends.push_back("#extension GL_EXT_shader_pixel_local_storage : require");
            extends.push_back("#extension GL_ARM_shader_framebuffer_fetch_depth_stencil : require");
            macros.push_back("#define USE_GLES_PLS 1");

        }
#endif

        m_DrawStrategy = DBG_NEW InstancedOpaqueDrawStrategy();
        ShaderDesc desc = ShaderDesc(ShaderType::All);
        desc.debugName = "GbufferShader";
        //todo:: 每个材质的 gpass 有可能不一样, USE_GLES_PLS 的 extends, macros，统一需要再创建材质的Material Shader时内部加， m_GBufferShader 没用到，后面可以删除
        m_GBufferShader = DBG_NEW MaterialShader("assets/shaders/glsl/gBuffer/gBuffer_pass.glsl",&extends, &macros);


        m_ShaderParameters[ShaderType::Pixel].bindingLayouts.push_back(m_ViewBindinglayout);
        m_ShaderParameters[ShaderType::Pixel].bindingSets.push_back(m_ViewBindingset);
    //    m_ShaderParameters->PixelShader = m_GBufferShader->GetPixelShader();
    //    m_ShaderParameters->VertexShader = m_GBufferShader->GetVertexShader();

    }



	void GBufferRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
	{
        SCOPE_TIME_COUNTER(GBuffer);

       // commandList->beginMarker("GBufferPass");
        GE_ERROR_JUDGE();


        SceneData* view = Renderer::GetSceneData();
        GE_ERROR_JUDGE();

        SceneData* preView = Renderer::GetPreSceneData();
        GE_ERROR_JUDGE();

        SetupView(commandList, view, preView);

        m_DrawStrategy->PrepareForView(scene, *view);
        GE_ERROR_JUDGE();

        RenderPassTemplate(commandList, targetFramebuffer, view, m_DrawStrategy, m_ShaderParameters);


     //   commandList->endMarker();
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
