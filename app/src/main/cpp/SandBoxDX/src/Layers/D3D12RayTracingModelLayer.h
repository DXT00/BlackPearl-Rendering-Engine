#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>

using namespace DX;

class D3D12RayTracingModelLayer :public BlackPearl::Layer {

public:

	D3D12RayTracingModelLayer(const std::string& name)
		: Layer(name)
	{

		m_D3D12RayTracingModelRenderer = DBG_NEW BlackPearl::D3D12RayTracingModelRenderer();

		BlackPearl::Object* light = CreateLight(BlackPearl::LightType::PointLight);
		light->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f, 18.0f, -20.0f });

		m_MetaBallScene = DBG_NEW BlackPearl::MetaBallSDFScene();

		m_D3D12RayTracingModelRenderer->InitScene(m_MetaBallScene);
		m_D3D12RayTracingModelRenderer->Init();
		/***************************************** Scene ********************************************************/

		//Group
#if 0
		{
			m_Sphere1 = CreateSphere(1.0f, 64, 64);
			m_Sphere2 = CreateSphere(1.0f, 64, 64);
			m_Sphere3 = CreateSphere(1.0f, 64, 64);
			m_Sphere4 = CreateSphere(1.0f, 64, 64);
			m_Sphere5 = CreateSphere(8.0f, 64, 64);

			m_Sphere1->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0.6,0.7,BlackPearl::Math::Rand_F() });
			m_Sphere2->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0.9,0.9,0 });
			m_Sphere3->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ BlackPearl::Math::Rand_F(),BlackPearl::Math::Rand_F(),BlackPearl::Math::Rand_F() });
			m_Sphere4->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0.9,0,0.26 });
			m_Sphere5->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ BlackPearl::Math::Rand_F(),BlackPearl::Math::Rand_F(),0.8 });

			m_Sphere1->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 1.0,0.0,0.0 });
			m_Sphere2->GetComponent<BlackPearl::Transform>()->SetInitPosition({ -1.0,0.0,0.0 });
			m_Sphere3->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 3.0,0.0,0.0 });
			m_Sphere4->GetComponent<BlackPearl::Transform>()->SetInitPosition({ -3.0,0.0,0.0 });
			m_Sphere5->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 0.0,-9.0,0.0 });

			group_obj = BlackPearl::Layer::CreateGroup();
			m_Group = DBG_NEW BlackPearl::Group(group_obj);

			m_Group->PushBack(m_Sphere1);
			m_Group->PushBack(m_Sphere2);
			m_Group->PushBack(m_Sphere3);
			m_Group->PushBack(m_Sphere4);
			m_Group->PushBack(m_Sphere5);

			m_SceneBuilder->CreateSceneData(m_Group);
			m_RayTracingRenderer->InitScene(m_SceneBuilder->GetScene());
		}

#endif
#if 0
		//BVHNode
		{
			m_Sphere1 = CreateSphere(5, 64, 64);
			//	m_Sphere1->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0.6,0.7,BlackPearl::Math::Rand_F() });
			m_Sphere1->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 0.0,0.0,0.0 });


			group_obj = BlackPearl::Layer::CreateGroup();
			m_Group = DBG_NEW BlackPearl::Group(group_obj);


			//std::vector<BlackPearl::Vertex> bunny_verteices = m_Sphere1->GetComponent<BlackPearl::MeshRenderer>()->GetModel()->GetMeshVertex();
			BlackPearl::Object* bunny_bvh_node = BlackPearl::g_objectManager->CreateBVHNode({ m_Sphere1 });
			m_BunnyRTXTransformNode = BlackPearl::g_objectManager->CreateRTXTransformNode(m_Sphere1->GetComponent<BlackPearl::Transform>()->GetTransformMatrix(), bunny_bvh_node);

			//m_Group->PushBack(m_BunnyRTXTransformNode);
			m_Group->PushBack(bunny_bvh_node);

			m_SceneBuilder->CreateSceneData(m_Group);
			m_RayTracingRenderer->InitScene(m_SceneBuilder->GetScene());
		}
#endif


		m_MainCamera->SetFov(45.0f);
		m_MainCamera->SetZFar(125.0f);
		m_MainCamera->SetZNear(0.01f);
		//m_MainCamera->SetPosition(math::float3(0.0f, 5.0f, -17.0f));//math::float3(0,1.387f,22.012f)
		m_MainCamera->SetPosition(glm::vec3(0.0f, 2.0f, -18.0f));//math::float3(0,1.387f,22.012f)

		m_CameraPosition = m_MainCamera->GetPosition();
		//BlackPearl::Renderer::Init();


		/*******************************************************************************************************/


	}

	virtual ~D3D12RayTracingModelLayer() {

		GE_SAVE_DELETE(m_MetaBallScene);
		DestroyObjects();
		GE_SAVE_DELETE(m_D3D12RayTracingModelRenderer);

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);
		//InputCheck(1.0);
		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtimeSecond = (currentTimeMs.count() - m_StartTimeMs.count()) / 1000.0f;

		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		
		m_D3D12RayTracingModelRenderer->Render(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources(), runtimeSecond);



	}

	void OnAttach() override {


	}


private:

	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };

	BlackPearl::SDFScene* m_MetaBallScene;

	BlackPearl::D3D12RayTracingModelRenderer* m_D3D12RayTracingModelRenderer;



};
