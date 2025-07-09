#include "pch.h"
#include "Renderer/CullingManager.h"
#include "Renderer/GIManager.h"
#include "Renderer/MasterRenderer/GI/IBLProbeRenderer.h"
#include "Renderer/MasterRenderer/GI/DDGIRenderer.h"
#include "Renderer/MasterRenderer/GI/SSGIRenderer.h"

namespace BlackPearl {

    GIRenderer* GIManager::CreateGIRenderer(IDevice* device, GIMethod method)
    {
        GE_ASSERT(!m_GIRenderer, "Renderer has been created! only one GI method can be used");
        switch (method)
        {
        case BlackPearl::DDGI:
            m_GIRenderer =  DBG_NEW DDGIRenderer(device);
            break;
        case BlackPearl::RTXDI:
            //todo::
            return nullptr;
        case BlackPearl::IBL:
            m_GIRenderer = DBG_NEW IBLProbeRenderer(device);
            break;
        case BlackPearl::SSGI:
            m_GIRenderer = DBG_NEW SSGIRenderer(device);
            break;
        default:
            break;
        }
        return m_GIRenderer;
    }
}