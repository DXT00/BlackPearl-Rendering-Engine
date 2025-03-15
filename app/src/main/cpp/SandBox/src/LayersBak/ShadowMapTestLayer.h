#pragma once
#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
const GLuint SCR_WIDTH = 960, SCR_HEIGHT = 540;
GLuint quadVAO = 0;
GLuint quadVBO = 0;
GLuint cubeVAO = 0;
GLuint cubeVBO = 0;
class ShadowMapTestLayer :public BlackPearl::Layer {

public:

	ShadowMapTestLayer(const std::string& name)
		: Layer(name)
	{

	/*	m_CameraObj = CreateCamera();
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		m_CameraPosition = cameraComponent->GetPosition();
		m_CameraRotation.Yaw = cameraComponent->Yaw();
		m_CameraRotation.Pitch = cameraComponent->Pitch();*/

		m_MasterRenderer = DBG_NEW BlackPearl::MasterRenderer(m_MainCamera->GetObj());

		BlackPearl::Renderer::Init();

		//	m_FrameBuffer.reset(DBG_NEW BlackPearl::FrameBuffer(960, 540, {BlackPearl::FrameBuffer::Attachment::DepthTexture },false));

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		// 设置OpenGL可选项
		glEnable(GL_DEPTH_TEST); // 开启深度测试

			//Shader reset
		m_Shader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/shadowMap/directLight/ShadowMaping_ShadowMapLayer.glsl"));
		m_QuadDepthShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/shadowMap/directLight/Quad_ShadowMapLayer.glsl"));


		m_Sun = CreateLight(BlackPearl::LightType::ParallelLight);
		

		BlackPearl::Object* Plane = CreatePlane();
		Plane->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_Shader);
		Plane->GetComponent<BlackPearl::Transform>()->SetScale({ 25.0f, 0.5f, 25.0f });

		BlackPearl::Object* Cube = CreateCube();
		Cube->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_Shader);
		Cube->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f, 1.5f, 0.0f });

		BlackPearl::Object* Cube1 = CreateCube();
		Cube1->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_Shader);
		Cube1->GetComponent<BlackPearl::Transform>()->SetPosition({ 2.0f, 0.0f, 1.0f });


		BlackPearl::Object* Cube2 = CreateCube();
		Cube2->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_Shader);
		Cube2->GetComponent<BlackPearl::Transform>()->SetPosition({ -1.0f, 0.0f, 2.0 });

		Quad = CreateQuad();
		Quad->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_MasterRenderer->GetShadowMapRenderer().GetFrameBuffer()->GetDepthTexture());
		Quad->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_QuadDepthShader);
		
	

		for (BlackPearl::Object* obj : m_ObjectsList) {
			if(obj->HasComponent<BlackPearl::MeshRenderer>())
			obj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_MasterRenderer->GetShadowMapRenderer().GetFrameBuffer()->GetDepthTexture());
		}
	
		

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);



		




	}

	virtual ~ShadowMapTestLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		//// render

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		for (BlackPearl::Object* obj : m_ObjectsList) {
			if (obj->HasComponent<BlackPearl::MeshRenderer>())
				obj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_MasterRenderer->GetShadowMapRenderer().GetFrameBuffer()->GetDepthTexture());
		}

		/*for (BlackPearl::Object* obj : m_ObjectsList) {
			if (obj!=Quad&&obj->HasComponent<BlackPearl::MeshRenderer>())
				obj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		}*/


		m_MasterRenderer->RenderShadowMap(m_ObjectsList, m_Sun->GetComponent<BlackPearl::ParallelLight>(), {Quad});
		// 2. Render scene as normal 
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_Shader->Bind();
		m_Shader->SetUniformVec3f("u_LightPos", m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection());
		m_Shader->SetUniformMat4f("u_LightProjectionViewMatrix", m_MasterRenderer->GetShadowMapLightProjectionMatrx());
		for (BlackPearl::Object* obj : m_ObjectsList) {
			if (obj != Quad && obj->HasComponent<BlackPearl::MeshRenderer>())
			obj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_Shader);
		}
		
		m_MasterRenderer->RenderSceneExcept(m_ObjectsList, Quad, GetLightSources() );

		glViewport(0, 0, 240, 135);
		Quad->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->GetShader()->Bind();
		Quad->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->GetShader()->SetUniformVec3f("u_LightPos", m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection());
		Quad->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->GetShader()->SetUniformMat4f("u_LightProjectionViewMatrix", m_MasterRenderer->GetShadowMapLightProjectionMatrx());


		m_MasterRenderer->RenderObject(Quad);


	}


	
	void OnImguiRender() override {


		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Suqare Color", glm::value_ptr(m_BackgroundColor));
		ImGui::End();

		static BlackPearl::Object* currentObj = nullptr;//TODO::注意内存泄漏

		if (ImGui::CollapsingHeader("Create")) {

			const char* const entityItems[] = { "Empty","ParallelLight","PointLight","SpotLight","IronMan","BB8","Cube","Plane" };
			static int entityIdx = -1;
			if (ImGui::Combo("CreateEntity", &entityIdx, entityItems, 8))
			{
				switch (entityIdx)
				{
				case 0:
					GE_CORE_INFO("Creating Empty...");
					Layer::CreateEmpty();
					break;
				case 1:
					GE_CORE_INFO("Creating PointLight...");
					Layer::CreateLight(BlackPearl::LightType::ParallelLight);
					break;
				case 2:
					GE_CORE_INFO("Creating PointLight...");
					Layer::CreateLight(BlackPearl::LightType::PointLight);
					break;
				case 3:
					GE_CORE_INFO("Creating SpotLight ...");
					Layer::CreateLight(BlackPearl::LightType::SpotLight);
					break;
				case 4:
					GE_CORE_INFO("Creating IronMan ...");
					Layer::CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/IronMan.glsl",false);
					break;
				case 5:
					GE_CORE_INFO("Creating BB8 ...");
					Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl",false);
					break;
				case 6:
					GE_CORE_INFO("Creating Cube ...");
					Layer::CreateCube();
					break;
				case 7:
					GE_CORE_INFO("Creating Plane ...");
					Layer::CreatePlane();
					break;
				}
			}
		}
		if (ImGui::BeginTabBar("TabBar 0", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Scene")) {
				std::vector<BlackPearl::Object*> objsList = GetObjects();		//TODO::
				ImGui::ListBoxHeader("CurrentEntities", (int)objsList.size(), 8);

				for (int n = 0; n < objsList.size(); n++) {
					//ImGui::Text("%s", objsList[n].c_str());
					bool is_selected = (currentObj != nullptr && currentObj->GetName() == objsList[n]->GetName());
					if (ImGui::Selectable(objsList[n]->GetName().c_str(), is_selected)) {
						currentObj = objsList[n];
						GE_CORE_INFO(objsList[n]->GetName() + "is selected")
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::ListBoxFooter();
				ImGui::EndTabItem();
			}
			//}
		}
		ImGui::EndTabBar();

		////////////////////Inspector/////////////////////////
		ImGui::Begin("Inspector");


		float pos[] = { m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection().x, m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection().y, m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection().z };
		ImGui::DragFloat3("m_LightPos", pos, 0.1f, -100.0f, 100.0f, "%.3f ");
		m_Sun->GetComponent<BlackPearl::ParallelLight>()->SetDirection({ pos[0],pos[1],pos[2] });

		
		/*ImGui::DragFloat("near_plane", &BlackPearl::ShadowMapRenderer::s_NearPlane, 0.5f, -50.0f, 100.0f, "%.3f ");
		ImGui::DragFloat("far_plane", &BlackPearl::ShadowMapRenderer::s_FarPlane, 0.5f, -50.0f, 100.0f, "%.3f ");*/
		ImGui::DragFloat("ShadowDistance", &BlackPearl::ShadowBox::s_ShadowDistance, 0.5f, 0.0f, 120.0f, "%.3f "); 

		if (currentObj != nullptr) {
			if (currentObj->HasComponent< BlackPearl::Transform>()) {
				ShowTransform(currentObj->GetComponent<BlackPearl::Transform>(), currentObj);

			}
			if (currentObj->HasComponent< BlackPearl::MeshRenderer>()) {
				ShowMeshRenderer(currentObj->GetComponent<BlackPearl::MeshRenderer>());

			}
			if (currentObj->HasComponent < BlackPearl::PointLight>()) {
				ShowPointLight(currentObj->GetComponent<BlackPearl::PointLight>());
			}
	/*		std::unordered_map<BlackPearl::BaseComponent::Family, std::shared_ptr<BlackPearl::BaseComponent>> componentList = currentObj->GetComponentList();

			for (auto pair : componentList) {
				auto component = pair.second;
				if (component != nullptr) {

					switch (component->GetType()) {
					case BlackPearl::BaseComponent::Type::MeshRenderer: {
						std::shared_ptr<BlackPearl::MeshRenderer> comp = std::dynamic_pointer_cast<BlackPearl::MeshRenderer>(component);
						ShowMeshRenderer(comp);
						break;
					}
					case BlackPearl::BaseComponent::Type::Transform: {
						std::shared_ptr<BlackPearl::Transform> comp = std::dynamic_pointer_cast<BlackPearl::Transform>(component);
						ShowTransform(comp);
						break;
					}
					case BlackPearl::BaseComponent::Type::Light: {
						std::shared_ptr<BlackPearl::Light> comp = std::dynamic_pointer_cast<BlackPearl::Light>(component);
						if (comp->GetType() == BlackPearl::LightType::PointLight)
							ShowPointLight(comp);
						break;
					}
					default:
						break;
					}

				}

			}*/
		}

		ImGui::End();



		m_fileDialog.Display();




	}
	void OnAttach() override {


	}
	
private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_Sun;

	BlackPearl::Object* Quad;
	BlackPearl::MasterRenderer *m_MasterRenderer;

	BlackPearl::Object* m_SkyBox;




	//BlackPearl::Object* m_IronMan;
//BlackPearl::Object* m_Quad;
//BlackPearl::Object* m_Plane;
//BlackPearl::Object* m_Cube;
//BlackPearl::Object* m_Cube1;
//BlackPearl::Object* m_Cube2;
	

	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };




	glm::vec3 m_LightPos = { 0.0f, 1.5f, 0.0f };
	std::shared_ptr<BlackPearl::Shader> m_Shader;
	std::shared_ptr<BlackPearl::Shader> m_QuadDepthShader;

	//std::shared_ptr<BlackPearl::Shader> m_SimpleDepthShader;

	GLuint woodTexture;
	GLuint planeVAO;
	GLuint depthMapFBO;
	GLuint depthMap;
	GLfloat near_plane = 1.0f, far_plane = 7.5f;
};
