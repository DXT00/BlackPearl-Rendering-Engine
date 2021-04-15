#include <BlackPeral.h>
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
	SandBox() {
		BlackPearl::EntityManager * entityManager = DBG_NEW BlackPearl::EntityManager();
		BlackPearl::ObjectManager * objectManager = DBG_NEW BlackPearl::ObjectManager(entityManager);
		//BlackPearl::Layer* layer = DBG_NEW ShadowMapPointLightLayer("ShadowMapPointLight Layer", objectManager);
		//BlackPearl::Layer* layer = DBG_NEW VoxelConeTracingLayer("VoxelConeTracing Layer", objectManager);

		//BlackPearl::Layer* layer = DBG_NEW PbrRenderingLayer("PbrRendering Layer", objectManager);
		//BlackPearl::Layer* layer = DBG_NEW IBLRenderingLayer("IBLRenderingLayer", objectManager);
		//BlackPearl::Layer* layer = DBG_NEW IBLProbesRenderingLayer("IBLProbesRenderingLayer", objectManager);
		
		//BlackPearl::Layer* layer = DBG_NEW VoxelConeTracingDeferredLayer("VoxelConeTracingDeferred Layer", objectManager);
		//BlackPearl::Layer* layer = DBG_NEW VoxelConeTracingSVOLayer("VoxelConeTracingSVO Layer", objectManager);

		BlackPearl::Layer* layer = DBG_NEW RayTracingLayer("RayTracing Layer", objectManager);

		GetScene()->PushLayer(layer);
	}
	virtual ~SandBox() = default;

};

BlackPearl::Application* BlackPearl::CreateApplication() {

	return DBG_NEW SandBox();

}

