###  LightProbe + GBuffer + AnimatedModel+shadowMap deferred rendering


#### 1.为每个lightProbe生成环境贴图 （一次即可）

#### 2.为每个PointLight生成ShadowMap

#### 3.结合 lightProbes获取环境光-GI ,渲染 objects属性到 gBuffer

#### 4.从gBuffer获取数据 -》渲染 GI pass,pointLight pass 到frameBuffer

#### 5.frameBuffer 做后期处理 --》包括HDR ,tone Mapping

#### 6.添加Animated Model （dynamic objects） 使用AnimatedModelRenderer 渲染

GBufferRenderer.cpp -->RenderSceneWithGBufferAndProbes()

IBLProbesRenderingLayer.h :

```
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		/*render scene*/
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());


		if (BlackPearl::Input::IsKeyPressed(BP_KEY_U)) {

			m_IBLProbesRenderer->Render(GetLightSources(), m_BackGroundObjsList, m_LightProbes, m_SkyBoxObj1);

		}

		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtime = currentTimeMs.count()-m_StartTimeMs.count();
		


		
		
		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		m_ShadowMapPointLightRenderer->RenderCubeMap(m_SphereObjsList,m_DynamicObjsList,runtime / 1000.0f,GetLightSources());
		m_GBufferRenderer->RenderSceneWithGBufferAndProbes(m_SphereObjsList, m_DynamicObjsList, runtime / 1000.0f,m_BackGroundObjsList, m_GBufferDebugQuad, GetLightSources(), m_LightProbes, m_IBLProbesRenderer->GetSpecularBrdfLUTTexture(), m_SkyBoxObj1);
		m_IBLProbesRenderer->RenderProbes(m_LightProbes);


	}
```

![LightProbes+gBuffer+Shadow+Animation1](/results/LightProbes+gBuffer+Shadow+Animation1.png)
![LightProbes+gBuffer+Shadow+Animation](/results/LightProbes+gBuffer+Shadow+Animation2.png)
