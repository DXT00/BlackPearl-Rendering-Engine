#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class IndirectDrawLayer :public BlackPearl::Layer {
public:

	IndirectDrawLayer(const std::string& name)
		: Layer(name)
	{

		BlackPearl::Renderer::Init();
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();
		m_BatchRenderer = DBG_NEW BlackPearl::BatchRenderer();
		m_IndirectRenderer = DBG_NEW BlackPearl::IndirectRenderer();
		m_IndirectRendererTest = DBG_NEW BlackPearl::IndirectRendererTest();

		m_Scene = DBG_NEW BlackPearl::Scene();

		float xOffset(-3.0f);
		float yOffset(-3.0f);
		// populate geometry
		for (unsigned int i = 0; i < 100; ++i)
		{
			for (unsigned int j = 0; j < 100; ++j)
			{
				auto cube = CreateCube();
				cube->GetComponent<BlackPearl::Transform>()->SetPosition(glm::vec3(xOffset, yOffset, 0.0));
				m_Scene->AddObject(cube);

				xOffset += 3.0f;
			}
			yOffset += 3.0f;// 0.2f;
			xOffset = -3.0f;// -0.95f;
		}
		/*auto cube = CreateCube();
		auto cube1 = CreateCube();
		auto cube2 = CreateCube();

		cube1->GetComponent<BlackPearl::Transform>()->SetPosition(glm::vec3(2.0,0.0,0.0));
		cube2->GetComponent<BlackPearl::Transform>()->SetPosition(glm::vec3(2.0, 2.0, 0.0));*/

		const std::string path = "assets/shaders/batch/Batch.glsl";
		m_BatchShader.reset(DBG_NEW BlackPearl::Shader(path));
		m_TerrainShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/Terrain/Terrain.glsl"));
		m_IndirectShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/IndirectDraw/Indirect.glsl"));

		//objs = LoadCubesScene1(6, glm::vec3(40, 0, 0));
		//objs1 = LoadCubesScene1(6, glm::vec3(0, 0, -40));
		//objs2 = LoadCubesScene1(6, glm::vec3(0, 0, 0));
		//objs3 = LoadCubesScene1(6, glm::vec3(-40, 0, 0));
		//objs4 = LoadCubesScene1(6, glm::vec3(40, 0, -40));
		//objs5 = LoadCubesScene1(6, glm::vec3(-40, 0, -40));
		//objs6 = LoadCubesScene1(6, glm::vec3(40, 0, -80));
		//objs7 = LoadCubesScene1(6, glm::vec3(-40, 0, -80));
		//objs8 = LoadCubesScene1(6, glm::vec3(0, 0, -80));

		//m_BatchNode = CreateBatchNode(objs, false);
		//m_BatchNode1 = CreateBatchNode(objs1, false);
		//m_BatchNode2 = CreateBatchNode(objs2, false);
		//m_BatchNode3 = CreateBatchNode(objs3, false);
		//m_BatchNode4 = CreateBatchNode(objs4, false);
		//m_BatchNode5 = CreateBatchNode(objs5, false);
		//m_BatchNode6 = CreateBatchNode(objs6, false);
		//m_BatchNode7 = CreateBatchNode(objs7, false);
		//m_BatchNode8 = CreateBatchNode(objs8, false);

	/*	m_Scene->AddNode(m_BatchNode);
		m_Scene->AddNode(m_BatchNode1);
		m_Scene->AddNode(m_BatchNode2);
		m_Scene->AddNode(m_BatchNode3);
		m_Scene->AddNode(m_BatchNode4);
		m_Scene->AddNode(m_BatchNode5);
		m_Scene->AddNode(m_BatchNode6);
		m_Scene->AddNode(m_BatchNode7);
		m_Scene->AddNode(m_BatchNode8);*/
		//cube->GetComponent<BlackPearl::Transform>()->SetPosition(glm::vec3(2.0, 0.0, 10.0));

		/*m_Scene->AddObject(cube);
		m_Scene->AddObject(cube1);
		m_Scene->AddObject(cube2);*/

		m_Terrain = CreateTerrain("assets/texture/iceland_heightmap.png");

		m_IndirectRenderer->Init(m_Scene, m_IndirectShader);
		//m_IndirectRendererTest->Init(m_Scene, m_IndirectShader);
		m_MainCamera->SetPosition({ 4.0,6.0,36.0 });
		m_MainCamera->SetMoveSpeed(20.0f);
		m_MainCamera->SetRotateSpeed(5.0f);
		m_MainCamera->SetZFar(2000);
		m_CameraPosition = m_MainCamera->GetPosition();

		int maxVertUniformsVect;
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertUniformsVect);

		GE_CORE_INFO("maxVertUniformsVect = {0}", maxVertUniformsVect);

		m_IndirectShader->Bind();

		m_IndirectShader->SetUniformMat4f("u_ProjectionView", m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->GetProjectionViewMatrix());
		//m_IndirectShader->SetUniformMat4f("u_Projection", m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->GetProjectionMatrix());
		//m_IndirectShader->SetUniformMat4f("u_View", m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->GetViewMatrix());

		////m_IndirectShader->SetUniformMat4f("u_Model", model);
		//m_IndirectShader->SetUniformVec3f("u_CameraViewPos", m_MainCamera->GetPosition());

	}

	virtual ~IndirectDrawLayer() {

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
		//m_IndirectShader->Bind();
		/*m_IndirectShader->Bind();

		m_IndirectShader->SetUniformMat4f("u_ProjectionView", m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->GetProjectionViewMatrix());
		m_IndirectShader->SetUniformMat4f("u_Projection", m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->GetProjectionMatrix());
		m_IndirectShader->SetUniformMat4f("u_View", m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->GetViewMatrix());

		m_IndirectShader->SetUniformMat4f("u_Model", model);
		m_IndirectShader->SetUniformVec3f("u_CameraViewPos", m_MainCamera->GetPosition());*/

		//m_BatchRenderer->Render(m_BatchNode, m_BatchShader);
		//m_BatchRenderer->Render(m_BatchNode1, m_BatchShader);
		//m_BatchRenderer->Render(m_BatchNode2, m_BatchShader);
		//m_BatchRenderer->Render(m_BatchNode3, m_BatchShader);
		//m_BatchRenderer->Render(m_BatchNode4, m_BatchShader);
		//m_BatchRenderer->Render(m_BatchNode5, m_BatchShader);
		//m_BatchRenderer->Render(m_BatchNode6, m_BatchShader);
		//m_BatchRenderer->Render(m_BatchNode7, m_BatchShader);
		//m_BatchRenderer->Render(m_BatchNode8, m_BatchShader);
		//m_IndirectShader->SetUniformMat4f("u_ProjectionView", m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->GetProjectionViewMatrix());

		m_IndirectRenderer->Render(m_IndirectShader);
		//m_IndirectRendererTest->Render(m_IndirectShader);

		//m_BasicRenderer->DrawTerrain(m_Terrain, m_TerrainShader, false);
		//m_IndirectShader->Unbind();

	}


	void OnAttach() override {


	}

private:

	std::vector<BlackPearl::Object*> m_LightObjs;

	BlackPearl::Object* m_SkyBoxObj;
	BlackPearl::Object* m_Terrain;
	BlackPearl::Scene* m_Scene;

	std::shared_ptr<BlackPearl::Shader> m_BatchShader;
	std::shared_ptr<BlackPearl::Shader> m_TerrainShader;
	std::shared_ptr<BlackPearl::Shader> m_IndirectShader;

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
	BlackPearl::BatchRenderer* m_BatchRenderer;
	BlackPearl::IndirectRenderer* m_IndirectRenderer;
	BlackPearl::IndirectRendererTest* m_IndirectRendererTest;


};
#pragma once
