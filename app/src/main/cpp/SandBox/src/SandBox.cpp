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
#include "Layers/CloudRenderLayer.h"
#include "Layers/WaterRenderLayer.h"
#include "Layers/SSRLayer.h"
enum RenderSample {
	BP_ShadowMapPointLight,
	BP_VoxelConeTracing,
	BP_PbrRendering,
	BP_IBLRendering,
	BP_IBLProbesRendering,
	BP_VoxelConeTracingDeferred,
	BP_VoxelConeTracingSVO,
	BP_RayTracing,
	BP_BatchRendering,
	BP_IndirectRendering,
	BP_IndirectOcclusionCull,
	BP_CloudRender,
	BP_WaterRender,
	BP_SSR

};
class SandBox :public BlackPearl::Application {
public:


	SandBox(HINSTANCE hInstance,int nShowCmd, BlackPearl::DynamicRHI::Type rhiType, RenderSample renderer)
	:Application(hInstance, nShowCmd, rhiType, selectAppVersion(renderer)){
		
		BlackPearl::Layer* layer = NULL;
		const std::string layer_name = renderer+"Layer";
		if (renderer == BP_ShadowMapPointLight) { //pcf, pcss
			layer = DBG_NEW ShadowMapPointLightLayer(layer_name);
		}
		else if(renderer == BP_VoxelConeTracing){
			layer = DBG_NEW VoxelConeTracingLayer(layer_name);
		}
		else if (renderer == BP_PbrRendering) {
			layer = DBG_NEW PbrRenderingLayer(layer_name);
		}
		else if (renderer == BP_IBLRendering) {
			layer = DBG_NEW IBLRenderingLayer(layer_name);
		}
		else if (renderer == BP_IBLProbesRendering) {
			layer = DBG_NEW IBLProbesRenderingLayer(layer_name);
		}
		else if (renderer == BP_VoxelConeTracingDeferred) {
			layer = DBG_NEW VoxelConeTracingDeferredLayer(layer_name);
		}
		else if (renderer == BP_VoxelConeTracingSVO) {
			layer = DBG_NEW VoxelConeTracingSVOLayer(layer_name);
		}
		else if (renderer == BP_RayTracing) {
			layer = DBG_NEW RayTracingLayer(layer_name);
		}
		else if (renderer == BP_BatchRendering) {
			layer = DBG_NEW BatchRenderingLayer(layer_name);
		}
		else if (renderer == BP_IndirectRendering) {
			layer = DBG_NEW IndirectDrawLayer(layer_name);
		}
		else if (renderer == BP_IndirectOcclusionCull) {
			layer = DBG_NEW IndirectOcclusionCullLayer(layer_name);
		}
		else if (renderer == BP_CloudRender) {
			layer = DBG_NEW CloudRenderLayer(layer_name);
		}
		else if (renderer == BP_WaterRender) {
			layer = DBG_NEW WaterRenderLayer(layer_name);
		}
		else if (renderer == BP_SSR) {
			layer = DBG_NEW SSRLayer(layer_name);
		}
		/*else if (renderer == "LumenRenderingLayer") {
			layer = DBG_NEW LumenRenderingLayer(layer_name);
		}*/
		GetLayerManager()->PushLayer(layer);
	}
	virtual ~SandBox() = default;
private:
	BlackPearl::AppVersion selectAppVersion(RenderSample sample) {
		return BlackPearl::AppVersion::VERSION_1_0;
	
	}

};

BlackPearl::Application* BlackPearl::CreateApplication(HINSTANCE hInstance, int nShowCmd) {

	return DBG_NEW SandBox(hInstance, nShowCmd, BlackPearl::DynamicRHI::Type::OpenGL, RenderSample::BP_IBLRendering);
	//return DBG_NEW SandBox(hInstance, nShowCmd, BlackPearl::DynamicRHI::Type::OpenGL, "PbrRendering");
}

