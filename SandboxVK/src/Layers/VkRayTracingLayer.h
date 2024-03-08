
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
		m_MainCamera->SetRotateSpeed(0.1f);
		m_MainCamera->SetMoveSpeed(0.3f);
		m_MainCamera->SetPosition({0,0.0,0});

		m_MainCamera->SetFov(30.0f);


		LoadModel("assets/models/bunny/bunny.obj", "bunny", 0, BlackPearl::Material::RTXType::RTX_DIFFUSE, 0.5, { -0.2f,0.0f,-3.0f });

		LoadModel("assets/models/bunny/bunny.obj", "bunny1", 1, BlackPearl::Material::RTXType::RTX_DIFFUSE,0.5, { 0.2f,0.0f,-3.0f });

		//LoadModel("assets/models/doge_scene/back.obj", "back", 0, BlackPearl::Material::RTXType::RTX_DIFFUSE, 1.0, { 0.0f,-1.0f,-2.0f });
		//LoadModel("assets/models/doge_scene/box1.obj", "box1", 1, BlackPearl::Material::RTXType::RTX_DIFFUSE, 1.0);
		//LoadModel("assets/models/doge_scene/box2.obj", "box2", 1, BlackPearl::Material::RTXType::RTX_DIFFUSE, 1.0);
		//LoadModel("assets/models/doge_scene/cheems.obj", "cheems", 0, BlackPearl::Material::RTXType::RTX_DIFFUSE, 1.0, { 0.0f,-1.0f,-2.0f });
		//LoadModel("assets/models/doge_scene/floor.obj", "floor", 0, BlackPearl::Material::RTXType::RTX_DIFFUSE, 1.0, { 0.0f,-1.0f,-2.0f });
		//LoadModel("assets/models/doge_scene/left.obj", "left", 1, BlackPearl::Material::RTXType::RTX_DIFFUSE, 1.0, { 0.0f,-1.0f,-2.0f });
		//LoadModel("assets/models/doge_scene/right.obj", "right", 1, BlackPearl::Material::RTXType::RTX_DIFFUSE, 1.0, { 0.0f,-1.0f,-2.0f });




		BlackPearl::Object* areaLight = CreateModel("assets/models/light/light.obj", "", false/*isAnimated*/, "AreaLight", true/*vertices sorted*/);
		areaLight->GetComponent<BlackPearl::Transform>()->SetInitScale(glm::vec3(1.0));
		areaLight->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 0.0f,0.0f,-2.0f });
		areaLight->GetComponent<BlackPearl::Transform>()->SetInitRotation({ 0.0f,0.0f,0.0f });
		areaLight->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		std::shared_ptr<BlackPearl::Material> areaLight_mat;
		areaLight_mat.reset(DBG_NEW BlackPearl::Material());
		areaLight_mat->SetRTXType(BlackPearl::Material::RTXType::RTX_EMISSION);
		areaLight_mat->SetId(2);

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

	void LoadModel(std::string objPath, std::string name, int matId, BlackPearl::Material::RTXType matType, float scale, glm::vec3 pos) {
		BlackPearl::Object* doge = CreateModel(objPath, "", false/*isAnimated*/, name, true/*vertices sorted*/);
		doge->GetComponent<BlackPearl::Transform>()->SetInitScale(glm::vec3(scale));
		doge->GetComponent<BlackPearl::Transform>()->SetInitPosition(pos);

		doge->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		std::shared_ptr<BlackPearl::Material> doge_mat;
		doge_mat.reset(DBG_NEW BlackPearl::Material());
		doge_mat->SetRTXType(matType);
		doge_mat->SetId(matId);

		std::vector<BlackPearl::Vertex> doge_verteices = doge->GetComponent<BlackPearl::MeshRenderer>()->GetModel()->GetMeshVertex();
		std::vector<BlackPearl::Vertex> doge_verteices_trans;
		glm::mat4 transM = doge->GetComponent<BlackPearl::Transform>()->GetTransformMatrix();
		for (size_t i = 0; i < doge_verteices.size(); i++)
		{
			BlackPearl::Vertex v = doge_verteices[i];

			v.position = transM * glm::vec4(v.position, 1.0);
			doge_verteices_trans.push_back(v);
		}

		BlackPearl::Object* doge_bvh_node = BlackPearl::g_objectManager->CreateBVHNode(doge_verteices_trans);
		doge_bvh_node->AddComponent<BlackPearl::MeshRenderer>(doge_mat);
		mScene->AddObject(doge_bvh_node);

	}

	void OnAttach() override {

	}

private:
	BlackPearl::RayTraceScene* mScene;
	glm::vec4 m_BackgroundColor1 = { 1.0f,1.0f,1.0f,1.0f };
	BlackPearl::VkRayTracingRenderer* m_VkRTRender;


};
#pragma once
