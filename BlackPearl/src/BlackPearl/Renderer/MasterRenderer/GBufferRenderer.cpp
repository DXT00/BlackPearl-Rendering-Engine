#include "pch.h"
#include "GBufferRenderer.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "ShadowMapPointLightRenderer.h"

#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"
#include "glm/gtc/random.hpp"
#include <glm\ext\scalar_common.hpp>

namespace BlackPearl {
	float GBufferRenderer::s_GICoeffs = 0.5f;
	bool GBufferRenderer::s_HDR = true;
	float GBufferRenderer::s_AttenuationItensity = 50.0f;
	GBufferRenderer::GBufferRenderer()
	{
		m_GBuffer.reset(DBG_NEW GBuffer(m_TextureWidth, m_TexxtureHeight));

		m_HDRPostProcessTexture.reset(DBG_NEW Texture(Texture::Type::None, m_TextureWidth, m_TexxtureHeight, false, GL_LINEAR, GL_LINEAR, GL_RGBA16F, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));


		m_GBufferShader.reset(DBG_NEW Shader("assets/shaders/gBufferProbe/gBuffer.glsl"));
		m_AmbientGIPassShader.reset(DBG_NEW Shader("assets/shaders/gBufferProbe/gBufferAmbientGIPass.glsl"));
		m_PointLightPassShader.reset(DBG_NEW Shader("assets/shaders/gBufferProbe/gBufferPontLightPass.glsl"));
		m_SphereDeBugShader.reset(DBG_NEW Shader("assets/shaders/gBufferProbe/SurroundSphereDebug.glsl"));
		m_FinalScreenShader.reset(DBG_NEW Shader("assets/shaders/gBufferProbe/FinalScreenQuad.glsl"));

		m_AnimatedModelRenderer = new AnimatedModelRenderer();
		std::shared_ptr<Shader> gBufferAnimatedShader(DBG_NEW Shader("assets/shaders/animatedModel/animatedGBufferModel.glsl"));
		m_AnimatedModelRenderer->SetShader(gBufferAnimatedShader);
	}

	void GBufferRenderer::Init(Object* screenQuad, Object* surroundSphere, Object* GIQuad)
	{
		m_FinalScreenQuad = screenQuad;
		m_GIQuad = GIQuad;
		m_SurroundSphere = surroundSphere;
		m_IsInitialized = true;
	}

	void GBufferRenderer::Render(std::vector<Object*> objects, Object* gBufferDebugQuad, LightSources* lightSources)
	{
		GE_ASSERT(m_IsInitialized, "GBufferRenderer have not been initialized! ");


		// Only the geometry pass updates the depth buffer
		glDepthMask(GL_TRUE);

		glEnable(GL_DEPTH_TEST);

		glDisable(GL_BLEND);

		/************************1. Geometry Pass: render scene's geometry/color data into gbuffer *****************/
		/***********************************************************************************************************/
		m_GBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_GBufferShader->Bind();
		DrawObjects(objects, m_GBufferShader);


		//DrawGBuffer(gBufferDebugQuad);

		/************************ 2. Lighting Pass: render lights according to gBuffer.********************************************/
		/*************************************************************************************************************************/


		/* When we get here the depth buffer is already populated and the stencil pass
		 depends on it, but it does not write to it.
		 */

		 /* following use default frameBuffer! 禁用深度缓冲-->使得 sphere 投影到二维平面，只有sphere投影里的fragment才绘制 */
		glDepthMask(GL_FALSE);
		/* disable DEPTH_TEST 后所有的 glClear(GL_DEPTH_BUFFER_BIT) 失效 */
		glDisable(GL_DEPTH_TEST);
		/* blending multiple sphere fragment */
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		m_GBuffer->UnBind();
		glClear(GL_COLOR_BUFFER_BIT);



		/* PointLight pass */
		for (Object* pointLight : lightSources->GetPointLights())
		{
			float radius = CalculateSphereRadius(pointLight);
			m_SurroundSphere->GetComponent<Transform>()->SetScale({ radius,radius,radius });
			m_SurroundSphere->GetComponent<Transform>()->SetPosition(pointLight->GetComponent<Transform>()->GetPosition());


			m_PointLightPassShader->Bind();
			m_PointLightPassShader->SetUniform1i("gPosition", 0);
			m_PointLightPassShader->SetUniform1i("gNormal", 1);
			m_PointLightPassShader->SetUniform1i("gDiffuse_Roughness", 2);
			m_PointLightPassShader->SetUniform1i("gSpecular_Mentallic", 3);
			m_PointLightPassShader->SetUniform1i("gAmbientGI_AO", 4);

			m_PointLightPassShader->SetUniformVec2f("gScreenSize", glm::vec2(m_TextureWidth, m_TexxtureHeight));

			glActiveTexture(GL_TEXTURE0);
			m_GBuffer->GetPositionTexture()->Bind();
			glActiveTexture(GL_TEXTURE1);
			m_GBuffer->GetNormalTexture()->Bind();
			glActiveTexture(GL_TEXTURE2);
			m_GBuffer->GetDiffuseRoughnessTexture()->Bind();
			glActiveTexture(GL_TEXTURE3);
			m_GBuffer->GetSpecularMentallicTexture()->Bind();
			glActiveTexture(GL_TEXTURE4);
			m_GBuffer->GetAmbientGIAOTexture()->Bind();

			m_PointLightPassShader->SetUniformVec3f("u_PointLight.ambient", pointLight->GetComponent<PointLight>()->GetLightProps().ambient);
			m_PointLightPassShader->SetUniformVec3f("u_PointLight.diffuse", pointLight->GetComponent<PointLight>()->GetLightProps().diffuse);
			m_PointLightPassShader->SetUniformVec3f("u_PointLight.specular", pointLight->GetComponent<PointLight>()->GetLightProps().specular);
			m_PointLightPassShader->SetUniformVec3f("u_PointLight.position", pointLight->GetComponent<Transform>()->GetPosition());

			m_PointLightPassShader->SetUniform1f("u_PointLight.constant", pointLight->GetComponent<PointLight>()->GetAttenuation().constant);
			m_PointLightPassShader->SetUniform1f("u_PointLight.linear", pointLight->GetComponent<PointLight>()->GetAttenuation().linear);
			m_PointLightPassShader->SetUniform1f("u_PointLight.quadratic", pointLight->GetComponent<PointLight>()->GetAttenuation().quadratic);

			m_SurroundSphere->GetComponent<MeshRenderer>()->SetShaders(m_PointLightPassShader);
			DrawObject(m_SurroundSphere, m_PointLightPassShader);


		}



		/* DirectionLight pass */
		//	DrawObject(m_ScreenQuad, m_DirectionLightPassShader);



		/*********************************正向渲染 light objects ******************************************************** /
		/************ 2.5. Copy content of geometry's depth buffer to default framebuffer's depth buffer****************/

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		m_GBuffer->Bind();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		glBlitFramebuffer(0, 0, m_TextureWidth, m_TexxtureHeight, 0, 0, m_TextureWidth, m_TexxtureHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		DrawLightSources(lightSources);

		/* Debug ... 画出包围球看看 */
		/*glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

		//std::vector<glm::vec3> color = {
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




	}

	void GBufferRenderer::DrawGBuffer(Object* gBufferDebugQuad)
	{

		/*glViewport(0, 270, 480, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetPositionTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(0, 0, 480, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetAmbientGIAOTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(480, 0, 480, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetDiffuseRoughnessTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(480, 270, 480, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetSpecularMentallicTexture());
		DrawObject(gBufferDebugQuad);*/

		glViewport(0, 0, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetPositionTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(0, 270, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetNormalTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(320, 0, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetDiffuseRoughnessTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(320, 270, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetSpecularMentallicTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(640, 0, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetAmbientGIAOTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(640, 270, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetNormalMapTexture());
		DrawObject(gBufferDebugQuad);

	}

	float GBufferRenderer::CalculateSphereRadius(Object* pointLight)
	{
		auto lightProps = pointLight->GetComponent<PointLight>()->GetLightProps();
		auto attenuation = pointLight->GetComponent<PointLight>()->GetAttenuation();
		glm::vec3 lightDiffuse = lightProps.diffuse;
		float constant = attenuation.constant;
		float linear = attenuation.linear;
		float quadratic = attenuation.quadratic;



		float maxChannel = std::max(std::max(lightDiffuse.x, lightDiffuse.y), lightDiffuse.z);

		float distance = (-linear + sqrtf(linear * linear - 4 * quadratic * (constant - 256.0f / s_AttenuationItensity * maxChannel))) / (2 * quadratic);

		return distance;
	}

	void GBufferRenderer::RenderSceneWithGBufferAndProbes(std::vector<Object*> staticObjects, std::vector<Object*> dynamicObjects, float timeInSecond, std::vector<Object*> backGroundObjs, Object* gBufferDebugQuad, LightSources* lightSources,
		std::vector<LightProbe*> diffuseProbes, std::vector<LightProbe*> reflectionProbes, 
		std::shared_ptr<Texture> specularBrdfLUTTexture, Object* skyBox,
		MapManager* mapManager)
	{

		GE_ASSERT(m_IsInitialized, "GBufferRenderer have not been initialized! ");


		// Only the geometry pass updates the depth buffer
		glDepthMask(GL_TRUE);

		glEnable(GL_DEPTH_TEST);

		glDisable(GL_BLEND);

		/************************1. Geometry Pass: render scene's geometry/color data into gbuffer *****************/
		/***********************************************************************************************************/
		m_GBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




		/************************1.5 probes : find the k near probes for each objects *****************/
		/***********************************************************************************************************/
		/*  在 DrawObject同时，计算全局光照！ */
		for (Object* obj : staticObjects)
		{

			glm::vec3 pos = obj->GetComponent<Transform>()->GetPosition();

			std::vector<LightProbe*> kDiffuseProbes = FindKnearAreaProbes(pos, diffuseProbes,m_K,mapManager);

			unsigned int k = kDiffuseProbes.size();//kDiffuseProbes的个数有可能小于m_K
			std::vector<float> distances;
			float distancesSum = 0.0;
			for (auto probe : kDiffuseProbes)
			{
				distances.push_back(glm::length(probe->GetPosition() - pos));
				distancesSum += glm::length(probe->GetPosition() - pos);
			}


			m_GBufferShader->Bind();
			m_GBufferShader->SetUniform1i("u_Kprobes", k);

			m_GBufferShader->SetUniform1i("u_BrdfLUTMap", 0);
			glActiveTexture(GL_TEXTURE0);
			specularBrdfLUTTexture->Bind();

			



			for (int i = 0; i < k; i++)
			{
				m_GBufferShader->SetUniform1f("u_ProbeWeight[" + std::to_string(i) + "]", (float)distances[i] / distancesSum);

				//m_IBLShader->SetUniform1i("u_IrradianceMap[" + std::to_string(i) + "]", textureK);
				//m_IBLShader->SetUniform1i("u_Image", textureK);
				for (int sh = 0; sh < 9; sh++)
				{
					int index = sh + 9 * i;
					m_GBufferShader->SetUniformVec3f("u_SHCoeffs[" + std::to_string(index) + "]", glm::vec3(kDiffuseProbes[i]->GetCoeffis()[sh][0], kDiffuseProbes[i]->GetCoeffis()[sh][1], kDiffuseProbes[i]->GetCoeffis()[sh][2]));
				}

				//glActiveTexture(GL_TEXTURE0 + textureK);
				//kProbes[i]->GetDiffuseIrradianceCubeMap()->Bind();
				//kProbes[i]->GetSHImage()->Bind(textureK);
				//textureK++;


			}

			if (reflectionProbes.size() > 0) {
				std::vector<LightProbe*> kReflectionProbes = FindKnearProbes(pos, reflectionProbes, 1);
				/*只需要最近的那个 probe 的 SpecularMap!*/
				m_GBufferShader->SetUniform1i("u_PrefilterMap", 1);
				glActiveTexture(GL_TEXTURE1);
				auto specularMap = kReflectionProbes[0]->GetSpecularPrefilterCubeMap();
				kReflectionProbes[0]->GetSpecularPrefilterCubeMap()->Bind();
			}
			

		
			m_GBufferShader->Bind();
			DrawObject(obj, m_GBufferShader);
		}


		for (Object* obj : dynamicObjects)
		{

			std::shared_ptr<Shader> animatedShader = m_AnimatedModelRenderer->GetShader();

			animatedShader->Bind();

			glm::vec3 pos = obj->GetComponent<Transform>()->GetPosition();

			std::vector<LightProbe*> kDiffuseProbes = FindKnearProbes(pos, diffuseProbes,m_K);

			unsigned int k = m_K;
			std::vector<float> distances;
			float distancesSum = 0.0;
			for (auto probe : kDiffuseProbes)
			{
				distances.push_back(glm::length(probe->GetPosition() - pos));
				distancesSum += glm::length(probe->GetPosition() - pos);
			}


			//m_GBufferShader->Bind();
			animatedShader->SetUniform1i("u_Kprobes", k);

			animatedShader->SetUniform1i("u_BrdfLUTMap", 0);
			glActiveTexture(GL_TEXTURE0);
			specularBrdfLUTTexture->Bind();

		

			for (int i = 0; i < k; i++)
			{
				animatedShader->SetUniform1f("u_ProbeWeight[" + std::to_string(i) + "]", (float)distances[i] / distancesSum);

				//m_IBLShader->SetUniform1i("u_IrradianceMap[" + std::to_string(i) + "]", textureK);
				//m_IBLShader->SetUniform1i("u_Image", textureK);
				for (int sh = 0; sh < 9; sh++)
				{
					int index = sh + 9 * i;
					animatedShader->SetUniformVec3f("u_SHCoeffs[" + std::to_string(index) + "]", glm::vec3(kDiffuseProbes[i]->GetCoeffis()[sh][0], kDiffuseProbes[i]->GetCoeffis()[sh][1], kDiffuseProbes[i]->GetCoeffis()[sh][2]));
				}

				//glActiveTexture(GL_TEXTURE0 + textureK);
				//kProbes[i]->GetDiffuseIrradianceCubeMap()->Bind();
				//kProbes[i]->GetSHImage()->Bind(textureK);
				//textureK++;


			}

			/*只需要最近的那个 probe 的 SpecularMap!*/
			if (reflectionProbes.size() > 0) {
				std::vector<LightProbe*> kReflectionProbes = FindKnearProbes(pos, reflectionProbes, 1);

				animatedShader->SetUniform1i("u_PrefilterMap", 1);
				glActiveTexture(GL_TEXTURE1);
				kReflectionProbes[0]->GetSpecularPrefilterCubeMap()->Bind();
			}
			


		/*	if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject())
				animatedShader->SetUniform1i("u_IsPBRObjects", 1);
			else
				animatedShader->SetUniform1i("u_IsPBRObjects", 0);*/

			m_AnimatedModelRenderer->Render(obj, timeInSecond, animatedShader);
			/*	obj->GetComponent<MeshRenderer>()->SetShaders(m_GBufferShader);
				DrawObject(obj, m_GBufferShader);*/
		}




		//m_GBuffer->UnBind();

		//DrawGBuffer(gBufferDebugQuad);

		/************************ 2. Lighting Pass: render lights according to gBuffer.********************************************/
		/*************************************************************************************************************************/





		/* When we get here the depth buffer is already populated and the stencil pass
		 depends on it, but it does not write to it.
		 */

		 /* following use default frameBuffer! 禁用深度缓冲-->使得 sphere 投影到二维平面，只有sphere投影里的fragment才绘制 */
		glDepthMask(GL_FALSE);
		/* disable DEPTH_TEST 后所有的 glClear(GL_DEPTH_BUFFER_BIT) 失效 */
		glDisable(GL_DEPTH_TEST);
		/* blending multiple sphere fragment */
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		m_GBuffer->UnBind();


		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		frameBuffer->Bind();
		frameBuffer->AttachRenderBuffer(m_TextureWidth, m_TexxtureHeight);
		frameBuffer->AttachColorTexture(m_HDRPostProcessTexture, 0);
		frameBuffer->BindRenderBuffer();
		glViewport(0, 0, m_TextureWidth, m_TexxtureHeight);
		glClear(GL_COLOR_BUFFER_BIT);


		/* AmbientGI pass */
		m_AmbientGIPassShader->Bind();

		m_AmbientGIPassShader->SetUniform1i("gAmbientGI_AO", 2);
		glActiveTexture(GL_TEXTURE2);
		m_GBuffer->GetAmbientGIAOTexture()->Bind();

		m_AmbientGIPassShader->SetUniformVec2f("gScreenSize", glm::vec2(m_TextureWidth, m_TexxtureHeight));
		m_AmbientGIPassShader->SetUniform1f("u_Settings.GICoeffs", s_GICoeffs);

		DrawObject(m_GIQuad, m_AmbientGIPassShader);



		/* PointLight pass */
		for (Object* pointLight : lightSources->GetPointLights())
		{
			float radius = CalculateSphereRadius(pointLight);
			m_SurroundSphere->GetComponent<Transform>()->SetScale({ radius,radius,radius });
			m_SurroundSphere->GetComponent<Transform>()->SetPosition(pointLight->GetComponent<Transform>()->GetPosition());


			m_PointLightPassShader->Bind();
			m_PointLightPassShader->SetUniform1i("gPosition", 0);
			m_PointLightPassShader->SetUniform1i("gNormal", 1);
			m_PointLightPassShader->SetUniform1i("gDiffuse_Roughness", 2);
			m_PointLightPassShader->SetUniform1i("gSpecular_Mentallic", 3);
			m_PointLightPassShader->SetUniform1i("gAmbientGI_AO", 4);
			m_PointLightPassShader->SetUniform1i("gNormalMap", 5);

			m_PointLightPassShader->SetUniformVec2f("gScreenSize", glm::vec2(m_TextureWidth, m_TexxtureHeight));

			glActiveTexture(GL_TEXTURE0);
			m_GBuffer->GetPositionTexture()->Bind();
			glActiveTexture(GL_TEXTURE1);
			m_GBuffer->GetNormalTexture()->Bind();
			glActiveTexture(GL_TEXTURE2);
			m_GBuffer->GetDiffuseRoughnessTexture()->Bind();
			glActiveTexture(GL_TEXTURE3);
			m_GBuffer->GetSpecularMentallicTexture()->Bind();
			glActiveTexture(GL_TEXTURE4);
			m_GBuffer->GetAmbientGIAOTexture()->Bind();
			glActiveTexture(GL_TEXTURE5);
			m_GBuffer->GetNormalMapTexture()->Bind();

			m_PointLightPassShader->SetUniformVec3f("u_PointLight.ambient", pointLight->GetComponent<PointLight>()->GetLightProps().ambient);
			m_PointLightPassShader->SetUniformVec3f("u_PointLight.diffuse", pointLight->GetComponent<PointLight>()->GetLightProps().diffuse);
			m_PointLightPassShader->SetUniformVec3f("u_PointLight.specular", pointLight->GetComponent<PointLight>()->GetLightProps().specular);
			m_PointLightPassShader->SetUniformVec3f("u_PointLight.position", pointLight->GetComponent<Transform>()->GetPosition());

			m_PointLightPassShader->SetUniform1f("u_PointLight.constant", pointLight->GetComponent<PointLight>()->GetAttenuation().constant);
			m_PointLightPassShader->SetUniform1f("u_PointLight.linear", pointLight->GetComponent<PointLight>()->GetAttenuation().linear);
			m_PointLightPassShader->SetUniform1f("u_PointLight.quadratic", pointLight->GetComponent<PointLight>()->GetAttenuation().quadratic);
			m_PointLightPassShader->SetUniform1f("u_PointLight.intensity", pointLight->GetComponent<PointLight>()->GetLightProps().intensity);

			m_PointLightPassShader->SetUniform1f("u_FarPlane", ShadowMapPointLightRenderer::s_FarPlane);
			m_PointLightPassShader->SetUniform1i("u_ShadowMap", 6);
			glActiveTexture(GL_TEXTURE6);
			pointLight->GetComponent<PointLight>()->GetShadowMap()->Bind();

			m_SurroundSphere->GetComponent<MeshRenderer>()->SetShaders(m_PointLightPassShader);
			DrawObject(m_SurroundSphere, m_PointLightPassShader);


		}

		frameBuffer->UnBind();

		glViewport(0, 0, m_TextureWidth, m_TexxtureHeight);

		glClear(GL_COLOR_BUFFER_BIT);
		m_FinalScreenShader->Bind();
		m_FinalScreenShader->SetUniform1i("u_FinalScreenTexture", 0);
		m_FinalScreenShader->SetUniform1f("u_Settings.hdr", s_HDR);
		glActiveTexture(GL_TEXTURE0);
		m_HDRPostProcessTexture->Bind();
		DrawObject(m_FinalScreenQuad, m_FinalScreenShader);

		/* DirectionLight pass */
		//	DrawObject(m_ScreenQuad, m_DirectionLightPassShader);



		/*********************************正向渲染 light objects ******************************************************** /
		/************ 2.5. Copy content of geometry's depth buffer to default framebuffer's depth buffer****************/

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		m_GBuffer->Bind();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		glBlitFramebuffer(0, 0, m_TextureWidth, m_TexxtureHeight, 0, 0, m_TextureWidth, m_TexxtureHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glDisable(GL_BLEND);

		if (skyBox != nullptr) {
			glDepthFunc(GL_LEQUAL);

			DrawObject(skyBox);
			glDepthFunc(GL_LESS);
		}

		//DrawObjects(backGroundObjs);

		DrawLightSources(lightSources);

		/* Debug ... 画出包围球看看 */
		//glEnable(GL_BLEND);
		//glDisable(GL_CULL_FACE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//std::vector<glm::vec3> color = {
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


		frameBuffer->CleanUp();



	}

	std::vector<LightProbe*> GBufferRenderer::FindKnearProbes(glm::vec3 objPos, std::vector<LightProbe*> probes,unsigned int k)
	{
		glm::vec3 pos = objPos;
		std::vector<LightProbe*> kProbes;
		std::sort(probes.begin(), probes.end(), [=](LightProbe* pa, LightProbe* pb)
		{return glm::length(pa->GetPosition() - pos) < glm::length(pb->GetPosition() - pos); });

		GE_ASSERT(k <= (unsigned int)probes.size(), "m_K larger than probes' number!");

		for (int i = 0; i < k; i++)
		{
			kProbes.push_back(probes[i]);
		}


		return kProbes;
	}
	std::vector<LightProbe*> GBufferRenderer::FindKnearAreaProbes(glm::vec3 objPos, std::vector<LightProbe*> probes, unsigned int k,MapManager* mapManager)
	{
		glm::vec3 pos = objPos;
		Area currentArea = mapManager->GetArea( mapManager->CalculateAreaId(pos));
		std::set<unsigned int> nearByArea = mapManager->FindNearByArea(pos);
	
		std::vector<LightProbe*> nearByProbes;
		std::set<unsigned int>::iterator it;
		for (it = nearByArea.begin(); it != nearByArea.end(); it++) {
			Area area = mapManager->GetArea(*it);
			for (auto probeIdx:area.GetProbesId())
			{
				nearByProbes.push_back(probes[probeIdx]);
			}
		}
		for (auto probeIdx : currentArea.GetProbesId())
		{
			nearByProbes.push_back(probes[probeIdx]);
		}
		std::vector<LightProbe*> kProbes;
		std::sort(nearByProbes.begin(), nearByProbes.end(), [=](LightProbe* pa, LightProbe* pb)
		{return glm::length(pa->GetPosition() - pos) < glm::length(pb->GetPosition() - pos); });

		if(k >= (unsigned int)nearByProbes.size())
			GE_CORE_WARN( "m_K {0} larger than nearby probes' number!", k);

		int kMin = glm::min(k, nearByProbes.size());
		GE_ASSERT(k >0, "no probe found near this object!");

		for (int i = 0; i < kMin; i++)
		{
			kProbes.push_back(nearByProbes[i]);
		}


		return kProbes;
	}
}
