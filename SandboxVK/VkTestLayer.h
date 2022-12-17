#pragma once
#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class VkTestLayer :public BlackPearl::Layer {
public:

	VkTestLayer(const std::string& name)
		: Layer(name)
	{
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();

		BlackPearl::Renderer::Init();
		LoadScene("CubesScene");
	}

	virtual ~VkTestLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {
		InputCheck(ts);

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor1);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		m_BasicRenderer->DrawObjects(m_BackGroundObjsList);

	}

	void OnAttach() override {

	}

private:
	glm::vec4 m_BackgroundColor1 = { 1.0f,1.0f,1.0f,1.0f };
	BlackPearl::BasicRenderer* m_BasicRenderer;


};
#pragma once
