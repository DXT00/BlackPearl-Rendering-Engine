#include "pch.h"
#ifdef GE_API_D3D12
#include "SDFScene.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12RootSignature.h"
#include "BlackPearl/RHI/D3D12RHI/DirectXRaytracingHelper.h"

namespace BlackPearl {
	SDFScene::SDFScene(DemoType type)
		:Scene(type)
	{
		
	}
	SDFScene::~SDFScene()
	{
	}
	void SDFScene::LoadScene(const std::string& sceneName)
	{

	}
	void SDFScene::LoadScene(int sceneId)
	{
	}


}
#endif