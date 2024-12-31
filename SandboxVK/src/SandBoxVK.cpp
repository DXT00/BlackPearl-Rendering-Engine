#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "BlackPearl/Application.h"
#include "BlackPearl/RHI/DynamicRHI.h"

#include "BlackPearl/RHI/DynamicRHI.h"
#include "Layers/VkTestLayer.h"
#include "Layers/VkGpuParticlesLayer.h"
#include "Layers/VkRayTracingLayer.h"
#include "Layers/VkRHIRenderGraphLayer.h"
enum RenderSample {
	BP_VkTest,
	BP_VkGpuParticles,
	BP_VkRayTracing,
	BP_VkRHIRenderGraphLayer
};

class SandBoxVK :public BlackPearl::Application {

public:

	SandBoxVK(HINSTANCE hInstance, int nShowCmd, BlackPearl::DynamicRHI::Type rhiType, RenderSample renderer)
		:Application(hInstance, nShowCmd, rhiType, selectAppVersion(renderer)) {

		BlackPearl::Layer* layer = NULL;
		const std::string layer_name = renderer + "Layer";

		if (renderer == BP_VkTest) {
			layer = DBG_NEW VkTestLayer(layer_name);
		}
		else if (renderer == BP_VkGpuParticles) {
			layer = DBG_NEW VkGpuParticlesLayer(layer_name);
		}
		else if (renderer == BP_VkRayTracing) {
			layer = DBG_NEW VkRayTracingLayer(layer_name);
		}
		else if (renderer == BP_VkRHIRenderGraphLayer) {
			layer = DBG_NEW VkRHIRenderGraphLayer(layer_name);
		}
		GetLayerManager()->PushLayer(layer);
	}
	virtual ~SandBoxVK() {

	}

private:
	BlackPearl::AppVersion selectAppVersion(RenderSample sample) {
		if (sample <= BP_VkRayTracing) {
			return BlackPearl::AppVersion::VERSION_0_0;
		}
		else {
			return BlackPearl::AppVersion::VERSION_1_0;
		}
	}
};

BlackPearl::Application* BlackPearl::CreateApplication(HINSTANCE hInstance, int nShowCmd) {
	return DBG_NEW SandBoxVK(hInstance, nShowCmd, BlackPearl::DynamicRHI::Type::Vulkan, RenderSample::BP_VkRayTracing);

}

