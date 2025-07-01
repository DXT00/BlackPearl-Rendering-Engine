#pragma once
#include "Math/vector.h"
#include "RHI/RHITexture.h"
#include "RHI/RHICommandList.h"

namespace BlackPearl {



    /** Distance field data payload and output of the mesh build process. */
    class Voxel
    {
    public:
        Voxel() {
            center = math::float3(0.0);
            dimension = 64;
            mipLevel = 3;
            tileID = math::float3(0.0);
            voxelTexture = nullptr;
        }



        void Init(math::float3 _center, math::float3 _tileID, math::float3 _areaExtend);
        void Update();


        math::float3 center;
        uint32_t dimension;
        uint32_t mipLevel;
        math::float3 tileID;
        math::float3 areaExtend;
        TextureHandle voxelTexture;
        bool isDirty = true;

    private:
 

    };






}

