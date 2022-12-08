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
#include "Layers/BatchRenderingLayer.h"
#include "BlackPearl/Application.h"
#include "Layers/VoxelizationTestLayer.h"
#include "Layers/CubeTestLayer.h"
#include "Layers/IndirectDrawLayer.h"
#include "Layers/IndirectOcclusionCullLayer.h"

class SandBox :public BlackPearl::Application {
public:

	SandBox(HINSTANCE hInstance,int nShowCmd, BlackPearl::DynamicRHI::Type rhiType, const std::string &renderer)
	:Application(hInstance, nShowCmd, rhiType, renderer){
		
		BlackPearl::Layer* layer = NULL;
		const std::string layer_name = renderer+"Layer";
		if (renderer == "ShadowMapPointLight") {
			layer = DBG_NEW ShadowMapPointLightLayer(layer_name);
		}
		else if(renderer == "VoxelConeTracing"){
			layer = DBG_NEW VoxelConeTracingLayer(layer_name);
		}
		else if (renderer == "PbrRendering") {
			layer = DBG_NEW PbrRenderingLayer(layer_name);
		}
		else if (renderer == "IBLRendering") {
			layer = DBG_NEW IBLRenderingLayer(layer_name);
		}
		else if (renderer == "IBLProbesRendering") {
			layer = DBG_NEW IBLProbesRenderingLayer(layer_name);
		}
		else if (renderer == "VoxelConeTracingDeferred") {
			layer = DBG_NEW VoxelConeTracingDeferredLayer(layer_name);
		}
		else if (renderer == "VoxelConeTracingSVO") {
			layer = DBG_NEW VoxelConeTracingSVOLayer(layer_name);
		}
		else if (renderer == "RayTracing") {
			layer = DBG_NEW RayTracingLayer(layer_name);
		}
		else if (renderer == "BatchRendering") {
			layer = DBG_NEW BatchRenderingLayer(layer_name);
		}
		else if (renderer == "IndirectRendering") {
			layer = DBG_NEW IndirectDrawLayer(layer_name);
		}
		else if (renderer == "IndirectOcclusionCull") {
			layer = DBG_NEW IndirectOcclusionCullLayer(layer_name);
		}
		GetLayerManager()->PushLayer(layer);
	}
	virtual ~SandBox() = default;

};

BlackPearl::Application* BlackPearl::CreateApplication(HINSTANCE hInstance, int nShowCmd) {

	return DBG_NEW SandBox(hInstance, nShowCmd, BlackPearl::DynamicRHI::Type::OpenGL, "IndirectOcclusionCull");

}

