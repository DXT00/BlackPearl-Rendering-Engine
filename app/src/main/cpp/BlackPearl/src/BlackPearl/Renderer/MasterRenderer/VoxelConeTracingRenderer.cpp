#include "pch.h"
#include "Renderer/MasterRenderer/VoxelConeTracingRenderer.h"
#include "Component/MeshRendererComponent/MeshRenderer.h"
#include "Component/TransformComponent/Transform.h"
#include "Component/LightComponent/PointLight.h"
#include "Renderer/DeviceManager.h"
#include "RHI/RHITexture.h"
#include "ObjectManager/ObjectManager.h"
#include "Timestep/TimeCounter.h"
#include "Renderer/SystemTextures.h"
#include "Map/MapManager.h"
#include "Timestep/SystemTime.h"
#ifdef GE_API_OPENGL
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLDrv.h"
#endif
#include "hlsl/core/voxel_cb.h"
#include "hlsl/core/sky_cb.h"

#include "BlackPearl/RHI/Common/RHIUtils.h"
#include "Config.h"
#include "Renderer/MasterRenderer/SkyboxRenderer.h"
#include "Component/CameraComponent/OrthographicCamera.h"

namespace BlackPearl {
	extern DeviceManager* g_deviceManager;
    extern ObjectManager* g_objectManager;
    extern MapManager* g_mapManager;

	bool VoxelConeTracingRenderer::s_Shadows = true;
	bool VoxelConeTracingRenderer::s_IndirectDiffuseLight = true;
	bool VoxelConeTracingRenderer::s_IndirectSpecularLight = true;
	bool VoxelConeTracingRenderer::s_DirectLight = false;
	float VoxelConeTracingRenderer::s_GICoeffs = 0.8f;
	bool VoxelConeTracingRenderer::s_VoxelizeNow = true;
	bool VoxelConeTracingRenderer::s_HDR = false;
	int VoxelConeTracingRenderer::s_VisualizeMipmapLevel = 0;

	VoxelConeTracingRenderer::VoxelConeTracingRenderer(IDevice* device)
        :BasicRenderer(device)
	{

      
	}
	VoxelConeTracingRenderer::~VoxelConeTracingRenderer()
	{
		if (!m_CubeObj) delete m_CubeObj;
		if (!m_QuadObj) delete m_QuadObj;
		if(!m_VoxelTexture) delete m_VoxelTexture;
	}


	void VoxelConeTracingRenderer::Init(Scene* scene)
	{

        m_QuadObj = scene->GetFullScreenObj();
        m_CubeObj = g_objectManager->CreateCube();
		//m_BrdfLUTQuadObj = scene->GetFullScreenObj();


		//glEnable(GL_MULTISAMPLE);
		m_VoxelConeTracingShader = DBG_NEW MaterialShader("assets/shaders/glsl/voxelization/voxelConeTracing/voxelConeTracingPBR.glsl");
        InitVoxelization();
		InitVoxelVisualization();

		/*debug shader*/
		//m_VoxelizationTestShader.reset(DBG_NEW Shader("assets/shaders/voxelization/debug/voxelizeTest.glsl"));
		//m_FrontBackCubeTestShader.reset(DBG_NEW Shader("assets/shaders/voxelization/debug/quadTest.glsl"));

		/*pbr BRDF LUT shader*/
		m_SpecularBRDFLutShader = DBG_NEW MaterialShader("assets/shaders/glsl/ibl/brdf.glsl");
		const std::vector<GLfloat> textureImage2D(4 * 256 * 256 , 0.0f);
		RenderSpecularBRDFLUTMap();


		m_IsInitialize = true;
	}

	void VoxelConeTracingRenderer::InitVoxelization()
	{
		m_VoxelizationShader = DBG_NEW MaterialShader("assets/shaders/glsl/voxelization/voxelizationPBR_TileCenter.glsl");
        m_VoxelizationSkyboxShader = DBG_NEW MaterialShader("assets/shaders/glsl/voxelization/voxelizationSky_TileCenter.glsl");
        m_VoxelClearShader         = DBG_NEW MaterialShader("assets/shaders/glsl/voxelization/voxelClear.glsl");

        //m_VoxelizationShader.reset(DBG_NEW Shader("assets/shaders/glsl/voxelization/voxelization.glsl"));
        std::vector<Area*> areas = g_mapManager->GetAreasList();
        for (size_t i = 0; i < areas.size(); i++)
        {
            Voxel voxel;
            voxel.Init(areas[i]->GetCenter(), areas[i]->GetId(), areas[i]->GetExtend());
            GE_ASSERT(i == areas[i]->GetId());
            areas[i]->RegisterVoxelId(i);

            SystemTexture::Get().SceneVoxels.push_back(voxel);
        }


        TextureDesc desc;
        desc.minFilter = FilterMode::Linear;
        desc.magFilter = FilterMode::Linear;
        desc.wrap = SamplerAddressMode::ClampToEdge;
        desc.format = Format::RGBA8_UNORM;
        desc.dimension = TextureDimension::Texture2D;
        desc.width = Configuration::VoxelDim;
        desc.height = Configuration::VoxelDim;

        m_DummyVoxelRT = m_Device->createTexture(desc);



        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.bindings = {
                RHIBindingLayoutItem::RT_VolatileConstantBuffer(8),
                RHIBindingLayoutItem::RT_Texture_SRV(0)
        };
        m_VoxelBindingLayout = m_Device->createBindingLayout(layoutDesc);

      
        m_VoxelCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(VoxelConstants), "VoxelConstants"));


        //Orth Camera
        m_OrthCamera = g_objectManager->CreateCamera("OrthCamera", Camera::CameraType::Orthographic);
      
       
        //	cameraComponent->SetPosition(probe->GetPosition());

        //std::vector<glm::mat4> ProbeView = {
        //   glm::lookAt(center, center - camera->Front(),-camera->Up()),
        //   glm::lookAt(center, center + camera->Front(),-camera->Up()),
        //   glm::lookAt(center, center + camera->Up(),-camera->Right()),//-camera->Front()
        //   glm::lookAt(center, center - camera->Up(),camera->Right()),//camera->Front()

        //   glm::lookAt(center, center - camera->Right(), -camera->Up()),
        //   glm::lookAt(center, center + camera->Right(), -camera->Up()),

        //};
        //std::vector<glm::mat4> ProbeProjectionViews = {
        //    projection * ProbeView[0],
        //    projection * ProbeView[1],
        //    projection * ProbeView[2],
        //    projection * ProbeView[3],
        //    projection * ProbeView[4],
        //    projection * ProbeView[5]
        //};




	/*	const std::vector<GLfloat> texture3D(4 * m_VoxelTextureSize*m_VoxelTextureSize*m_VoxelTextureSize, 0.0f);
		m_VoxelTexture = DBG_NEW Texture3D(texture3D, m_VoxelTextureSize, m_VoxelTextureSize, m_VoxelTextureSize, true);
	*/
        //init voxel:

        // init skybox 
  
        m_SkyboxTexture[0] = SystemTexture::Get().SkyboxTexture0;
        m_SkyboxTexture[1] = SystemTexture::Get().SkyboxTexture1;
        m_SkyboxTexture[2] = SystemTexture::Get().SkyboxTexture2;


        //Skybox Material
        RHIBindingLayoutDesc skyLayoutDesc;
        skyLayoutDesc.visibility = ShaderType::Pixel;
        skyLayoutDesc.bindings = {
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(2),
            RHIBindingLayoutItem::RT_Texture_SRV(1),
            RHIBindingLayoutItem::RT_Texture_SRV(2),
            RHIBindingLayoutItem::RT_Texture_SRV(3)

        };
        m_SkyboxBindingLayout = m_Device->createBindingLayout(skyLayoutDesc);
        m_SkyCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(SkyConstants), "SkyConstants"));

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::ConstantBuffer(2, m_SkyCB),
            BindingSetItem::Texture_SRV(1, m_SkyboxTexture[0].Get(), "skyboxTexture0"),
            BindingSetItem::Texture_SRV(2, m_SkyboxTexture[1].Get(), "skyboxTexture1"),
            BindingSetItem::Texture_SRV(3, m_SkyboxTexture[2].Get(), "skyboxTexture2")

        };
        m_SkyboxBindingSet = m_Device->createBindingSet(bindingSetDesc, m_SkyboxBindingLayout);


    }
	void VoxelConeTracingRenderer::InitVoxelVisualization()
	{
		//Shaders
		m_WorldPositionShader = DBG_NEW MaterialShader("assets/shaders/glsl/voxelization/visualization/worldPosition.glsl");
        m_VoxelVisualizationShader = DBG_NEW MaterialShader("assets/shaders/glsl/voxelization/visualization/voxelVisualization_TileCenter.glsl");



        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.bindings = {
                RHIBindingLayoutItem::RT_VolatileConstantBuffer(8),
        };
        m_VoxelVisualBindingLayout = m_Device->createBindingLayout(layoutDesc);

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
                    BindingSetItem::ConstantBuffer(8, m_VoxelCB),
        };


        m_VoxelVisualCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(VoxelVisualConstants), "VoxelVisualConstants"));
	}

    void VoxelConeTracingRenderer::Voxilize(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, bool clearVoxelizationFirst)
    {

        SCOPE_TIME_COUNTER(Voxelize)
            FRHIRenderPassInfo RPShadingInfo(m_DummyVoxelRT,
                ERenderTargetActions::Clear_Store
            );

       if (clearVoxelizationFirst) {

           _VoxelClear(cmdList, targetFramebuffer, scene);
    
        
    	}
        cmdList->beginRenderPass(RPShadingInfo, "Voxilize");
        _VoxelizeScene(cmdList, targetFramebuffer, scene);
      //  _VoxelizeSky(cmdList, targetFramebuffer, scene);
        cmdList->endRenderPass();

    }



	void VoxelConeTracingRenderer::Voxilize(const std::vector<Object*>& objs, Object* skybox,bool clearVoxelizationFirst)
	{
	//	if (clearVoxelizationFirst) {
	//		float clearColor[4] = { 0.0,0.0,0.0,0.0 };
	//		m_VoxelTexture->Clear(clearColor);
	//	}
	//	//m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
	////	m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(false);


	//	glViewport(0, 0, m_VoxelTextureSize, m_VoxelTextureSize);
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//	// Settings.
	//	//  disable writing of frame buffer color components
	//	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	//	glDisable(GL_CULL_FACE);
	//	glDisable(GL_DEPTH_TEST);
	//	glDisable(GL_BLEND);

	//	
	//	
	//	//for (auto obj : objs) {
	//	//	if (obj->HasComponent<MeshRenderer>())
	//	//		obj->GetComponent<MeshRenderer>()->SetShaders(m_VoxelizationShader);
	//	//}
	//	//Render
	//	//Voxel Texture
	//

	//	if (skybox != nullptr) {

	//		//glDepthFunc(GL_LEQUAL);
	//		m_VoxelizationShader->Bind();
	//		//glActiveTexture(GL_TEXTURE0);
	//		GE_ERROR_JUDGE();

	//	/*	m_VoxelTexture->Bind();
	//		m_VoxelizationShader->SetUniform1i("texture3D", 0);
	//		m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());*/
	//		glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
	//		skybox->GetComponent<Transform>()->SetScale(m_CubeObj->GetComponent<Transform>()->GetScale() - glm::vec3(3.0f));
	//		skybox->GetComponent<Transform>()->SetPosition(Renderer::GetSceneData()->CameraPosition);
	//		GE_ERROR_JUDGE();

	//		m_VoxelizationShader->SetUniform1i("u_IsSkybox", 1);
	//		m_VoxelizationShader->SetUniform1i("u_IsPBRObjects", 0);
	//		GE_ERROR_JUDGE();

	//		DrawObject(skybox, m_VoxelizationShader);
	//		GE_ERROR_JUDGE();

	//		//glDepthFunc(GL_LESS);
	//	}
	//		
	//	for (auto obj : objs) {
	//		//m_VoxelizationShader->Bind();
	//		//glActiveTexture(GL_TEXTURE0);
	//		//m_VoxelTexture->Bind();
	//		//m_VoxelizationShader->SetUniform1i("texture3D", 0);
	//		//m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
	//		////m_VoxelizationShader->SetUniformVec3f("u_CubePos", m_CubeObj->GetComponent<Transform>()->GetPosition());
	//		//glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16);
	//		m_VoxelizationShader->Bind();
	//		GE_ERROR_JUDGE();

	//		/*glActiveTexture(GL_TEXTURE0);
	//		m_VoxelTexture->Bind();*/
	//		m_VoxelizationShader->SetUniform1i("texture3D", 0);
	//		m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
	//		glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
	//		GE_ERROR_JUDGE();

	//		//m_VoxelizationShader->SetUniform1i("u_IsSkybox", 0);
	//		//GE_ERROR_JUDGE();


	//		if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
	//			m_VoxelizationShader->SetUniform1i("u_IsPBRObjects", 1);
	//			GE_ERROR_JUDGE();

	//		}
	//		else {
	//			m_VoxelizationShader->SetUniform1i("u_IsPBRObjects", 0);
	//			GE_ERROR_JUDGE();


	//		}
	//		DrawObject(obj, m_VoxelizationShader, Renderer::GetSceneData(), 4);
	//		GE_ERROR_JUDGE();

	//	}
	//	if (m_AutomaticallyRegenerateMipmap || m_RegenerateMipmapQueued) {
	//		m_VoxelTexture->Bind();
	//		glGenerateMipmap(GL_TEXTURE_3D);
	//		m_VoxelTexture->UnBind();

	//		m_RegenerateMipmapQueued = false;
	//	}
	//	//enable writing of frame buffer color components
	//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	}

	//void VoxelConeTracingRenderer::Render(Camera* camera,const std::vector<Object*>& objs, const LightSources * lightSources, unsigned int viewportWidth, unsigned int viewportHeight, Object* skybox,
	//	RenderingMode reneringMode)
    void VoxelConeTracingRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, RenderingMode reneringMode)
	{
		GE_ASSERT(m_IsInitialize, "Please Call VoxelConeTracingRenderer::Init() first!");
		//bool voxelizeNow = m_VoxelizationQueued || (m_AutomaticallyVoxelize &&m_VoxelizationSparsity > 0 && ++m_TicksSinceLastVoxelization >= m_VoxelizationSparsity);
		if (s_VoxelizeNow) {
			Voxilize(cmdList, targetFramebuffer, scene, true);
			m_TicksSinceLastVoxelization = 0;
			m_VoxelizationQueued = false;
           // s_VoxelizeNow = false;
		}
		switch (reneringMode) {
		    case RenderingMode::VOXELIZATION_VISUALIZATION:
			    RenderVoxelVisualization(cmdList, targetFramebuffer, scene);
			    break;

		    case RenderingMode::VOXEL_CONE_TRACING:
			    RenderScene(cmdList, targetFramebuffer, scene);
			    break;
            case RenderingMode::VOXELIZE:
                break;
		}
	}

	void VoxelConeTracingRenderer::RenderVoxelVisualization(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
	{
        auto camPos = Renderer::GetSceneData()->CameraPosition;

        SCOPE_TIME_COUNTER(VoxilizeVisualize)
            FRHIRenderPassInfo RPShadingInfo(SystemTexture::Get().GetBackBuffer(),
                ERenderTargetActions::Clear_Store
            );
        cmdList->beginRenderPass(RPShadingInfo, "VoxilizeVisualize");

        // todo:: 收集附近8个体素
        uint32_t areaId = g_mapManager->CalculateAreaId(camPos);
        uint32_t voxelId = g_mapManager->GetArea(areaId)->GetVoxelId();
        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
                    BindingSetItem::ConstantBuffer(8, m_VoxelVisualCB),
                    BindingSetItem::Texture_SRV(0, SystemTexture::Get().SceneVoxels[voxelId].voxelTexture,"VoxelTexture")
        };


        m_VoxelVisualBindingSet = m_Device->createBindingSet(bindingSetDesc, m_VoxelVisualBindingLayout);



        SceneData* view = Renderer::GetSceneData();
        GE_ERROR_JUDGE();

        SceneData* preView = Renderer::GetPreSceneData();
        GE_ERROR_JUDGE();

        SetupView(cmdList, view, preView);

        DrawItem drawItem = IDrawStrategy::ObjectToDrawItem(scene->GetFullScreenObj())[0];

        GraphicsState graphicsPSO;
        graphicsPSO.framebuffer = targetFramebuffer;
        graphicsPSO.viewport = view->GetViewportState();
        graphicsPSO.shadingRateState = view->GetVariableRateShadingState();

        GraphicsPipelineDesc psoDesc;

        psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
        psoDesc.depthStencilState.enableDepthTest();
        psoDesc.depthStencilState.disableDepthWrite();
        psoDesc.depthStencilState.disableStencil();

        psoDesc.blendState.alphaToCoverageEnable = false;
        psoDesc.rasterState.frontCounterClockwise = true;
        psoDesc.rasterState.cullMode = RasterCullMode::Back;
        psoDesc.primType = PrimitiveType::TriangleList;
        psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

        psoDesc.VS = m_VoxelVisualizationShader->GetVertexShader();
        psoDesc.PS = m_VoxelVisualizationShader->GetPixelShader();
        psoDesc.bFromPSOFileCache = false;
        psoDesc.bindingLayouts.push_back(m_VoxelVisualBindingLayout);
        psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);
        



        if (!m_VoxelVisualPso) {
            m_VoxelVisualPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
        }
        graphicsPSO.pipeline = m_VoxelVisualPso;
        {
            graphicsPSO.bindings.push_back(m_VoxelVisualBindingSet);
            graphicsPSO.bindings.push_back(m_ViewBindingset);
        }
        graphicsPSO.inputLayout = psoDesc.inputLayout;
  
        SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
        GE_ERROR_JUDGE();

        VoxelVisualConstants voxelVisualConstants{};
        voxelVisualConstants.areaExtent = g_mapManager->GetArea(areaId)->GetExtend();
        voxelVisualConstants.center = SystemTexture::Get().SceneVoxels[voxelId].center;
        voxelVisualConstants.dimension = SystemTexture::Get().SceneVoxels[voxelId].dimension;
        voxelVisualConstants.mipLevel = SystemTexture::Get().SceneVoxels[voxelId].mipLevel;

        cmdList->writeBuffer(m_VoxelVisualCB, &voxelVisualConstants, sizeof(VoxelVisualConstants));

        cmdList->setGraphicsState(graphicsPSO);

        Draw(cmdList, drawItem);

        cmdList->endRenderPass();









		//m_CubeObj->GetComponent<MeshRenderer>()->SetShaders(m_WorldPositionShader);
		//m_CubeObj->GetComponent<Transform>()->SetPosition(Renderer::GetSceneData()->CameraPosition);
		//m_CubeObj->GetComponent<Transform>()->SetRotation(Renderer::GetSceneData()->CameraRotation);

		//// -------------------------------------------------------
		//// Render 3D texture to screen.
		//// -------------------------------------------------------

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0, viewportWidth, viewportHeight);
		///*	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//// Render.

	

		//m_VoxelVisualizationShader->Bind();

		//glActiveTexture(GL_TEXTURE0);
		//m_VoxelTexture->Bind();
		//m_VoxelVisualizationShader->SetUniform1i("texture3D", 0);


	

		//m_VoxelVisualizationShader->SetUniformVec3f("u_CameraFront", camera->Front());
		//m_VoxelVisualizationShader->SetUniformVec3f("u_CameraUp", camera->Up());
		//m_VoxelVisualizationShader->SetUniformVec3f("u_CameraRight", camera->Right());

		//m_VoxelVisualizationShader->SetUniform1i("u_State",s_VisualizeMipmapLevel);
		//m_VoxelVisualizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());

		//
		////m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(true);
		//DrawObject(m_QuadObj, m_VoxelVisualizationShader);

		

		

	}

	void VoxelConeTracingRenderer::RenderScene(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
	{


	////	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	m_CubeObj->GetComponent<Transform>()->SetPosition(Renderer::GetSceneData()->CameraPosition);
	//	m_CubeObj->GetComponent<Transform>()->SetRotation(Renderer::GetSceneData()->CameraRotation);


	//	

	//	// GL Settings.
	//	glViewport(0, 0, viewportWidth, viewportHeight);
	//	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glEnable(GL_DEPTH_TEST);
	//	glEnable(GL_CULL_FACE);
	//	glCullFace(GL_BACK);
	//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//
	//	
	//	//TODO::
	//	float specularReflectivity = 1.0f, diffuseReflectivity = 1.0f, emissivity =0.1f, specularDiffusion = 0.0f;
	//	float transparency = 0.0f, refractiveIndex = 1.4f;

	//	//m_VoxelConeTracingShader->Bind();



	//	for (auto obj : objs) {
	//		m_VoxelConeTracingShader->Bind();


	//	//	m_VoxelConeTracingShader->SetUniform1i("u_Settings.shadows", s_Shadows);
	//		m_VoxelConeTracingShader->SetUniform1i("u_Settings.indirectDiffuseLight", s_IndirectDiffuseLight);
	//		m_VoxelConeTracingShader->SetUniform1i("u_Settings.indirectSpecularLight", s_IndirectSpecularLight);
	//		m_VoxelConeTracingShader->SetUniform1i("u_Settings.directLight", s_DirectLight);
	//		m_VoxelConeTracingShader->SetUniform1f("u_Settings.GICoeffs", s_GICoeffs);
	//		m_VoxelConeTracingShader->SetUniform1i("u_Settings.hdr", s_HDR);

	//		m_VoxelConeTracingShader->SetUniform1i("u_State", s_VisualizeMipmapLevel);

	//		m_VoxelConeTracingShader->SetUniform1f("u_Material.diffuseReflectivity", diffuseReflectivity);
	//		m_VoxelConeTracingShader->SetUniform1f("u_Material.specularReflectivity", specularReflectivity);
	//		m_VoxelConeTracingShader->SetUniform1f("u_Material.emissivity", emissivity);

	//		m_VoxelConeTracingShader->SetUniform1f("u_Material.specularDiffusion", specularDiffusion);
	//		m_VoxelConeTracingShader->SetUniform1f("u_Material.transparency", transparency);
	//		m_VoxelConeTracingShader->SetUniform1f("u_Material.refractiveIndex", refractiveIndex);
	//		m_VoxelConeTracingShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
	//		GE_ERROR_JUDGE();


	//		glActiveTexture(GL_TEXTURE0);
	//		GE_ERROR_JUDGE();

	//		m_VoxelTexture->Bind();
	//		GE_ERROR_JUDGE();

	//		m_VoxelConeTracingShader->SetUniform1i("texture3D", 0);
	//		GE_ERROR_JUDGE();
	//		//glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8);

	//		glActiveTexture(GL_TEXTURE1);
	//		m_SpecularBrdfLUTTexture->Bind();
	//		m_VoxelConeTracingShader->SetUniform1i("u_BrdfLUTMap", 1);
	//		GE_ERROR_JUDGE();

	//		if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
	//			m_VoxelConeTracingShader->SetUniform1i("u_IsPBRObjects", 1);
	//			GE_ERROR_JUDGE();
	//		}
	//		else {
	//			m_VoxelConeTracingShader->SetUniform1i("u_IsPBRObjects", 0);
	//			GE_ERROR_JUDGE();

	//		}
	//		DrawObject(obj, m_VoxelConeTracingShader);
	//		GE_ERROR_JUDGE();

	//	}
		//DrawObjects(objs, m_VoxelConeTracingShader);

	}

	void VoxelConeTracingRenderer::RenderSpecularBRDFLUTMap()
	{
		//TextureDesc desc;
		//desc.type = TextureType::DiffuseMap;
		//desc.width = m_VoxelTextureSize;
		//desc.height = m_VoxelTextureSize;
		//desc.minFilter = FilterMode::Linear;
		//desc.magFilter = FilterMode::Linear;
		//desc.wrap = SamplerAddressMode::ClampToEdge;
		//desc.format = Format::RG16_FLOAT;
		//m_SpecularBrdfLUTTexture = g_deviceManager->GetDevice()->createTexture(desc);

		////m_SpecularBrdfLUTTexture.reset(DBG_NEW Texture(Texture::DiffuseMap, m_VoxelTextureSize, m_VoxelTextureSize, false, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		////std::shared_ptr<Texture> brdfLUTTexture(new Texture(Texture::None, 512, 512, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		//std::shared_ptr<FrameBuffer> frameBuffer = std::make_shared<FrameBuffer>();
		////	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//frameBuffer->Bind();
		//frameBuffer->AttachRenderBuffer(m_VoxelTextureSize, m_VoxelTextureSize);

		////m_FrameBuffer->Bind();
		////m_FrameBuffer->BindRenderBuffer();
		//frameBuffer->AttachColorTexture(m_SpecularBrdfLUTTexture, 0);
		//frameBuffer->BindRenderBuffer();
		////glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);


		//glViewport(0, 0, m_VoxelTextureSize, m_VoxelTextureSize);
		//m_SpecularBRDFLutShader->Bind();
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//DrawObject(m_BrdfLUTQuadObj, m_SpecularBRDFLutShader);
		//frameBuffer->UnBind();
		//frameBuffer->CleanUp();


	}

    void VoxelConeTracingRenderer::_VoxelClear(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        cmdList->beginMarker("VoxelizeClear");
        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Compute;
        layoutDesc.bindings = {
                RHIBindingLayoutItem::RT_Texture_UAV(0),
        };
        m_VoxelClearBindingLayout = m_Device->createBindingLayout(layoutDesc);

        for (size_t i = 0; i < SystemTexture::Get().SceneVoxels.size(); i++)
        {
            ComputePipelineDesc psoDesc;

            psoDesc.bindingLayouts.push_back(m_VoxelClearBindingLayout);
            psoDesc.CS = m_VoxelClearShader->GetComputeShader();

            if (!m_VoxelClearPso) {
                m_VoxelClearPso = m_Device->createComputePipeline(psoDesc);
            }

            BindingSetDesc bindingSetDesc;
            bindingSetDesc.bindings = {
                        BindingSetItem::Texture_UAV(0, SystemTexture::Get().SceneVoxels[i].voxelTexture,"VoxelTexture"+std::to_string(i))
            };


            m_VoxelClearBindingSet = m_Device->createBindingSet(bindingSetDesc, m_VoxelClearBindingLayout);




            ComputeState computePSO;
            computePSO.pipeline = m_VoxelClearPso;
            computePSO.bindings.push_back(m_VoxelClearBindingSet);


            cmdList->setComputeState(computePSO);
            cmdList->dispatch(SystemTexture::Get().SceneVoxels[i].dimension/ 8, SystemTexture::Get().SceneVoxels[i].dimension / 8, SystemTexture::Get().SceneVoxels[i].dimension / 8);

        }
        cmdList->endMarker();
       
    }

    void VoxelConeTracingRenderer::_VoxelizeScene(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        // view 没用到
        /*SceneData* view = Renderer::GetSceneData();
        ViewportState viewState;
        viewState.addViewportAndScissorRect(RHIViewport(m_DummyVoxelRT.Get()->getDesc().width, m_DummyVoxelRT.Get()->getDesc().height));*/
        cmdList->beginMarker("VoxelizeScene");







        const std::vector<Area*>& areas = g_mapManager->GetAreasList();
        GE_ASSERT(areas.size() == SystemTexture::Get().SceneVoxels.size());
        for (size_t i = 0; i < areas.size(); i++)
        {
            const auto& objs = areas[i]->GetObjects();

            if (objs.empty())
                continue;


            uint32_t voxelId = areas[i]->GetVoxelId();
            m_OrthCamera->SetPosition(Math::ToVec3(SystemTexture::Get().SceneVoxels[voxelId].center));
            OrthographicCamera* cameraComponent = m_OrthCamera->GetObj()->GetComponent<OrthographicCamera>();
            cameraComponent->SetRange(
                -SystemTexture::Get().SceneVoxels[voxelId].areaExtend.x * 0.5,
                 SystemTexture::Get().SceneVoxels[voxelId].areaExtend.x * 0.5,
                -SystemTexture::Get().SceneVoxels[voxelId].areaExtend.y * 0.5,
                 SystemTexture::Get().SceneVoxels[voxelId].areaExtend.y * 0.5
            );
            const glm::mat4& projection = cameraComponent->GetProjectionMatrix();
            const glm::mat4& view = cameraComponent->GetViewMatrix();

            SceneData* voxelView = DBG_NEW SceneData({ projection* view ,view,projection, m_OrthCamera->GetPosition(),{},cameraComponent->Front(),*scene->GetLightSources() });
            voxelView->SetViewport(RHIViewport(
                SystemTexture::Get().SceneVoxels[voxelId].areaExtend.x,
                SystemTexture::Get().SceneVoxels[voxelId].areaExtend.y
            ));
            voxelView->zNear = -1.0f;
            voxelView->zFar = 1.0f;

            SetupView(cmdList, voxelView, nullptr);


            BindingSetDesc bindingSetDesc;
            bindingSetDesc.bindings = {
                BindingSetItem::ConstantBuffer(8, m_VoxelCB),
                //BindingSetItem::ConstantBuffer(8, m_ForwardLightCB),
                BindingSetItem::Texture_UAV(0, SystemTexture::Get().SceneVoxels[areas[i]->GetVoxelId()].voxelTexture)
            };
            m_VoxelBindingSet = m_Device->createBindingSet(bindingSetDesc, m_VoxelBindingLayout);


            for (auto& obj : objs)
            {
                for (auto& drawItem : IDrawStrategy::ObjectToDrawItem(obj)) {


                    GraphicsState graphicsPSO;
                    graphicsPSO.framebuffer = targetFramebuffer;
                    graphicsPSO.viewport = voxelView->GetViewportState();

                    GraphicsPipelineDesc psoDesc;
                    psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::Less);
                    psoDesc.depthStencilState.enableDepthTest();
                    psoDesc.depthStencilState.disableDepthWrite();
                    psoDesc.depthStencilState.disableStencil();

                    psoDesc.blendState.alphaToCoverageEnable = false;
                    psoDesc.rasterState.frontCounterClockwise = true;
                    psoDesc.rasterState.cullMode = RasterCullMode::None;
                    psoDesc.primType = PrimitiveType::TriangleList;
                    psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

                    psoDesc.VS = m_VoxelizationShader->GetVertexShader();
                    psoDesc.GS = m_VoxelizationShader->GetGeometryShader();
                    psoDesc.PS = m_VoxelizationShader->GetPixelShader();
                    psoDesc.bFromPSOFileCache = false;
                    psoDesc.bindingLayouts.push_back(m_VoxelBindingLayout);
                    psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


                    if (!m_VoxelPso) {
                        m_VoxelPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
                    }
                    graphicsPSO.pipeline = m_VoxelPso;
                    graphicsPSO.bindings.push_back(m_VoxelBindingSet);
                    graphicsPSO.bindings.push_back(m_ViewBindingset);
                    graphicsPSO.inputLayout = psoDesc.inputLayout;


                    const_cast<Material*>(drawItem.material)->UploadConstantsBuffer(cmdList);
                    GE_ERROR_JUDGE();
                    SetupMaterial(drawItem.material, drawItem.cullMode, psoDesc, graphicsPSO);
                    GE_ERROR_JUDGE();

                    SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
                    GE_ERROR_JUDGE();


                  
                    VoxelConstants voxelConstants{};
                    voxelConstants.center = SystemTexture::Get().SceneVoxels[i].center;
                    voxelConstants.areaExtent = areas[i]->GetExtend();
                    voxelConstants.dimension = SystemTexture::Get().SceneVoxels[i].dimension;



                    cmdList->writeBuffer(m_VoxelCB, &voxelConstants, sizeof(VoxelConstants));
                    cmdList->setGraphicsState(graphicsPSO);

                    Draw(cmdList, drawItem);


                }

            }
        }
        cmdList->endMarker();

    }

    void VoxelConeTracingRenderer::_VoxelizeSky(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        cmdList->beginMarker("VoxelizeSky");
        SceneData* view = Renderer::GetSceneData();
        ViewportState viewState;
        viewState.addViewportAndScissorRect(RHIViewport(m_DummyVoxelRT.Get()->getDesc().width, m_DummyVoxelRT.Get()->getDesc().height));

        const std::vector<Area*>& areas = g_mapManager->GetAreasList();
        GE_ASSERT(areas.size() == SystemTexture::Get().SceneVoxels.size());

        // 设置sky trasform ，为了包围在voxel周围
        Object* sky = scene->GetSkyBox();
        auto oldPos = sky->GetComponent<Transform>()->GetPosition();
        auto oldScale = sky->GetComponent<Transform>()->GetScale();
 


        for (size_t i = 0; i < SystemTexture::Get().SceneVoxels.size(); i++) {

            math::float3 voxelSize = math::float3(SystemTexture::Get().SceneVoxels[i].areaExtend.x / (SystemTexture::Get().SceneVoxels[i].dimension),
                SystemTexture::Get().SceneVoxels[i].areaExtend.y / SystemTexture::Get().SceneVoxels[i].dimension,
                SystemTexture::Get().SceneVoxels[i].areaExtend.z/ SystemTexture::Get().SceneVoxels[i].dimension
            );
            sky->GetComponent<Transform>()->SetPosition(Math::ToVec3(SystemTexture::Get().SceneVoxels[i].center));
            sky->GetComponent<Transform>()->SetScale(Math::ToVec3(SystemTexture::Get().SceneVoxels[i].areaExtend - 2.0f * voxelSize)); //防止超出边界
            BindingSetDesc bindingSetDesc;
            bindingSetDesc.bindings = {
                BindingSetItem::ConstantBuffer(8, m_VoxelCB),
                //BindingSetItem::ConstantBuffer(8, m_ForwardLightCB),
                BindingSetItem::Texture_UAV(0, SystemTexture::Get().SceneVoxels[i].voxelTexture)
            };

            m_VoxelBindingSet = m_Device->createBindingSet(bindingSetDesc, m_VoxelBindingLayout);

        
            DrawItem drawItem = IDrawStrategy::ObjectToDrawItem(scene->GetSkyBox())[0];

            GraphicsState graphicsPSO;
            graphicsPSO.framebuffer = targetFramebuffer;
            graphicsPSO.viewport = viewState;// view->GetViewportState();

            GraphicsPipelineDesc psoDesc;
            psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
            psoDesc.depthStencilState.enableDepthTest();
            psoDesc.depthStencilState.disableDepthWrite();
            psoDesc.depthStencilState.disableStencil();

            psoDesc.blendState.alphaToCoverageEnable = false;
            psoDesc.rasterState.frontCounterClockwise = true;
            psoDesc.rasterState.cullMode = RasterCullMode::Back;
            psoDesc.primType = PrimitiveType::TriangleList;
            psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

            psoDesc.VS = m_VoxelizationSkyboxShader->GetVertexShader();
            psoDesc.GS = m_VoxelizationSkyboxShader->GetGeometryShader();
            psoDesc.PS = m_VoxelizationSkyboxShader->GetPixelShader();
            psoDesc.bFromPSOFileCache = false;
            psoDesc.bindingLayouts.push_back(m_VoxelBindingLayout);
            psoDesc.bindingLayouts.push_back(m_SkyboxBindingLayout);

            //psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


            if (!m_VoxelSkyboxPso) {
                m_VoxelSkyboxPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
            }
            graphicsPSO.pipeline = m_VoxelSkyboxPso;
            graphicsPSO.bindings.push_back(m_VoxelBindingSet);
            graphicsPSO.bindings.push_back(m_SkyboxBindingSet);

            // graphicsPSO.bindings.push_back(m_ViewBindingset);
            graphicsPSO.inputLayout = psoDesc.inputLayout;



            SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
            GE_ERROR_JUDGE();


            if (scene->GetLightSources()->GetParallelLights().empty()) {
                GE_CORE_ERROR("no direction light found in SkyPass");
                return;
            }
            VoxelConstants voxelConstants{};
            voxelConstants.center = SystemTexture::Get().SceneVoxels[i].center;
            voxelConstants.areaExtent = SystemTexture::Get().SceneVoxels[i].areaExtend;
            voxelConstants.dimension = SystemTexture::Get().SceneVoxels[i].dimension;



            cmdList->writeBuffer(m_VoxelCB, &voxelConstants, sizeof(VoxelConstants));


            double timeSecond = SystemTime::GetRuntimeFromStartMs() / 1000.0f;
            float currentTimeS = fmod(timeSecond, SkyboxRenderer::m_TotalTimeIntervalS);
            int state = int(currentTimeS / SkyboxRenderer::m_StateIntervalS);
            int nextState = state + 1;
            float stateFactor = nextState * SkyboxRenderer::m_StateIntervalS - currentTimeS;
            float nextStateFactor = currentTimeS - state * SkyboxRenderer::m_StateIntervalS;
            nextState %= 3;
            state %= 3;

            SkyConstants skyConstants{};
            skyConstants.factors[state] = stateFactor / SkyboxRenderer::m_StateIntervalS;
            skyConstants.factors[nextState] = nextStateFactor / SkyboxRenderer::m_StateIntervalS;
            skyConstants.factors[3 - state - nextState] = 0;


            cmdList->writeBuffer(m_SkyCB, &skyConstants, sizeof(SkyConstants));


            cmdList->setGraphicsState(graphicsPSO);

            Draw(cmdList, drawItem);
        }
       


        sky->GetComponent<Transform>()->SetPosition(oldPos);
        sky->GetComponent<Transform>()->SetScale(oldScale);
        cmdList->endMarker();
    }



	//void VoxelConeTracingRenderer::VoxelizeTest(const std::vector<Object*>& objs)
	//{


	//	glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
	//	glEnable(GL_CULL_FACE);
	//	glEnable(GL_DEPTH_TEST);
	//	glEnable(GL_BLEND);
	//	DrawObjects(objs);


	//	//Shader
	//	m_VoxelizationTestShader->Bind();


	//	// Settings.
	//	glViewport(120, 120, m_VoxelTextureSize, m_VoxelTextureSize);

	//	glDisable(GL_CULL_FACE);
	//	glDisable(GL_DEPTH_TEST);
	//	glDisable(GL_BLEND);

	//
	//	m_VoxelizationTestShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());

	//	
	//	DrawObjects(objs, m_VoxelizationTestShader);

	//




	//}


	

}