
#ifndef VOXEL_CB_H
#define VOXEL_CB_H
#include "align.h"
#include "light_types.h"


struct VoxelConstants
{
    ALIGN(16) float3 areaExtent; // voxel 覆盖的区域大小，与MapManager 划分的area一致

    ALIGN(16) float3 center;
    ALIGN(16) float3 dimension;


};

struct VoxelVisualConstants
{
    ALIGN(16) float3 areaExtent; // voxel 覆盖的区域大小，与MapManager 划分的area一致

    ALIGN(16) float3 center;
    ALIGN(16) float3 dimension;


    ALIGN(4) int mipLevel;


};


#endif // VOXEL_CB_H