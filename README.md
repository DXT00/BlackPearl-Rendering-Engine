###  GBuffer  deferred rendering -- 使用包围球限制光照范围

时间复杂度减少为：O(Objects num + lights num)

reference:  
https://learnopengl-cn.github.io/05%20Advanced%20Lighting/08%20Deferred%20Shading/


#### 1.在每个光源出添加一个sphere,使用衰减方程计算球半径

![gBuffer](/results/gBufferPointLightSphere4.png)



GBufferRenderer.cpp

```
		void GBufferRenderer::Render(std::vector<Object*> objects,Object * gBufferDebugQuad, LightSources* lightSources)
	{
		GE_ASSERT(m_IsInitialized,"GBufferRenderer have not been initialized! ")
		

		// Only the geometry pass updates the depth buffer
		glDepthMask(GL_TRUE);

		glEnable(GL_DEPTH_TEST);

		glDisable(GL_BLEND);

		/************************1. Geometry Pass: render scene's geometry/color data into gbuffer *****************/
		/***********************************************************************************************************/
		m_GBffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		m_GBufferShader->Bind();
		DrawObjects(objects,m_GBufferShader);


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
		glClear(GL_COLOR_BUFFER_BIT );

		

		/* PointLight pass */
		for (Object* pointLight : lightSources->GetPointLights())
		{
			float radius =  CalculateSphereRadius(pointLight);
			m_SurroundSphere->GetComponent<Transform>()->SetScale({ radius,radius,radius });
			m_SurroundSphere->GetComponent<Transform>()->SetPosition(pointLight->GetComponent<Transform>()->GetPosition());


			m_PointLightPassShader->Bind();
			m_PointLightPassShader->SetUniform1i("gPosition", 0);
			m_PointLightPassShader->SetUniform1i("gNormal", 1);
			m_PointLightPassShader->SetUniform1i("gAlbedoSpec", 2);
			m_PointLightPassShader->SetUniform1i("gSpecular", 3);
			m_PointLightPassShader->SetUniformVec2f("gScreenSize", glm::vec2(m_TextureWidth, m_TexxtureHeight));

			glActiveTexture(GL_TEXTURE0);
			m_GBffer->GetPositionTexture()->Bind();
			glActiveTexture(GL_TEXTURE1);
			m_GBffer->GetNormalTexture()->Bind();
			glActiveTexture(GL_TEXTURE2);
			m_GBffer->GetAlbedoTexture()->Bind();
			glActiveTexture(GL_TEXTURE3);
			m_GBffer->GetSpecularTexture()->Bind();


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

		m_GBffer->Bind();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		glBlitFramebuffer(0, 0, m_TextureWidth, m_TexxtureHeight, 0, 0, m_TextureWidth, m_TexxtureHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

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




	}
```
![gBuffer](/results/gBufferPointLightSphere.png)
![gBuffer](/results/gBufferPointLightSphere1.png)
![gBuffer](/results/gBufferPointLightSphere2.png)
![gBuffer](/results/gBufferPointLightSphere3.png)
![gBuffer](/results/gBufferPointLightSphere5.png)



