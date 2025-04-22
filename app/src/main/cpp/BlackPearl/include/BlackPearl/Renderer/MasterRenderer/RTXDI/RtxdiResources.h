#pragma once
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/RHICommandList.h"
namespace BlackPearl{
    class RtxdiResources
    {
    private:
        bool m_NeighborOffsetsInitialized = false;
        uint32_t m_MaxEmissiveMeshes = 0;
        uint32_t m_MaxEmissiveTriangles = 0;
        uint32_t m_MaxPrimitiveLights = 0;
        uint32_t m_MaxGeometryInstances = 0;

    public:
        BufferHandle TaskBuffer;
        BufferHandle PrimitiveLightBuffer;
        BufferHandle LightDataBuffer;
        BufferHandle GeometryInstanceToLightBuffer;
        BufferHandle LightIndexMappingBuffer;
        BufferHandle RisBuffer;
        BufferHandle RisLightDataBuffer;
        BufferHandle NeighborOffsetsBuffer;
        BufferHandle LightReservoirBuffer;
        BufferHandle GIReservoirBuffer;
        TextureHandle LocalLightPdfTexture;

        RtxdiResources(
            IDevice* device,
           // const rtxdi::ReSTIRDIContext& context,
           // const rtxdi::RISBufferSegmentAllocator& risBufferSegmentAllocator,
            uint32_t maxEmissiveMeshes,
            uint32_t maxEmissiveTriangles,
            uint32_t maxPrimitiveLights,
            uint32_t maxGeometryInstances);

        void InitializeNeighborOffsets(ICommandList* commandList, uint32_t neighborOffsetCount);

        uint32_t GetMaxEmissiveMeshes() const { return m_MaxEmissiveMeshes; }
        uint32_t GetMaxEmissiveTriangles() const { return m_MaxEmissiveTriangles; }
        uint32_t GetMaxPrimitiveLights() const { return m_MaxPrimitiveLights; }
        uint32_t GetMaxGeometryInstances() const { return m_MaxGeometryInstances; }

        BufferHandle GetRisLightDataBuffer() const { return RisLightDataBuffer; }
        BufferHandle GetLightDataBuffer() const { return LightDataBuffer; }
        BufferHandle GetRisBuffer() const { return RisBuffer; }

        static constexpr uint32_t c_NumReservoirBuffers = 3;
        static constexpr uint32_t c_NumGIReservoirBuffers = 2;

    };
}