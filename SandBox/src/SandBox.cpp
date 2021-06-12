#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"


//#include "BlackPearl/Scene/Scene.h"
#include "Layers/FrameBufferTestLayer.h"
#include "Layers/AdvanceLightTestLayer.h"
#include "Layers/ShadowMapTestLayer.h"
#include "Layers/ShadowMapPointLightLayer.h"
#include "Layers/PbrRenderingLayer.h"
#include "Layers/IBLRenderingLayer.h"
#include "Layers/SkyBoxTestLayer.h"
#include "Layers/VoxelConeTracingLayer.h"
#include "Layers/IBLProbesRenderingLayer.h"
#include "Layers/VoxelConeTracingDeferredLayer.h"
#include "Layers/VoxelConeTracingSVOLayer.h"
#include "Layers/RayTracingLayer.h"
#include "BlackPearl/Application.h"
#include "Layers/VoxelizationTestLayer.h"
#include "Layers/CubeTestLayer.h"

class SandBox :public BlackPearl::Application {
public:

	SandBox(const std::string &renderer) {
		BlackPearl::EntityManager * entityManager = DBG_NEW BlackPearl::EntityManager();
		BlackPearl::ObjectManager * objectManager = DBG_NEW BlackPearl::ObjectManager(entityManager);
		BlackPearl::Layer* layer = NULL;
		if (renderer == "ShadowMapPointLight") {
			layer = DBG_NEW ShadowMapPointLightLayer(renderer + "Layer", objectManager);
		}
		else if(renderer == "VoxelConeTracing"){
			layer = DBG_NEW VoxelConeTracingLayer(renderer + "Layer", objectManager);
		}
		else if (renderer == "PbrRendering") {
			layer = DBG_NEW PbrRenderingLayer(renderer + "Layer", objectManager);
		}
		else if (renderer == "IBLRendering") {
			layer = DBG_NEW IBLRenderingLayer(renderer + "Layer", objectManager);
		}
		else if (renderer == "IBLProbesRendering") {
			layer = DBG_NEW IBLProbesRenderingLayer(renderer + "Layer", objectManager);
		}
		else if (renderer == "VoxelConeTracingDeferred") {
			layer = DBG_NEW VoxelConeTracingDeferredLayer(renderer + "Layer", objectManager);
		}
		else if (renderer == "VoxelConeTracingSVO") {
			layer = DBG_NEW VoxelConeTracingSVOLayer(renderer + "Layer", objectManager);
		}
		else if (renderer == "RayTracing") {
			layer = DBG_NEW RayTracingLayer(renderer + "Layer", objectManager);
		}
		GetScene()->PushLayer(layer);
	}
	virtual ~SandBox() = default;

};

BlackPearl::Application* BlackPearl::CreateApplication() {

	return DBG_NEW SandBox("RayTracing");

}

