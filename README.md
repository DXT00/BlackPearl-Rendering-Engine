###  LightProbe + GBuffer + AnimatedModel deferred rendering

#### 1.结合 lightProbes获取环境光-GI ,渲染 objects属性到 gBuffer

#### 2.从gBuffer获取数据 -》渲染 GI pass,pointLight pass 到frameBuffer

#### 3.frameBuffer 做后期处理 --》包括HDR ,tone Mapping

#### 4.添加Animated Model （dynamic objects） 使用AnimatedModelRenderer 渲染

```

void GBufferRenderer::RenderSceneWithGBufferAndProbes(std::vector<Object*> staticObjects, std::vector<Object*> dynamicObjects, float timeInSecond, std::vector<Object*> backGroundObjs, Object* gBufferDebugQuad, LightSources* lightSources,
		std::vector<LightProbe*> probes, std::shared_ptr<Texture> specularBrdfLUTTexture,Object* skyBox)
	{

		GE_ASSERT(m_IsInitialized, "GBufferRenderer have not been initialized! ");


		// Only the geometry pass updates the depth buffer
		glDepthMask(GL_TRUE);

		glEnable(GL_DEPTH_TEST);

		glDisable(GL_BLEND);

		/************************1. Geometry Pass: render scene's geometry/color data into gbuffer *****************/
		/***********************************************************************************************************/
		m_GBffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		


		/************************1.5 probes : find the k near probes for each objects *****************/
		/***********************************************************************************************************/
		/*  在 DrawObject同时，计算全局光照！ */
		for (Object* obj:staticObjects)
		{

			glm::vec3 pos = obj->GetComponent<Transform>()->GetPosition();

			std::vector<LightProbe*> kProbes = FindKnearProbes(pos, probes);

			unsigned int k = m_K;
			std::vector<float> distances;
			float distancesSum = 0.0;
			for (auto probe : kProbes)
			{
				distances.push_back(glm::length(probe->GetPosition() - pos));
				distancesSum += glm::length(probe->GetPosition() - pos);
			}


			m_GBufferShader->Bind();
			m_GBufferShader->SetUniform1i("u_Kprobes", k);
			
			m_GBufferShader->SetUniform1i("u_BrdfLUTMap", 0);
			glActiveTexture(GL_TEXTURE0);
			specularBrdfLUTTexture->Bind();
			
			/*只需要最近的那个 probe 的 SpecularMap!*/
			m_GBufferShader->SetUniform1i("u_PrefilterMap", 1);
			glActiveTexture(GL_TEXTURE1);
			kProbes[0]->GetSpecularPrefilterCubeMap()->Bind();
			


			for (int i = 0; i < k; i++)
			{
				m_GBufferShader->SetUniform1f("u_ProbeWeight[" + std::to_string(i) + "]", (float)distances[i] / distancesSum);

				//m_IBLShader->SetUniform1i("u_IrradianceMap[" + std::to_string(i) + "]", textureK);
				//m_IBLShader->SetUniform1i("u_Image", textureK);
				for (int sh = 0; sh < 9; sh++)
				{
					int index = sh + 9 * i;
					m_GBufferShader->SetUniformVec3f("u_SHCoeffs[" + std::to_string(index) + "]", glm::vec3(kProbes[i]->GetCoeffis()[sh][0], kProbes[i]->GetCoeffis()[sh][1], kProbes[i]->GetCoeffis()[sh][2]));
				}

				//glActiveTexture(GL_TEXTURE0 + textureK);
				//kProbes[i]->GetDiffuseIrradianceCubeMap()->Bind();
				//kProbes[i]->GetSHImage()->Bind(textureK);
				//textureK++;
		

			}
			if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject())
				m_GBufferShader->SetUniform1f("u_IsPBRObjects", 1.0f);
			else
				m_GBufferShader->SetUniform1f("u_IsPBRObjects", 0.0f);


			obj->GetComponent<MeshRenderer>()->SetShaders(m_GBufferShader);
			DrawObject(obj, m_GBufferShader);
		}

		
		for (Object* obj : dynamicObjects)
		{

			std::shared_ptr<Shader> animatedShader = m_AnimatedModelRenderer->GetShader();

			animatedShader->Bind();

			glm::vec3 pos = obj->GetComponent<Transform>()->GetPosition();

			std::vector<LightProbe*> kProbes = FindKnearProbes(pos, probes);

			unsigned int k = m_K;
			std::vector<float> distances;
			float distancesSum = 0.0;
			for (auto probe : kProbes)
			{
				distances.push_back(glm::length(probe->GetPosition() - pos));
				distancesSum += glm::length(probe->GetPosition() - pos);
			}


			//m_GBufferShader->Bind();
			animatedShader->SetUniform1i("u_Kprobes", k);

			animatedShader->SetUniform1i("u_BrdfLUTMap", 0);
			glActiveTexture(GL_TEXTURE0);
			specularBrdfLUTTexture->Bind();

			/*只需要最近的那个 probe 的 SpecularMap!*/
			animatedShader->SetUniform1i("u_PrefilterMap", 1);
			glActiveTexture(GL_TEXTURE1);
			kProbes[0]->GetSpecularPrefilterCubeMap()->Bind();



			for (int i = 0; i < k; i++)
			{
				animatedShader->SetUniform1f("u_ProbeWeight[" + std::to_string(i) + "]", (float)distances[i] / distancesSum);

				//m_IBLShader->SetUniform1i("u_IrradianceMap[" + std::to_string(i) + "]", textureK);
				//m_IBLShader->SetUniform1i("u_Image", textureK);
				for (int sh = 0; sh < 9; sh++)
				{
					int index = sh + 9 * i;
					animatedShader->SetUniformVec3f("u_SHCoeffs[" + std::to_string(index) + "]", glm::vec3(kProbes[i]->GetCoeffis()[sh][0], kProbes[i]->GetCoeffis()[sh][1], kProbes[i]->GetCoeffis()[sh][2]));
				}

				//glActiveTexture(GL_TEXTURE0 + textureK);
				//kProbes[i]->GetDiffuseIrradianceCubeMap()->Bind();
				//kProbes[i]->GetSHImage()->Bind(textureK);
				//textureK++;


			}
			if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject())
				animatedShader->SetUniform1f("u_IsPBRObjects", 1.0f);
			else
				animatedShader->SetUniform1f("u_IsPBRObjects", 0.0f);

			m_AnimatedModelRenderer->Render(obj, timeInSecond);
		/*	obj->GetComponent<MeshRenderer>()->SetShaders(m_GBufferShader);
			DrawObject(obj, m_GBufferShader);*/
		}




		//m_GBffer->UnBind();

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
		m_GBffer->UnBind();


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
		m_GBffer->GetAmbientGIAOTexture()->Bind();

		m_AmbientGIPassShader->SetUniformVec2f("gScreenSize", glm::vec2(m_TextureWidth, m_TexxtureHeight));

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
			m_GBffer->GetPositionTexture()->Bind();
			glActiveTexture(GL_TEXTURE1);
			m_GBffer->GetNormalTexture()->Bind();
			glActiveTexture(GL_TEXTURE2);
			m_GBffer->GetDiffuseRoughnessTexture()->Bind();
			glActiveTexture(GL_TEXTURE3);
			m_GBffer->GetSpecularMentallicTexture()->Bind();
			glActiveTexture(GL_TEXTURE4);
			m_GBffer->GetAmbientGIAOTexture()->Bind();
			glActiveTexture(GL_TEXTURE5);
			m_GBffer->GetNormalMapTexture()->Bind();

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

		frameBuffer->UnBind();

		glViewport(0, 0, m_TextureWidth, m_TexxtureHeight);

		glClear(GL_COLOR_BUFFER_BIT);
		m_FinalScreenShader->Bind();
		m_FinalScreenShader->SetUniform1i("u_FinalScreenTexture", 0);
		glActiveTexture(GL_TEXTURE0);
		m_HDRPostProcessTexture->Bind();
		DrawObject(m_FinalScreenQuad, m_FinalScreenShader);

		/* DirectionLight pass */
		//	DrawObject(m_ScreenQuad, m_DirectionLightPassShader);



		/*********************************正向渲染 light objects ******************************************************** /
		/************ 2.5. Copy content of geometry's depth buffer to default framebuffer's depth buffer****************/

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		m_GBffer->Bind();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		glBlitFramebuffer(0, 0, m_TextureWidth, m_TexxtureHeight, 0, 0, m_TextureWidth, m_TexxtureHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glDisable(GL_BLEND);

		glDepthFunc(GL_LEQUAL);

		DrawObject(skyBox);
		glDepthFunc(GL_LESS);
		DrawObjects(backGroundObjs);

		DrawLightSources(lightSources);

		/* Debug ... 画出包围球看看 */

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
```



![gBufferGIRendering](/results/gBufferGIRendering.png)
![gBufferGIRendering](/results/gBufferGIRendering1.png)
![gBufferGI_Animation_LightProbe_Rendering](/results/gBufferGI_Animation_LightProbe_Rendering.png)
![gBufferGI_Animation_LightProbe_Rendering](/results/gBufferGI_Animation_LightProbe_Rendering1.png)
![gBufferGI_Animation_LightProbe_Rendering](/results/gBufferGI_Animation_LightProbe_Rendering2.png)

