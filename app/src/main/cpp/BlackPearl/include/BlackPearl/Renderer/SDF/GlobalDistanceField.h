#pragma once
#include "Math/vector.h"
#include "RHI/RHITexture.h"
#include "RHI/RHICommandList.h"
namespace BlackPearl {



    /** Distance field data payload and output of the mesh build process. */
    class FDistanceFieldVolumeData 
    {
    public:

        /** Local space bounding box of the distance field volume. */
        AABB LocalSpaceMeshBounds;

        /** Whether most of the triangles in the mesh used a two-sided material. */
        bool bMostlyTwoSided;

        bool bAsyncBuilding;

        //TStaticArray<FSparseDistanceFieldMip, DistanceField::NumMips> Mips;

        //// Lowest resolution mip is always loaded so we always have something
        //TArray<uint8> AlwaysLoadedMip;

        //// Remaining mips are streamed
        //FByteBulkData StreamableMips;

        uint64_t Id;

        // For stats
        std::string AssetName;

        FDistanceFieldVolumeData() {


         }

     /*   void GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) const
        {
            CumulativeResourceSize.AddDedicatedSystemMemoryBytes(sizeof(*this));

            for (const FSparseDistanceFieldMip& Mip : Mips)
            {
                Mip.GetResourceSizeEx(CumulativeResourceSize);
            }

            CumulativeResourceSize.AddDedicatedSystemMemoryBytes(AlwaysLoadedMip.GetAllocatedSize());
        }*/

    /*    SIZE_T GetResourceSizeBytes() const
        {
            FResourceSizeEx ResSize;
            GetResourceSizeEx(ResSize);
            return ResSize.GetTotalMemoryBytes();
        }

        bool IsValid() const
        {
            return Mips[0].IndirectionDimensions.GetMax() > 0;
        }

#if WITH_EDITORONLY_DATA

        ENGINE_API void CacheDerivedData(const FString& InStaticMeshDerivedDataKey, const ITargetPlatform* TargetPlatform, UStaticMesh* Mesh, class FStaticMeshRenderData& RenderData, UStaticMesh* GenerateSource, float DistanceFieldResolutionScale, bool bGenerateDistanceFieldAsIfTwoSided);

#endif

        ENGINE_API void Serialize(FArchive& Ar, UObject* Owner);*/

        uint64_t GetId() const { return Id; }
    };



    struct ClipmapLevel
    {
        float VoxelSize;    // 当前层级的体素大小（世界单位）
        float Extent;       // 覆盖范围（世界单位）
        math::float3 Center;   // 当前层级的中心（世界坐标）
        TextureHandle MipTexture;
    };



    class GlobalDistanceField
    {
    public:
        GlobalDistanceField() {

        }
        void Init();
        void Update(const math::float3& cameraPos);




        TextureHandle GlobalDistanceFieldMipTexture;   // 存储距离场的 3D 纹理
        float ClipDim = 64.0f;

        // 全局配置
        const int NumClipMapLevels = 3;
        std::vector<ClipmapLevel> Clipmaps;


    private:

        void InitClipmaps();
        void UpdateClipmapCenters(const math::float3& CameraPos);

    };

}

