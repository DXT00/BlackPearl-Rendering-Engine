#pragma once
#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class AdvanceLightTestLayer :public BlackPearl::Layer {
public:

	AdvanceLightTestLayer(const std::string& name)
		: Layer(name)
	{

		m_MasterRenderer = DBG_NEW BlackPearl::MasterRenderer(m_MainCamera->GetObj());
		//m_FrameBuffer.reset(DBG_NEW BlackPearl::FrameBuffer(BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight));

		BlackPearl::Renderer::Init();

	
		m_CubeObj = CreateCube("assets/shaders/Cube.glsl", "assets/texture/wood.png");
	
	
	}

	virtual ~AdvanceLightTestLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
	
		//std::vector<BlackPearl::Object*> objs;
		//objs.push_back(m_SkyBoxObj);
		//	objs.push_back(m_CubeObj);
		//DrawObjects();
		m_MasterRenderer->RenderScene(m_ObjectsList, GetLightSources());

		//DrawObjectsExcept(objs);
		//小于等于当前深度缓冲的fragment才被绘制
		//glDepthFunc(GL_LEQUAL);
		//DrawObject(m_SkyBoxObj);

		//glDepthFunc(GL_LESS);

	

	}

	
	void OnAttach() override {


	}
	
private:

	std::vector<BlackPearl::Object*> m_LightObjs;
//	BlackPearl::Object* m_CameraObj;
	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_SkyBoxObj;


	BlackPearl::math::float4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };


	BlackPearl::MasterRenderer *m_MasterRenderer;


	//std::shared_ptr<BlackPearl::FrameBuffer> m_FrameBuffer;

};
