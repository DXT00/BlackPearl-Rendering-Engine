#pragma once
#include "BlackPearl/Scene/Scene.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Object/Object.h"
namespace BlackPearl {
	class CullingManager
	{
	public:
		std::vector<Object*> CullOctree(Scene* Scene, const IView& View, std::vector<bool>& OutVisibleNodes);

	private:
		std::vector<Object*> _GetVisibleObjs(std::vector<bool>& OutVisibleNodes, Scene* Scene);
	};

}

