#pragma once
#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
class FrameBufferTestLayer :public BlackPearl::Layer {
public:

	FrameBufferTestLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
	{
		
		m_CameraObj = CreateCamera();
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		m_CameraPosition = cameraComponent->GetPosition();
		m_CameraRotation.Yaw = cameraComponent->Yaw();
		m_CameraRotation.Pitch = cameraComponent->Pitch();

		m_MasterRenderer = DBG_NEW BlackPearl::MasterRenderer(m_CameraObj);

		m_FrameBuffer.reset(DBG_NEW BlackPearl::FrameBuffer(
			BlackPearl::Configuration::WindowWidth, 
			BlackPearl::Configuration::WindowHeight, 
			{BlackPearl::FrameBuffer::Attachment::ColorTexture,  BlackPearl::FrameBuffer::Attachment::RenderBuffer},
			0,false
		));

		BlackPearl::Renderer::Init();

		m_PlaneObj = Layer::CreatePlane();
		m_QuadObj = Layer::CreateQuad();
		m_CubeObj = Layer::CreateCube();
		//m_IronManObj = Layer::CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/IronMan.glsl");
		auto meshComponent = m_QuadObj->GetComponent<BlackPearl::MeshRenderer>();
		meshComponent->SetTexture(0, m_FrameBuffer->GetColorTexture(0));

		//把FrameBuffer中的texture作为贴图，贴到m_CubeObj上
		auto CubemeshComponent = m_CubeObj->GetComponent<BlackPearl::MeshRenderer>();
		CubemeshComponent->SetTexture(0, m_FrameBuffer->GetColorTexture(0));
		//	Layer::CreateLight(BlackPearl::LightType::PointLight);
	}

	virtual ~FrameBufferTestLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render
		// Render to our framebuffer
		//m_FrameBuffer->Bind(960, 540);
		glViewport(0, 0, 960, 540);
	/*	m_IronManObj->GetComponent<BlackPearl::Transform>()->SetRotation({
				m_IronManObj->GetComponent<BlackPearl::Transform>()->GetRotation().x,
				 sin(glfwGetTime() * 0.5) * 90.0f,
				 m_IronManObj->GetComponent<BlackPearl::Transform>()->GetRotation().z,

			});*/
		m_FrameBuffer->Bind();
		glEnable(GL_DEPTH_TEST);

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		// Set Drawing buffers
		/*GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);*/

		//DrawObjects();
		std::vector<BlackPearl::Object*> objs;
		objs.push_back(m_QuadObj);
		objs.push_back(m_CubeObj);

		m_MasterRenderer->RenderObjectsExcept(m_ObjectsList, { m_QuadObj,m_CubeObj });
		//DrawObjectsExcept(objs);
		////DrawObject(m_PlaneObj);


		m_FrameBuffer->UnBind();
		//glDisable(GL_DEPTH_TEST);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, 960, 540);
		//DrawObjectsExcept(m_CubeObj);
		objs.pop_back();
		//DrawObjectsExcept(objs);
		m_MasterRenderer->RenderObjectsExcept(m_ObjectsList, { m_QuadObj });

		//m_FrameBuffer.BindTexture();
		glViewport(0, 0, 240, 135);
		
		//m_MasterRenderer.RenderObject(m_QuadObj);



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

	
	std::shared_ptr<BlackPearl::FrameBuffer> m_FrameBuffer;

	BlackPearl::MasterRenderer *m_MasterRenderer;

};
