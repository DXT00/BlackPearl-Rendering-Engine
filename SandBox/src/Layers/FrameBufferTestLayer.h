#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
class FrameBufferTestLayer :public BlackPearl::Layer {
public:

	FrameBufferTestLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
	{
		
		

		m_MasterRenderer = DBG_NEW BlackPearl::MasterRenderer(m_MainCamera->GetObj());

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
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
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
private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	
	std::shared_ptr<BlackPearl::FrameBuffer> m_FrameBuffer;

	BlackPearl::MasterRenderer *m_MasterRenderer;

};
