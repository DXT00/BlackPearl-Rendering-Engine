#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"


//#include "BlackPearl/Scene/Scene.h"
#include "Layers/AdvanceLightTestLayer.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/RHI/DynamicRHI.h"
#include "Layers/D3D12RayTracingLayer.h"
#include "Layers/D3D12RayTracingCubeLayer.h"
#include "Layers/D3D12RayTracingModelLayer.h"
#include "Layers/D3D12BasicRenderLayer.h"
#include "Layers/D3D12MeshShaderLayer.h"
#include "BlackPearl/RHI/DynamicRHI.h"
class SandBoxDX :public BlackPearl::Application {

public:

	SandBoxDX(HINSTANCE hInstance,int nShowCmd, BlackPearl::DynamicRHI::Type rhiType, const std::string &renderer)
	:Application(hInstance, nShowCmd, rhiType, renderer){
		
		BlackPearl::Layer* layer = NULL;
		const std::string layer_name = renderer+"Layer";

		if (renderer == "D3D12RayTracing") {
			layer = DBG_NEW D3D12RayTracingLayer(layer_name);
		}
		else if (renderer == "D3D12RayTracingCube") {
			layer = DBG_NEW D3D12RayTracingCubeLayer(layer_name);
		}
		else if (renderer == "D3D12RayTracingModelLayer") {
			layer = DBG_NEW D3D12RayTracingModelLayer(layer_name);
		}
		else if (renderer == "D3D12BasicRenderLayer") {
			layer = DBG_NEW D3D12BasicRenderLayer(layer_name);
		}
		else if (renderer == "D3D12MeshShaderRender") {
			layer = DBG_NEW D3D12MeshShaderLayer(layer_name);
		}

		GetLayerManager()->PushLayer(layer);
	}
	virtual ~SandBoxDX() = default;

};

BlackPearl::Application* BlackPearl::CreateApplication(HINSTANCE hInstance, int nShowCmd) {
	//return DBG_NEW SandBoxDX(hInstance, nShowCmd, BlackPearl::DynamicRHI::Type::D3D12, "D3D12MeshShaderRender");
	return DBG_NEW SandBoxDX(hInstance, nShowCmd, BlackPearl::DynamicRHI::Type::D3D12, "D3D12RayTracingCube");

}

