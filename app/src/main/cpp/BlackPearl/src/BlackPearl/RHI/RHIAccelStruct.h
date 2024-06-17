#pragma once
#include<string>
#include "RHIDefinitions.h"
#include "RefCountPtr.h"
#include "RHIResources.h"
#include "RHIBuffer.h"
namespace BlackPearl {
            typedef float AffineTransform[12];

        constexpr AffineTransform c_IdentityTransform = {
        //  +----+----+---------  rotation and scaling
        //  v    v    v
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f
        //                 ^
        //                 +----  translation
        };

        enum class GeometryFlags : uint8_t
        {
            None = 0,
            Opaque = 1,
            NoDuplicateAnyHitInvocation = 2
        };

        NVRHI_ENUM_CLASS_FLAG_OPERATORS(GeometryFlags)

        enum class GeometryType : uint8_t
        {
            Triangles = 0,
            AABBs = 1
        };

        struct GeometryAABB
        {
            float minX;
            float minY;
            float minZ;
            float maxX;
            float maxY;
            float maxZ;
        };

        struct GeometryTriangles
        {
            IBuffer* indexBuffer = nullptr;   // make sure the first fields in both Triangles 
            IBuffer* vertexBuffer = nullptr;  // and AABBs are IBuffer* for easier debugging
            Format indexFormat = Format::UNKNOWN;
            Format vertexFormat = Format::UNKNOWN;
            uint64_t indexOffset = 0;
            uint64_t vertexOffset = 0;
            uint32_t indexCount = 0;
            uint32_t vertexCount = 0;
            uint32_t vertexStride = 0;

          //  IOpacityMicromap* opacityMicromap = nullptr;
            IBuffer* ommIndexBuffer = nullptr;
            uint64_t ommIndexBufferOffset = 0;
            Format ommIndexFormat = Format::UNKNOWN;
           // const OpacityMicromapUsageCount* pOmmUsageCounts = nullptr;
            uint32_t numOmmUsageCounts = 0;

            GeometryTriangles& setIndexBuffer(IBuffer* value) { indexBuffer = value; return *this; }
            GeometryTriangles& setVertexBuffer(IBuffer* value) { vertexBuffer = value; return *this; }
            GeometryTriangles& setIndexFormat(Format value) { indexFormat = value; return *this; }
            GeometryTriangles& setVertexFormat(Format value) { vertexFormat = value; return *this; }
            GeometryTriangles& setIndexOffset(uint64_t value) { indexOffset = value; return *this; }
            GeometryTriangles& setVertexOffset(uint64_t value) { vertexOffset = value; return *this; }
            GeometryTriangles& setIndexCount(uint32_t value) { indexCount = value; return *this; }
            GeometryTriangles& setVertexCount(uint32_t value) { vertexCount = value; return *this; }
           // GeometryTriangles& setVertexStride(uint32_t value) { vertexStride = value; return *this; }
            //GeometryTriangles& setOpacityMicromap(IOpacityMicromap* value) { opacityMicromap = value; return *this; }
            GeometryTriangles& setOmmIndexBuffer(IBuffer* value) { ommIndexBuffer = value; return *this; }
            GeometryTriangles& setOmmIndexBufferOffset(uint64_t value) { ommIndexBufferOffset = value; return *this; }
            GeometryTriangles& setOmmIndexFormat(Format value) { ommIndexFormat = value; return *this; }
            //GeometryTriangles& setPOmmUsageCounts(const OpacityMicromapUsageCount* value) { pOmmUsageCounts = value; return *this; }
            GeometryTriangles& setNumOmmUsageCounts(uint32_t value) { numOmmUsageCounts = value; return *this; }
        };

        struct GeometryAABBs
        {
            IBuffer* buffer = nullptr;
            IBuffer* unused = nullptr;
            uint64_t offset = 0;
            uint32_t count = 0;
            uint32_t stride = 0;

            GeometryAABBs& setBuffer(IBuffer* value) { buffer = value; return *this; }
            GeometryAABBs& setOffset(uint64_t value) { offset = value; return *this; }
            GeometryAABBs& setCount(uint32_t value) { count = value; return *this; }
            GeometryAABBs& setStride(uint32_t value) { stride = value; return *this; }
        };

    struct GeometryDesc
    {
        union GeomTypeUnion
        {
            GeometryTriangles triangles;
            GeometryAABBs aabbs;
        } geometryData;

        bool useTransform = false;
        AffineTransform transform{};
        GeometryFlags flags = GeometryFlags::None;
        GeometryType geometryType = GeometryType::Triangles;

        GeometryDesc() : geometryData{} { }

        GeometryDesc& setTransform(const AffineTransform& value) { memcpy(&transform, &value, sizeof(AffineTransform)); useTransform = true; return *this; }
        GeometryDesc& setFlags(GeometryFlags value) { flags = value; return *this; }
        GeometryDesc& setTriangles(const GeometryTriangles& value) { geometryData.triangles = value; geometryType = GeometryType::Triangles; return *this; }
        GeometryDesc& setAABBs(const GeometryAABBs& value) { geometryData.aabbs = value; geometryType = GeometryType::AABBs; return *this; }
    };

    enum class AccelStructBuildFlags : uint8_t
    {
        None = 0,
        AllowUpdate = 1,
        AllowCompaction = 2,
        PreferFastTrace = 4,
        PreferFastBuild = 8,
        MinimizeMemory = 0x10,
        PerformUpdate = 0x20,

        // Removes the errors or warnings that NVRHI validation layer issues when a TLAS
        // includes an instance that points at a NULL BLAS or has a zero instance mask.
        // Only affects the validation layer, doesn't translate to Vk/DX12 AS build flags.
        AllowEmptyInstances = 0x80
    };

    NVRHI_ENUM_CLASS_FLAG_OPERATORS(AccelStructBuildFlags)

        struct AccelStructDesc
    {
        size_t topLevelMaxInstances = 0; // only applies when isTopLevel = true
        std::vector<GeometryDesc> bottomLevelGeometries; // only applies when isTopLevel = false
        AccelStructBuildFlags buildFlags = AccelStructBuildFlags::None;
        std::string debugName;
        bool trackLiveness = true;
        bool isTopLevel = false;
        bool isVirtual = false;

        AccelStructDesc& setTopLevelMaxInstances(size_t value) { topLevelMaxInstances = value; isTopLevel = true; return *this; }
        AccelStructDesc& addBottomLevelGeometry(const GeometryDesc& value) { bottomLevelGeometries.push_back(value); isTopLevel = false; return *this; }
        AccelStructDesc& setBuildFlags(AccelStructBuildFlags value) { buildFlags = value; return *this; }
        AccelStructDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
        AccelStructDesc& setTrackLiveness(bool value) { trackLiveness = value; return *this; }
        AccelStructDesc& setIsTopLevel(bool value) { isTopLevel = value; return *this; }
        AccelStructDesc& setIsVirtual(bool value) { isVirtual = value; return *this; }
    };

    //////////////////////////////////////////////////////////////////////////
    // rt::AccelStruct
    //////////////////////////////////////////////////////////////////////////

    class IAccelStruct : public IResource
    {
    public:
        [[nodiscard]] virtual const AccelStructDesc& getDesc() const = 0;
        [[nodiscard]] virtual bool isCompacted() const = 0;
        [[nodiscard]] virtual uint64_t getDeviceAddress() const = 0;
    };

    typedef RefCountPtr<IAccelStruct> AccelStructHandle;
}