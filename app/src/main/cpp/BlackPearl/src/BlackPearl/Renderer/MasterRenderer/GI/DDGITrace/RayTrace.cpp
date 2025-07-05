#include "pch.h"
#include "Renderer/MasterRenderer/GI/DDGITrace/RayTrace.h"
#include "Renderer/MasterRenderer/GI/DDGITrace/SDFTrace.h"
#include "Renderer/MasterRenderer/GI/DDGITrace/HWTrace.h"
#include "Renderer/MasterRenderer/GI/DDGIRenderer.h"
namespace BlackPearl {
    RayTrace* RayTrace::CreateRayTracer(DDGITraceType type)
    {
        switch (type)
        {
        case DDGITraceType::SW_Trace:
            return DBG_NEW SDFTrace();
        case DDGITraceType::HW_Trace:
            return DBG_NEW HWTrace();
            ;
        default:
            break;
        }




        return nullptr;
    }

}
