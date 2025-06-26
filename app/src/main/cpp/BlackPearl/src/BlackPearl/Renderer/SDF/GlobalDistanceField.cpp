#include "pch.h"
#include "Renderer/SDF/GlobalDistanceField.h"
#include "Renderer/DeviceManager.h"

namespace BlackPearl {
    extern DeviceManager* g_deviceManager;

    void GlobalDistanceField::Init() {
        InitClipmaps();
    }

    void GlobalDistanceField::Update(const math::float3& cameraPos) {
        UpdateClipmapCenters(cameraPos);
    }


    void GlobalDistanceField::InitClipmaps()
    {
        // 多层clipmap
        for (int i = 0; i < NumClipMapLevels; i++) {
            ClipmapLevel level;
            level.VoxelSize = 1.0f * std::pow(2, i); // 层级越高，体素越大
            level.Extent = ClipDim * level.VoxelSize;
            level.Center = math::float3(0.0f);
            std::vector<float> texture3D(1.0 * ClipDim * ClipDim * ClipDim, 0.0f);

            TextureDesc desc;
            desc.type = TextureType::Image3DMap;
            desc.width = ClipDim;
            desc.height = ClipDim;
            desc.depth = ClipDim;
            desc.minFilter = FilterMode::Linear;
            desc.magFilter = FilterMode::Linear;
            desc.wrap = SamplerAddressMode::ClampToEdge;
            desc.dimension = TextureDimension::Texture3D;
            desc.format = Format::R16_FLOAT;
            desc.access = TextureAccess::ReadWrite;
            desc.data = texture3D.data();
            // 创建 3D 纹理（R16F 存储距离场值）
            level.MipTexture = g_deviceManager->GetDevice()->createTexture(desc);

            Clipmaps.push_back(level);
        }
    }

    void GlobalDistanceField::UpdateClipmapCenters(const math::float3& CameraPos)
    {
        for (auto& level : Clipmaps) {
            // 将中心对齐到体素网格，避免抖动
            level.Center = math::float3(
                std::floor(CameraPos.x / level.VoxelSize),
                std::floor(CameraPos.y / level.VoxelSize),
                std::floor(CameraPos.z / level.VoxelSize)
            ) * level.VoxelSize;
        }
    }

}