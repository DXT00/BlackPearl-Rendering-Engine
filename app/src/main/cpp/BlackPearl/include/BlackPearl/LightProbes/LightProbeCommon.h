#pragma once
#include "BlackPearl/Math/Math.h"
using namespace BlackPearl::math;
#include "hlsl/core/light_probe_cb.h"

namespace BlackPearl {

    enum ProbeType {
        DIFFUSE_PROBE = PT_DIFFUSE_PROBE,
        REFLECTION_PROBE = PT_REFLECTION_PROBE
    };

    enum ProbeStorageType {
        PS_CubeMap,
        PS_SH,
        PS_Texture2D //ddgi
    };

}