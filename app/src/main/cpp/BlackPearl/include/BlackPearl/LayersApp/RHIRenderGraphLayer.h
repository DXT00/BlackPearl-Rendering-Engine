#pragma once
//#include <BlackPearl.h>
#include "BlackPearl/Renderer/RenderTargets.h"
#include "BlackPearl/Renderer/RenderGraph/RenderGraph.h"
#include "BlackPearl/Renderer/RenderGraph/ForwardRenderGraph.h"
#include "BlackPearl/Renderer/RenderGraph/DeferredRenderGraph.h"
#include "BlackPearl/Renderer/RenderGraph/IBLProbeGraph.h"
#include "BlackPearl/Renderer/RenderGraph/SDFBakeGraph.h"

#include "BlackPearl/LayerScene/Layer.h"
#include "Component/LightComponent/DirectionLight.h"
#include "Component/LightComponent/PointLight.h"
#include "RHI/RHIGlobals.h"
#include "Config.h"
#include <Component/LightComponent/DirectionLight.h>


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

        m_Scene = DBG_NEW Scene();
        m_SphereObj = CreateSphere(0.5, 64, 64, "assets/shaders/glsl/Cube.glsl", "assets/texture/wood.png");
        m_CubeObj = CreateCube("assets/shaders/glsl/Cube.glsl", "assets/texture/wood.png");
        m_SkyBox = CreateSkyBox(
            { "assets/skybox/skybox/right.jpg",
             "assets/skybox/skybox/left.jpg",
             "assets/skybox/skybox/top.jpg",
             "assets/skybox/skybox/bottom.jpg",
             "assets/skybox/skybox/front.jpg",
             "assets/skybox/skybox/back.jpg",
            });
        m_CubeObj->GetComponent<Transform>()->SetScale({ 0.2,0.2,0.2 });
        m_CubeObj->GetComponent<Transform>()->SetRotation({ 0,30,0 });
        m_SphereObj->GetComponent<Transform>()->SetScale({ 0.5,0.5,0.5 });
        m_CubeObj->SetPosition({ 0.0,0.0,-2.0 });
        m_SphereObj->SetPosition({ -0.4,0.0,-2.0 });


        m_MainCamera->SetMoveSpeed(0.5f);
        m_MainCamera->SetRotateSpeed(5.0f);

        m_DirectionLight = CreateLight(LightType::DirectionLight, "DirectionLight");

#ifdef GE_PLATFORM_ANDROID
        // if(RenderGraph::SupportPLS())
        m_DirectionLight->GetComponent<DirectionLight>()->SetDirection({ 0.2f, -1.0f, 0.2f });
        //else
        //    m_DirectionLight->GetComponent<DirectionLight>()->SetDirection({ -0.2f, 1.0f, -0.2f });
#elif defined(GE_PLATFORM_WINDOWS)
        m_DirectionLight->GetComponent<DirectionLight>()->SetDirection({ -0.2f, 1.0f, -0.2f });
      //  m_DirectionLight->GetComponent<DirectionLight>()->SetDirection({ 0.2f, -1.0f, 0.2f });
#endif
        //m_Scene->AddObject(m_SphereObj);
        m_Scene->AddObject(m_CubeObj);
        m_Scene->AddObject(m_SphereObj);

        m_Scene->SetLightSources(GetLightSources());
        m_Scene->SetSkyBox(m_SkyBox);


        if (Configuration::bUseIBL) {
        
            m_MapManager = DBG_NEW MapManager(Configuration::MapSize, Configuration::AreaSize);

            m_DiffuseLightProbeGrid = CreateProbeGrid(m_MapManager, ProbeType::DIFFUSE_PROBE,
                math::float3(2, 2, 1), math::float3(0.0f, 1.0f, -0.2f), 5);

          /*  m_ReflectLightProbeGrid = CreateProbeGrid(m_MapManager, ProbeType::REFLECTION_PROBE,
                math::float3(2, 1, 1), math::float3(0.2f, -1.0f, 0.2f), 6);*/
        
            m_Scene->SetDiffuseLightProbes(m_DiffuseLightProbes);
            //m_Scene->SetReflectLightProbes(m_ReflectionLightProbes);
        }


    }

	void OnSetup() override {
        
        SetupScene();

        
        m_IBLRenderGraph = DBG_NEW IBLProbeGraph(m_DeviceManager);
        m_SDFBakeGraph = DBG_NEW SDFBakeGraph(m_DeviceManager);

        if (Configuration::bDeferredShading) {
            m_RenderGraph = DBG_NEW DeferredRenderGraph(m_DeviceManager);
        }
        else {
            m_RenderGraph = DBG_NEW ForwardRenderGraph(m_DeviceManager);
        }

		
        if(Configuration::bUseIBL){
            m_IBLRenderGraph->Init(m_Scene);
            m_DeviceManager->AddRenderGraphToBack(m_IBLRenderGraph);
        }
        if (Configuration::bUseSDF) {
            m_SDFBakeGraph->Init(m_Scene);
            m_DeviceManager->AddRenderGraphToBack(m_SDFBakeGraph);
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
    RenderGraph* m_IBLRenderGraph;
    RenderGraph* m_SDFBakeGraph;

    //Scene
    Scene* m_Scene;
	Object* m_CubeObj;
	Object* m_SphereObj;
	Object* m_SkyBox;
	Object* m_DirectionLight;


    //IBL Light probe
    MapManager* m_MapManager; // light probe Map
    Object* m_DiffuseLightProbeGrid;
    Object* m_ReflectLightProbeGrid;


};
#pragma once
