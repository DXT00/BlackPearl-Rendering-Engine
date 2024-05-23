#pragma once
#pragma once
#include <BlackPearl.h>

class VkRHIRenderGraphLayer :public BlackPearl::Layer {
public:

	VkRHIRenderGraphLayer(const std::string& name)
		: Layer(name)
	{
		m_VkComputeRender = DBG_NEW BlackPearl::VkComputeShaderRender();

		m_VkComputeRender->Init();
	}

	virtual ~VkRHIRenderGraphLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {

		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		m_VkComputeRender->Render();
	}

	void OnAttach() override {

	}

private:
	glm::vec4 m_BackgroundColor1 = { 1.0f,1.0f,1.0f,1.0f };
	BlackPearl::VkComputeShaderRender* m_VkComputeRender;


};
#pragma once
