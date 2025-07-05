#include "pch.h"
#include "Renderer/CullingManager.h"
#include "Renderer/GIManager.h"
#include "Renderer/MasterRenderer/GI/IBLProbeRenderer.h"
#include "Renderer/MasterRenderer/GI/DDGIRenderer.h"
#include "Renderer/MasterRenderer/GI/SSGIRenderer.h"

namespace BlackPearl {

    GIRenderer* GIManager::CreateGIRenderer(IDevice* device, GIMethod method)
    {
        switch (method)
        {
        case BlackPearl::DDGI:
            return  DBG_NEW DDGIRenderer(device);
        case BlackPearl::RTXDI:
            //todo::
            return nullptr;
        case BlackPearl::IBL:
            return  DBG_NEW IBLProbeRenderer(device);
        case BlackPearl::SSGI:
            return  DBG_NEW SSGIRenderer(device);
        default:
            break;
        }
        return nullptr;
    }
}