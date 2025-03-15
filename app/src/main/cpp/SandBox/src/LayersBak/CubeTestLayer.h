#pragma once
#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class CubeTestLayer :public BlackPearl::Layer {
public:

	CubeTestLayer(const std::string& name)
		: Layer(name)
	{

		BlackPearl::Renderer::Init();

		LoadScene("CubesScene");
	}

	virtual ~CubeTestLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor1);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		//std::vector<BlackPearl::Object*> objs;
		//objs.push_back(m_SkyBoxObj);
		//	objs.push_back(m_CubeObj);
		//DrawObjects();
		//m_MasterRenderer->RenderScene(m_ObjectsList, GetLightSources());

		//DrawObjectsExcept(objs);
		//小于等于当前深度缓冲的fragment才被绘制
		//glDepthFunc(GL_LEQUAL);
		//DrawObject(m_SkyBoxObj);

		//glDepthFunc(GL_LESS);
		m_BasicRenderer->DrawObjects(m_BackGroundObjsList);


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


	BlackPearl::math::float4  m_BackgroundColor1 = { 1.0f,1.0f,1.0f,1.0f };


	BlackPearl::BasicRenderer* m_BasicRenderer;


	//std::shared_ptr<BlackPearl::FrameBuffer> m_FrameBuffer;

};
#pragma once
