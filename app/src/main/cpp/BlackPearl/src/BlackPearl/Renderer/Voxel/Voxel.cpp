#include "pch.h"
#include "Renderer/Voxel/Voxel.h"
#include "Config.h"
#include "Renderer/DeviceManager.h"
namespace BlackPearl {
    extern DeviceManager* g_deviceManager;
    void Voxel::Init(math::float3 _center, math::float3 _tileID, math::float3 _areaExtend) {
        center = _center;
        tileID = _tileID;
        mipLevel = Configuration::VoxelMipLevel;
        dimension = Configuration::VoxelDim;
        areaExtend = _areaExtend;

        std::vector<float> texture3D(1.0 * dimension * dimension * dimension, 0.0f);

        TextureDesc desc;
        desc.type = TextureType::Image3DMap;
        desc.width = dimension;
        desc.height = dimension;
        desc.depth = dimension;
        desc.minFilter = FilterMode::Linear;
        desc.magFilter = FilterMode::Linear;
        desc.wrap = SamplerAddressMode::ClampToEdge;
        desc.dimension = TextureDimension::Texture3D;
        desc.format = Format::RGBA8_UNORM;
        desc.access = TextureAccess::ReadWrite;
        desc.data = texture3D.data();
        // 创建 3D 纹理（R16F 存储距离场值）

        voxelTexture = g_deviceManager->GetDevice()->createTexture(desc);

        isDirty = false;

    }


    void Voxel::Update() {

        if (isDirty) {
            Init(center, tileID, areaExtend);
        }


    }

}