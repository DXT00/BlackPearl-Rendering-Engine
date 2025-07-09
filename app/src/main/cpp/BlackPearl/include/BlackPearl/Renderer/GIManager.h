#pragma once
#include "BlackPearl/Scene/Scene.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Object/Object.h"
#include "Renderer/MasterRenderer/GI/GIRenderer.h"
#include "Config.h"
namespace BlackPearl {

 
    
	class GIManager
	{
	public:
        GIManager() = default;
        GIRenderer* CreateGIRenderer(IDevice* device, GIMethod method);
        GIRenderer* GetGIRenderer() const {
            return m_GIRenderer;
        }
    private:
        GIRenderer* m_GIRenderer = nullptr;

	};

}

