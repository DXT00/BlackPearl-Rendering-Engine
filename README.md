

1.添加FrameBuffer

2.把FrameBuffer中的TextureBuffer作为贴图，贴到正方体CubeObj上

3.主要代码如下：

	初始化：
```
	  	m_PlaneObj= Layer::CreatePlane();
	
		m_CubeObj = Layer::CreateCube();
		

		//把FrameBuffer中的texture作为贴图，贴到m_CubeObj上
		auto CubemeshComponent = m_CubeObj->GetComponent<BlackPearl::MeshRenderer>();
		CubemeshComponent->SetTexture(0, m_FrameBuffer->GetColorTexture());
```

	Update循环：
```
		m_FrameBuffer->Bind(960,540);

		glEnable(GL_DEPTH_TEST);

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);

		BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		

		
		DrawObjectsExcept(m_CubeObj);
		

		m_FrameBuffer->UnBind();
		

		glClearColor(0.0f, 0.1f, 0.1f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

		glViewport(0, 0, 960, 540);
		

		DrawObjects();
```
4.FrameBuffer 学习链接参考：https://www.youtube.com/watch?v=21UsMuFTN0k&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh&index=2
		