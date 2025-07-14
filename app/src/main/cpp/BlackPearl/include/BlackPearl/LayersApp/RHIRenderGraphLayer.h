#pragma once
//#include <BlackPearl.h>
#include "BlackPearl/Renderer/RenderTargets.h"
#include "BlackPearl/Renderer/RenderGraph/RenderGraph.h"
#include "BlackPearl/Renderer/RenderGraph/ForwardRenderGraph.h"
#include "BlackPearl/Renderer/RenderGraph/DeferredRenderGraph.h"
#include "BlackPearl/Renderer/RenderGraph/IBLProbeGraph.h"
#include "BlackPearl/Renderer/RenderGraph/SDFGraph.h"
#include "BlackPearl/Renderer/RenderGraph/VoxelGraph.h"
#include "BlackPearl/Renderer/RenderGraph/DDGIGraph.h"

#include "BlackPearl/LayerScene/Layer.h"
#include "Component/LightComponent/DirectionLight.h"
#include "Component/LightComponent/PointLight.h"
#include "RHI/RHIGlobals.h"
#include "Config.h"
#include "Component/BoundingBoxComponent/BoundingBox.h"


class RHIRenderGraphLayer :public Layer {
public:

	RHIRenderGraphLayer(const std::string& name)
		: Layer(name)
	{
		
	}

	virtual ~RHIRenderGraphLayer() {

		DestroyObjects();

	}

    void SetupScene() {
        std::vector<std::string> macros;
       // macros.push_back("#define USE_ALBEDO_MAP 1");
        m_Scene = DBG_NEW Scene();
        // TODO:: 设置每个pass的材质
        m_SphereObj = CreateSphere(0.5, 64, 64, "assets/shaders/glsl/gBuffer/gBuffer_pass.glsl", &macros, "assets/texture/wood.png");
        m_CubeObj = CreateCube("assets/shaders/glsl/gBuffer/gBuffer_pass.glsl", &macros, "assets/texture/wood.png");


        //m_HouseModel = CreateFBXModel("assets/models/BurgerPiz/Models/BurgerPiz.fbx", "assets/shaders/glsl/Cube.glsl", false, "House");
     //   LoadChurchScene();

      // Object* model = LoadStaticBackGroundObject("Church", "assets/shaders/glsl/gBuffer/gBuffer_pass.glsl");
      // m_Scene->AddObject(model);
       LoadCornellScene1(m_Scene);
        m_SkyBox = CreateSkyBox(
            { "assets/skybox/skybox/right.jpg",
             "assets/skybox/skybox/left.jpg",
             "assets/skybox/skybox/top.jpg",
             "assets/skybox/skybox/bottom.jpg",
             "assets/skybox/skybox/front.jpg",
             "assets/skybox/skybox/back.jpg",
            });
        m_CubeObj->GetComponent<Transform>()->SetScale({ 5.0,5.0,5.0 });
       // m_CubeObj->GetComponent<Transform>()->SetRotation({ 0,30,0 });
        m_SphereObj->GetComponent<Transform>()->SetScale({ 10.0,10.0,10.0 });
        m_CubeObj->SetPosition({ 5.0, 0.0,0.0 });
        m_SphereObj->SetPosition({ -3.0,-0.0,-0.0 });

        m_SphereObj->GetComponent<BoundingBox>()->Get().UpdateTransform(m_SphereObj->GetComponent<Transform>()->GetTransformMatrix());
        m_CubeObj->GetComponent<BoundingBox>()->Get().UpdateTransform(m_CubeObj->GetComponent<Transform>()->GetTransformMatrix());
       
        
        m_MainCamera->SetMoveSpeed(1.0f);
        m_MainCamera->SetRotateSpeed(5.0f);

        m_DirectionLight = CreateLight(LightType::DirectionLight, "DirectionLight");

#ifdef GE_PLATFORM_ANDROID
        // if(RenderGraph::SupportPLS())
        m_DirectionLight->GetComponent<DirectionLight>()->SetDirection({ 0.2f, -1.0f, 0.2f });
        //else
        //    m_DirectionLight->GetComponent<DirectionLight>()->SetDirection({ -0.2f, 1.0f, -0.2f });
#elif defined(GE_PLATFORM_WINDOWS)
        m_DirectionLight->GetComponent<DirectionLight>()->SetDirection({ 0.2f, -1.0f, 0.2f });
      //  m_DirectionLight->GetComponent<DirectionLight>()->SetDirection({ 0.2f, -1.0f, 0.2f });
#endif
        //m_Scene->AddObject(m_SphereObj);
        //m_Scene->AddObject(m_CubeObj);
        m_Scene->AddObject(m_SphereObj);

        m_Scene->SetLightSources(GetLightSources());
        m_Scene->SetSkyBox(m_SkyBox);


        if (Configuration::bUseIndirectLight &&
            (Configuration::GIMethod == GIMethod::IBL 
                || Configuration::GIMethod == GIMethod::DDGI)) {
        

            m_DiffuseLightProbeGrid = CreateProbeGrid(ProbeType::DIFFUSE_PROBE,
                math::float3(3, 3, 3), math::float3(-10.0f, -2.5f, -3.0f), 10);

          /*  m_ReflectLightProbeGrid = CreateProbeGrid(m_MapManager, ProbeType::REFLECTION_PROBE,
                math::float3(2, 1, 1), math::float3(0.2f, -1.0f, 0.2f), 6);*/
        
            m_Scene->SetDiffuseLightProbes(m_DiffuseLightProbes);
            //m_Scene->SetReflectLightProbes(m_ReflectionLightProbes);


            m_Scene->AddLightProbeGrid(m_DiffuseLightProbeGrid);
        }
        auto gridPos = m_DiffuseLightProbeGrid->GridObj->GetComponent<Transform>()->GetPosition();

        m_CubeObj->SetPosition(gridPos);
    }

	void OnSetup() override {
        
        SetupScene();

        
        if (Configuration::bDeferredShading) {
            m_RenderGraph = DBG_NEW DeferredRenderGraph(m_DeviceManager);
        }
        else {
            m_RenderGraph = DBG_NEW ForwardRenderGraph(m_DeviceManager);
        }

        if (Configuration::bUseIndirectLight) {
            if (Configuration::GIMethod == GIMethod::IBL) {
                m_GIGraph = DBG_NEW IBLProbeGraph(m_DeviceManager);

                m_GIGraph->Init(m_Scene);
                m_DeviceManager->AddRenderGraphToBack(m_GIGraph);
            }
            else if (Configuration::GIMethod == GIMethod::DDGI) {
                m_VoxelGraph = DBG_NEW VoxelGraph(m_DeviceManager);
                m_VoxelGraph->Init(m_Scene);
                m_DeviceManager->AddRenderGraphToBack(m_VoxelGraph);

                m_SDFBakeGraph = DBG_NEW SDFGraph(m_DeviceManager);
                m_SDFBakeGraph->Init(m_Scene);
                m_DeviceManager->AddRenderGraphToBack(m_SDFBakeGraph);

             

                m_GIGraph = DBG_NEW DDGIGraph(m_DeviceManager);
                m_GIGraph->Init(m_Scene);
                m_DeviceManager->AddRenderGraphToBack(m_GIGraph);
            }
        }
       
      

        m_RenderGraph->Init(m_Scene);
        m_DeviceManager->AddRenderGraphToBack(m_RenderGraph);


	}

	void OnUpdate(Timestep ts) override {

		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtime = currentTimeMs.count() - m_StartTimeMs.count();

		InputCheck(ts);


		Renderer::BeginScene((m_MainCamera->GetObj()->GetComponent<PerspectiveCamera>()), *GetLightSources());
		//Update Camera, Materials ..
		//m_DeviceManager->UpdateWindowSize();

		m_DeviceManager->Run(ts);

		//m_RenderGraph->Render(m_DeviceManager->GetFrameBuffer(), Renderer::GetSceneData());
		
		
	}

    virtual void OnImguiRender() override {

        m_DeviceManager->RunUI();


    }

	void OnAttach() override {
	}

private:

    //Graph
	RenderGraph* m_RenderGraph;
    RenderGraph* m_GIGraph;
    RenderGraph* m_SDFBakeGraph;
    RenderGraph* m_VoxelGraph;

    //Scene
    Scene* m_Scene;
	Object* m_CubeObj;
	Object* m_SphereObj;

    Object* m_HouseModel;
	Object* m_SkyBox;
	Object* m_DirectionLight;


    //IBL Light probe
    MapManager* m_MapManager; // light probe Map
    LightProbeGrid* m_DiffuseLightProbeGrid;
    LightProbeGrid* m_ReflectLightProbeGrid;


};
#pragma once
