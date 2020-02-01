### LightProbes --Render DiffuseMap

#### 1.更新DiffuseIrradianceMap，每次按下U键更新所有lightProbes的DiffuseMap（不需要每帧都更新，否则会卡死。。）

IBLProbesRenderingLayer.h

```
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		/*render scene*/
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_U)) {

			/*IBLProbes rendering*/
			m_IBLProbesRenderer->Render(m_ProbesCamera, *GetLightSources(), m_BackGroundObjsList, m_LightProbes);
		}
		

		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		m_BasicRenderer->RenderScene(m_BackGroundObjsList, GetLightSources());


		m_IBLProbesRenderer->RenderProbes(m_LightProbes);



	}
```

#### 2.更新DiffuseIrradianceMap
IBLProbesRenderer class:

```
	void IBLProbesRenderer::RenderDiffuseIrradianceMap(Object* ProbesCamera, const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probe)
	{
		auto cameraComponent = ProbesCamera->GetComponent<PerspectiveCamera>();
		std::shared_ptr<CubeMapTexture> diffuseIrradianceCubeMap = probe->GetDiffuseIrradianceCubeMap();
		
		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		frameBuffer->Bind();
		frameBuffer->AttachRenderBuffer(diffuseIrradianceCubeMap->GetWidth(),diffuseIrradianceCubeMap->GetHeight());

		glViewport(0, 0, diffuseIrradianceCubeMap->GetWidth(), diffuseIrradianceCubeMap->GetWidth()); // don't forget to configure the viewport to the capture dimensions.
		
		// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
		for (unsigned int i = 0; i < 6; ++i)
		{
			Renderer::SceneData* scene = DBG_NEW Renderer::SceneData{ m_ProbeProjectionViews[i] ,m_ProbeView[i],m_ProbeProjection,probe->GetPosition(),cameraComponent->Front(),lightSources };
		
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuseIrradianceCubeMap->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//remember bind shader before drawcall!!
			m_IrradianceShader->Bind();
			m_IrradianceShader->SetUniform1i("u_EnvironmentMap", 2);
			glActiveTexture(GL_TEXTURE2);
			probe->GetHdrEnvironmentCubeMap()->Bind();

			DrawObject(probe->GetCubeObj(), m_IrradianceShader, scene);
		}
		frameBuffer->UnBind();
		frameBuffer->CleanUp();
	}
```


![UpdateDiffuseIrradianceMap](/results/lightProbesDiffuseMapUpdate1.png)
![UpdateDiffuseIrradianceMap](/results/lightProbesDiffuseMapUpdate2.png)
![UpdateDiffuseIrradianceMap](/results/lightProbesDiffuseMapUpdate3.png)
![UpdateDiffuseIrradianceMap](/results/lightProbesDiffuseMapUpdate4.png)



