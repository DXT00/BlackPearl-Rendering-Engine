#pragma once
#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class BatchRenderingLayer :public BlackPearl::Layer {
public:

	BatchRenderingLayer(const std::string& name)
		: Layer(name)
	{

		BlackPearl::Renderer::Init();
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();

		const std::string path = "assets/shaders/batch/Batch.glsl";
		m_BatchShader.reset(DBG_NEW BlackPearl::Shader(path));

		objs = LoadCubesScene1(6, glm::vec3(40,0,0));
		objs1 = LoadCubesScene1(6, glm::vec3(0, 0, -40));
		objs2 = LoadCubesScene1(6, glm::vec3(0, 0, 0));
		objs3 = LoadCubesScene1(6, glm::vec3(-40, 0, 0));
		objs4 = LoadCubesScene1(6, glm::vec3(40, 0, -40));
		objs5 = LoadCubesScene1(6, glm::vec3(-40, 0, -40));
		objs6 = LoadCubesScene1(6, glm::vec3(40, 0, -80));
		objs7 = LoadCubesScene1(6, glm::vec3(-40, 0, -80));
		objs8 = LoadCubesScene1(6, glm::vec3(0, 0, -80));

		m_BatchNode = DBG_NEW BlackPearl::BatchNode(objs, false);
		m_BatchNode1 = DBG_NEW BlackPearl::BatchNode(objs1, false);
		m_BatchNode2 = DBG_NEW BlackPearl::BatchNode(objs2, false);
		m_BatchNode3 = DBG_NEW BlackPearl::BatchNode(objs3, false);
		m_BatchNode4 = DBG_NEW BlackPearl::BatchNode(objs4, false);
		m_BatchNode5 = DBG_NEW BlackPearl::BatchNode(objs5, false);
		m_BatchNode6 = DBG_NEW BlackPearl::BatchNode(objs6, false);
		m_BatchNode7 = DBG_NEW BlackPearl::BatchNode(objs7, false);
		m_BatchNode8 = DBG_NEW BlackPearl::BatchNode(objs8, false);

		m_MainCamera->SetPosition({ 4.0,6.0,36.0 });
		m_MainCamera->SetMoveSpeed(20.0f);
		m_MainCamera->SetRotateSpeed(5.0f);
		m_MainCamera->SetZFar(2000);
		m_CameraPosition = m_MainCamera->GetPosition();

		int maxVertUniformsVect;
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertUniformsVect);

		GE_CORE_INFO("maxVertUniformsVect = {0}", maxVertUniformsVect);


	}

	virtual ~BatchRenderingLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());


		//m_BasicRenderer->DrawObjects(objs);
		//m_BasicRenderer->DrawObjects(objs1);
		//m_BasicRenderer->DrawObjects(objs2);
		//m_BasicRenderer->DrawObjects(objs3);
		//m_BasicRenderer->DrawObjects(objs4);
		//m_BasicRenderer->DrawObjects(objs5);
		//m_BasicRenderer->DrawObjects(objs6);
		//m_BasicRenderer->DrawObjects(objs7);
		//m_BasicRenderer->DrawObjects(objs8);

		m_BasicRenderer->DrawBatchNode(m_BatchNode, m_BatchShader);
		m_BasicRenderer->DrawBatchNode(m_BatchNode1, m_BatchShader);
		m_BasicRenderer->DrawBatchNode(m_BatchNode2, m_BatchShader);
		m_BasicRenderer->DrawBatchNode(m_BatchNode3, m_BatchShader);
		m_BasicRenderer->DrawBatchNode(m_BatchNode4, m_BatchShader);
		m_BasicRenderer->DrawBatchNode(m_BatchNode5, m_BatchShader);
		m_BasicRenderer->DrawBatchNode(m_BatchNode6, m_BatchShader);
		m_BasicRenderer->DrawBatchNode(m_BatchNode7, m_BatchShader);
		m_BasicRenderer->DrawBatchNode(m_BatchNode8, m_BatchShader);


	}


	void OnAttach() override {


	}

private:

	std::vector<BlackPearl::Object*> m_LightObjs;

	BlackPearl::Object* m_SkyBoxObj;
	std::shared_ptr<BlackPearl::Shader> m_BatchShader;
	BlackPearl::BatchNode* m_BatchNode;
	BlackPearl::BatchNode* m_BatchNode1;
	BlackPearl::BatchNode* m_BatchNode2;
	BlackPearl::BatchNode* m_BatchNode3;
	BlackPearl::BatchNode* m_BatchNode4;
	BlackPearl::BatchNode* m_BatchNode5;
	BlackPearl::BatchNode* m_BatchNode6;
	BlackPearl::BatchNode* m_BatchNode7;
	BlackPearl::BatchNode* m_BatchNode8;

	std::vector<BlackPearl::Object*> objs;
	std::vector<BlackPearl::Object*> objs1;
	std::vector<BlackPearl::Object*> objs2;
	std::vector<BlackPearl::Object*> objs3; 
	std::vector<BlackPearl::Object*> objs4; 
	std::vector<BlackPearl::Object*> objs5;
	std::vector<BlackPearl::Object*> objs6;
	std::vector<BlackPearl::Object*> objs7;
	std::vector<BlackPearl::Object*> objs8;

	BlackPearl::BasicRenderer* m_BasicRenderer;

};
#pragma once
