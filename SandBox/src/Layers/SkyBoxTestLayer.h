#pragma once
#pragma once
#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class SkyBoxTestLayer :public BlackPearl::Layer {
public:

	SkyBoxTestLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
	{

		m_CameraObj = CreateCamera();
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		m_CameraPosition = cameraComponent->GetPosition();
		m_CameraRotation.Yaw = cameraComponent->Yaw();
		m_CameraRotation.Pitch = cameraComponent->Pitch();
		m_MasterRenderer = DBG_NEW BlackPearl::MasterRenderer(m_CameraObj);

		//m_FrameBuffer.reset(DBG_NEW BlackPearl::FrameBuffer(BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight));

		BlackPearl::Renderer::Init();

		m_SkyBoxObj = CreateSkyBox(
			{"assets/skybox/skybox/right.jpg",
			 "assets/skybox/skybox/left.jpg",
			 "assets/skybox/skybox/top.jpg",
			 "assets/skybox/skybox/bottom.jpg",
			 "assets/skybox/skybox/front.jpg",
			 "assets/skybox/skybox/back.jpg",
			});
		//m_ObjectsList.push_back(m_SkyBoxObj);
		//m_PlaneObj =CreatePlane();
		//m_QuadObj = CreateQuad();

		m_CubeObj = CreateCube("assets/shaders/CubeRefraction.glsl","");
		auto meshComponent = m_CubeObj->GetComponent<BlackPearl::MeshRenderer>();
		std::shared_ptr<BlackPearl::Texture> cubeMapTexture(DBG_NEW BlackPearl::CubeMapTexture(BlackPearl::Texture::Type::CubeMap, 
			{ "assets/skybox/skybox/right.jpg",
			 "assets/skybox/skybox/left.jpg",
			 "assets/skybox/skybox/top.jpg",
			 "assets/skybox/skybox/bottom.jpg",
			 "assets/skybox/skybox/front.jpg",
			 "assets/skybox/skybox/back.jpg",
			},GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE));
		meshComponent->SetTexture(0, cubeMapTexture);
	//	m_ObjectsList.push_back(m_CubeObj);

		//把FrameBuffer中的texture作为贴图，贴到m_CubeObj上
	//	auto CubemeshComponent = m_CubeObj->GetComponent<BlackPearl::MeshRenderer>();
		//CubemeshComponent->SetTexture(0, m_FrameBuffer->GetColorTexture());
		//	Layer::CreateLight(BlackPearl::LightType::PointLight);

		

		// (optional) set browser properties
		
		//m_fileDialog.SetTypeFilters({ ".h", ".cpp" });
	}

	virtual ~SkyBoxTestLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);
		/*for (BlackPearl::Object* obj : m_ObjectsList) {
			if (obj->HasComponent<BlackPearl::MeshRenderer>())
				obj->GetComponent<BlackPearl::Transform>()->SetRotation({
				obj->GetComponent<BlackPearl::Transform>()->GetRotation().x,
				 sin(glfwGetTime() * 0.5) * 90.0f,
				 obj->GetComponent<BlackPearl::Transform>()->GetRotation().z,

					});
		}*/
		// render
		// Render to our framebuffer
		//m_FrameBuffer->Bind(960, 540);
		//glEnable(GL_DEPTH_TEST);

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
	
		//std::vector<BlackPearl::Object*> objs;
		//objs.push_back(m_SkyBoxObj);

		m_MasterRenderer->RenderSceneExcept(m_ObjectsList, m_SkyBoxObj, GetLightSources());
		//DrawObjectsExcept(objs);
		//小于等于当前深度缓冲的fragment才被绘制
		glDepthFunc(GL_LEQUAL);
		m_MasterRenderer->RenderObject(m_SkyBoxObj);

		glDepthFunc(GL_LESS);

		//DrawObjectsExcept(objs);
		


		//m_FrameBuffer->UnBind();
	

		/*glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, 960, 540);
		
		objs.pop_back();
		DrawObjectsExcept(objs);

		
		glViewport(0, 0, 240, 135);
		
		DrawObject(m_QuadObj);*/
		
	}


	void OnAttach() override {


	}
	void InputCheck(float ts)
	{
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_W)) {
			m_CameraPosition += cameraComponent->Front() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_S)) {
			m_CameraPosition -= cameraComponent->Front()* m_CameraMoveSpeed * ts;
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_A)) {
			m_CameraPosition -= cameraComponent->Right() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_D)) {
			m_CameraPosition += cameraComponent->Right() * m_CameraMoveSpeed * ts;
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_E)) {
			m_CameraPosition += cameraComponent->Up() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_Q)) {
			m_CameraPosition -= cameraComponent->Up() * m_CameraMoveSpeed * ts;
		}
		// ---------------------Rotation--------------------------------------

		float posx = BlackPearl::Input::GetMouseX();
		float posy = BlackPearl::Input::GetMouseY();
		if (BlackPearl::Input::IsMouseButtonPressed(BP_MOUSE_BUTTON_RIGHT)) {


			if (BlackPearl::Input::IsFirstMouse()) {
				BlackPearl::Input::SetFirstMouse(false);
				m_LastMouseX = posx;
				m_LastMouseY = posy;
			}
			float diffx = posx - m_LastMouseX;
			float diffy = -posy + m_LastMouseY;

			m_LastMouseX = posx;
			m_LastMouseY = posy;
			m_CameraRotation.Yaw += diffx * m_CameraRotateSpeed * ts;
			m_CameraRotation.Pitch += diffy * m_CameraRotateSpeed * ts;

			if (m_CameraRotation.Pitch > 89.0f)
				m_CameraRotation.Pitch = 89.0f;
			if (m_CameraRotation.Pitch < -89.0f)
				m_CameraRotation.Pitch = -89.0f;

			cameraComponent->SetRotation(m_CameraRotation.Yaw, m_CameraRotation.Pitch);

		}
		else {

			m_LastMouseX = posx;//lastMouse时刻记录当前坐标位置，防止再次点击右键时，发生抖动！
			m_LastMouseY = posy;
		}


		cameraComponent->SetPosition(m_CameraPosition);
	}
private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_CameraObj;
	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_SkyBoxObj;
	glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
	struct CameraRotation {
		float Yaw;
		float Pitch;

	};
	CameraRotation m_CameraRotation;
	float m_LastMouseX;
	float m_LastMouseY;
	float m_CameraMoveSpeed = 5.0f;
	float m_CameraRotateSpeed = 9.0f;

	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };
	BlackPearl::MasterRenderer* m_MasterRenderer;

	


	//std::shared_ptr<BlackPearl::FrameBuffer> m_FrameBuffer;

};
