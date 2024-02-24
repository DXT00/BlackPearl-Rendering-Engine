#pragma once
#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class VkGpuParticlesLayer :public BlackPearl::Layer {
public:

	VkGpuParticlesLayer(const std::string& name)
		: Layer(name)
	{
		m_VkComputeRender = DBG_NEW BlackPearl::VkComputeShaderRender();

		m_VkComputeRender->Init();
		//LoadScene("CubesScene");
	}

	virtual ~VkGpuParticlesLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {
		//InputCheck(ts);

		//BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor1);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		m_VkComputeRender->Render();
		//m_VkBasicRender->DrawObjects(m_BackGroundObjsList);

	}

	void OnAttach() override {

	}

private:
	glm::vec4 m_BackgroundColor1 = { 1.0f,1.0f,1.0f,1.0f };
	BlackPearl::VkComputeShaderRender* m_VkComputeRender;


};
#pragma once
