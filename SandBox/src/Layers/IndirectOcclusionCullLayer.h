#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class IndirectOcclusionCullLayer :public BlackPearl::Layer {
public:

	IndirectOcclusionCullLayer(const std::string& name)
		: Layer(name)
	{

		BlackPearl::Renderer::Init();
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();
		m_BatchRenderer = DBG_NEW BlackPearl::BatchRenderer();
		m_IndirectCullRenderer = DBG_NEW BlackPearl::IndirectCullRenderer();
		m_Scene = DBG_NEW BlackPearl::Scene();
		m_PonitLight = CreateLight(BlackPearl::LightType::PointLight);
		m_Quad = CreateQuad();
		float xOffset(-3.0f);
		float yOffset(-3.0f);
		// populate geometry
		int row = 5;
		int col = 5;
		int k = 0;
		m_Bunnys.resize(row * col);
		for (unsigned int i = 0; i < row; ++i)
		{
			for (unsigned int j = 0; j < col; ++j)
			{
				m_Bunnys[k] = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny", false);// CreateCube();
				m_Bunnys[k]->GetComponent<BlackPearl::Transform>()->SetPosition(glm::vec3(xOffset, 0.0, yOffset));
				m_Bunnys[k]->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(2.0f));
				m_Bunnys[k]->GetComponent<BlackPearl::Transform>()->SetRotation({ 0,180.0,0 });
				m_Scene->AddObject(m_Bunnys[k]);

				xOffset += 3.0f;
				k++;
			}
			yOffset += 3.0f;// 0.2f;
			xOffset = -3.0f;// -0.95f;
		}

		m_Bunny = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny");// CreateCube();
		const std::string path = "assets/shaders/batch/Batch.glsl";
		m_BatchShader.reset(DBG_NEW BlackPearl::Shader(path));
		m_TerrainShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/Terrain/Terrain.glsl"));
		m_IndirectCullShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/occlusionCulling/IndirectCull.glsl"));

	
		m_Scene->UpdateObjsAABB();
		m_Terrain = CreateTerrain("assets/texture/iceland_heightmap.png");

		m_IndirectCullRenderer->Init(m_Scene);
		m_MainCamera->SetPosition({ 0.0,0.0,0.0 });
		m_MainCamera->SetMoveSpeed(20.0f);
		m_MainCamera->SetRotateSpeed(5.0f);
		m_MainCamera->SetZFar(2000);
		m_CameraPosition = m_MainCamera->GetPosition();

		int maxVertUniformsVect;
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertUniformsVect);
		GE_CORE_INFO("maxVertUniformsVect = {0}", maxVertUniformsVect);


	}

	virtual ~IndirectOcclusionCullLayer() {

		DestroyObjects();
		GE_SAVE_DELETE(m_BasicRenderer);
		GE_SAVE_DELETE(m_BatchRenderer);
		GE_SAVE_DELETE(m_IndirectCullRenderer);
		GE_SAVE_DELETE(m_Scene);


	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());


		
		//m_BasicRenderer->DrawTerrain(m_Terrain, m_TerrainShader, false);

		m_IndirectCullRenderer->RenderDepthMap(m_Quad);
		m_IndirectCullRenderer->Render(m_IndirectCullShader, m_MainCamera);

	}


	void OnAttach() override {


	}

private:

	std::vector<BlackPearl::Object*> m_LightObjs;

	BlackPearl::Object* m_Terrain;
	BlackPearl::Scene* m_Scene;
	BlackPearl::Object* m_PonitLight;

	std::shared_ptr<BlackPearl::Shader> m_BatchShader;
	std::shared_ptr<BlackPearl::Shader> m_TerrainShader;
	std::shared_ptr<BlackPearl::Shader> m_IndirectCullShader;



	BlackPearl::BasicRenderer* m_BasicRenderer;
	BlackPearl::BatchRenderer* m_BatchRenderer;
	BlackPearl::IndirectCullRenderer* m_IndirectCullRenderer;

	BlackPearl::Object* m_Bunny;
	BlackPearl::Object* m_Quad;

	std::vector<BlackPearl::Object*> m_Bunnys;

};
#pragma once
