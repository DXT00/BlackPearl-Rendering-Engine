
#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class VkRayTracingLayer :public BlackPearl::Layer {
public:

	VkRayTracingLayer(const std::string& name)
		: Layer(name)
	{
	
		mScene = new BlackPearl::RayTraceScene();

		BlackPearl::Object* bunny = CreateModel("assets/models/bunny/bunny.obj", "", false/*isAnimated*/, "Bunny", true/*vertices sorted*/);
		bunny->GetComponent<BlackPearl::Transform>()->SetInitScale(glm::vec3(1));
		bunny->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 0.0f,-2.0f,-2.0f });
		bunny->GetComponent<BlackPearl::Transform>()->SetInitRotation({ 0.0f,0.0f,0.0f });
		bunny->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		std::shared_ptr<BlackPearl::Material> bunny_mat;
		bunny_mat.reset(DBG_NEW BlackPearl::Material());
		bunny_mat->SetRTXType(BlackPearl::Material::RTXType::RTX_DIFFUSE);
		bunny_mat->SetId(0);

		std::vector<BlackPearl::Vertex> bunny_verteices = bunny->GetComponent<BlackPearl::MeshRenderer>()->GetModel()->GetMeshVertex();
		std::vector<BlackPearl::Vertex> bunny_verteices_trans;
		glm::mat4 transM = bunny->GetComponent<BlackPearl::Transform>()->GetTransformMatrix();
		for (size_t i = 0; i < bunny_verteices.size(); i++)
		{
			BlackPearl::Vertex v = bunny_verteices[i];
			
			v.position = transM * glm::vec4(v.position,1.0);
			bunny_verteices_trans.push_back(v);
		}

		BlackPearl::Object* bunny_bvh_node = BlackPearl::g_objectManager->CreateBVHNode(bunny_verteices_trans);
	//	BlackPearl::Object* bunny_bvh_node = BlackPearl::g_objectManager->CreateBVHNode(bunny_verteices);
		glm::vec3 extent = bunny_bvh_node->GetComponent<class BlackPearl::BVHNode>()->GetRootBox().GetExtent();
		//BlackPearl::Object* bunnyRTXTransformNode = BlackPearl::g_objectManager->CreateRTXTransformNode(bunny->GetComponent<BlackPearl::Transform>()->GetTransformMatrix(), bunny_bvh_node, bunny_mat);
		//LoadScene("CubesScene");
		bunny_bvh_node->AddComponent<BlackPearl::MeshRenderer>(bunny_mat);

		mScene->AddObject(bunny_bvh_node);

		BlackPearl::Object* areaLight = CreateModel("assets/models/light/light.obj", "", false/*isAnimated*/, "AreaLight", true/*vertices sorted*/);
		areaLight->GetComponent<BlackPearl::Transform>()->SetInitScale(glm::vec3(1.0));
		areaLight->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 0.0f,0.0f,-2.0f });
		areaLight->GetComponent<BlackPearl::Transform>()->SetInitRotation({ 0.0f,0.0f,0.0f });
		areaLight->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		std::shared_ptr<BlackPearl::Material> areaLight_mat;
		areaLight_mat.reset(DBG_NEW BlackPearl::Material());
		areaLight_mat->SetRTXType(BlackPearl::Material::RTXType::RTX_EMISSION);
		areaLight_mat->SetId(1);

		std::vector<BlackPearl::Vertex> light_verteices = areaLight->GetComponent<BlackPearl::MeshRenderer>()->GetModel()->GetMeshVertex();
		BlackPearl::Object* light_bvh_node = BlackPearl::g_objectManager->CreateBVHNode(light_verteices);
		//BlackPearl::Object* lightRTXTransformNode = BlackPearl::g_objectManager->CreateRTXTransformNode(bunny->GetComponent<BlackPearl::Transform>()->GetTransformMatrix(), light_bvh_node, areaLight_mat);
		light_bvh_node->AddComponent<BlackPearl::MeshRenderer>(areaLight_mat);
		mScene->AddObject(light_bvh_node);
		mScene->BuildRayTraceData();


		m_VkRTRender = DBG_NEW BlackPearl::VkRayTracingRenderer();

		m_VkRTRender->Init(mScene);
		m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetPositionAndUpdateMatrix(glm::vec3(0, 0, -25));
	}

	virtual ~VkRayTracingLayer() {
		DestroyObjects();
	}
	void OnUpdate(BlackPearl::Timestep ts) override {
		InputCheck(ts);

		//BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor1);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		m_VkRTRender->Render(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>());
		//m_VkBasicRender->DrawObjects(m_BackGroundObjsList);

	}

	void OnAttach() override {

	}

private:
	BlackPearl::RayTraceScene* mScene;
	glm::vec4 m_BackgroundColor1 = { 1.0f,1.0f,1.0f,1.0f };
	BlackPearl::VkRayTracingRenderer* m_VkRTRender;


};
#pragma once
