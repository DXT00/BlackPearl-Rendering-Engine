#pragma once
#include "RHI/RHITexture.h"
#include "RHI/RHIBuffer.h"

namespace BlackPearl {



    struct DDGIPipelineInternal
    {
        //raytrace pass.
        TextureHandle traceRadiance;
        TextureHandle traceDirectionDepth;

        TextureHandle irradiance[2];
        TextureHandle depth[2];

        int32_t    frames = 0;
        int32_t    pingPong = 0;
     

        //Randomizer rand;
    };
  


}
