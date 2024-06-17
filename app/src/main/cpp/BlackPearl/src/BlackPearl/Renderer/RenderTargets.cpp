#include "pch.h"
#include "RenderTargets.h"
#include "BlackPearl/RHI/RHI.h"
namespace BlackPearl {


    void RenderTargets::Init(
        IDevice* device,
        donut::math::int2 renderSize,
        donut::math::int2 displaySize,
        bool enableMotionVectors,
        bool useReverseProjection,
        int backbufferCount)
    {
//        m_UseReverseProjection = useReverseProjection;
//        m_BackbufferCount = backbufferCount;
//        m_Device = device;
//        m_RenderSize = renderSize;
//        m_DisplaySize = displaySize;
//
//        TextureDesc desc;
//        desc.width = renderSize.x;
//        desc.height = renderSize.y;
//
//        desc.isVirtual = false; //device->queryFeatureSupport(Feature::VirtualResources); <- codepath not up to date, needs refactoring
//
//        desc.initialState = ResourceStates::RenderTarget;
//        desc.isRenderTarget = true;
//        desc.useClearValue = true;
//        desc.clearValue = Color(0.f);
//        desc.sampleCount = 1; assert(m_SampleCount == 1);
//        desc.dimension = TextureDimension::Texture2D;
//        desc.keepInitialState = true;
//        desc.isTypeless = false;
//        desc.isUAV = false;
//        desc.mipLevels = 1;
//
//        desc.format = Format::R32_FLOAT;
//        desc.isTypeless = false;
//        desc.isUAV = true;
//        desc.isRenderTarget = true;
//        desc.initialState = ResourceStates::RenderTarget;
//        desc.debugName = "Depth";
//        desc.clearValue = useReverseProjection ? Color(0.f) : Color(1.f);
//        desc.useClearValue = true;
//        Depth = device->createTexture(desc);
//
//        desc.isTypeless = false;
//        desc.isRenderTarget = true;
//        desc.initialState = ResourceStates::RenderTarget;
//        desc.clearValue = Color(0.f);
//        desc.isUAV = true;
//        desc.format = Format::RGBA16_FLOAT;	// allow for .z component too
//        desc.debugName = "ScreenMotionVectors";
//        ScreenMotionVectors = device->createTexture(desc);
//        desc.format = Format::RGBA16_FLOAT;
//        desc.debugName = "DenoiserMotionVectors";
//        DenoiserMotionVectors = device->createTexture(desc);
//
//        desc.format = Format::R32_UINT;
//        desc.debugName = "Throughput";
//        Throughput = device->createTexture(desc);
//
//        desc.format = Format::RGBA16_FLOAT;
//        desc.debugName = "StableRadianceBuffer";
//        StableRadiance = device->createTexture(desc);
//
//        desc.format = Format::R32_UINT;
//        desc.arraySize = 4;
//        desc.dimension = TextureDimension::Texture2DArray;
//        desc.debugName = "StablePlanesHeader";
//        StablePlanesHeader = device->createTexture(desc);
//        desc.dimension = TextureDimension::Texture2D;
//        desc.arraySize = 1;
//
//        desc.format = Format::RGBA16_FLOAT;
//        desc.debugName = "DenoiserDiffRadianceHitDist";
//        DenoiserDiffRadianceHitDist = device->createTexture(desc);
//        desc.debugName = "DenoiserOutDiffRadianceHitDist";
//        for (int i = 0; i < cStablePlaneCount; ++i)
//            DenoiserOutDiffRadianceHitDist[i] = device->createTexture(desc);
//
//        desc.debugName = "DenoiserSpecRadianceHitDist";
//        DenoiserSpecRadianceHitDist = device->createTexture(desc);
//        desc.debugName = "DenoiserOutSpecRadianceHitDist";
//        for (int i = 0; i < cStablePlaneCount; ++i)
//            DenoiserOutSpecRadianceHitDist[i] = device->createTexture(desc);
//
//        desc.debugName = "DebugVizOutput";
//        desc.format = Format::RGBA16_FLOAT;
//        desc.clearValue = Color(0.0f, 0.0f, 0.0f, 0.0f);   // avoid the debug layer warnings... not actually cleared except for debug purposes
//        DebugVizOutput = device->createTexture(desc);
//#if ENABLE_DEBUG_VIZUALISATION
//        desc.format = Format::RGBA8_UNORM;
//        desc.debugName = "DenoiserOutValidation";
//        DenoiserOutValidation = device->createTexture(desc);
//#endif
//
//        desc.format = Format::R32_FLOAT;
//        desc.debugName = "DenoiserViewspaceZ";
//        DenoiserViewspaceZ = device->createTexture(desc);
//
//        desc.format = Format::R10G10B10A2_UNORM;
//        desc.debugName = "DenoiserNormalRoughness";
//        DenoiserNormalRoughness = device->createTexture(desc);
//
//        desc.format = Format::RGBA32_FLOAT;
//        desc.debugName = "SecondarySurfacePositionNormal";
//        SecondarySurfacePositionNormal = device->createTexture(desc);
//
//        desc.format = Format::RGBA16_FLOAT;
//        desc.debugName = "SecondarySurfaceRadiance";
//        SecondarySurfaceRadiance = device->createTexture(desc);
//
//        desc.isUAV = false;
//        desc.isRenderTarget = true;
//        desc.useClearValue = true;
//        desc.clearValue = Color(1.f);
//        desc.sampleCount = m_SampleCount;
//        desc.dimension = m_SampleCount > 1 ? TextureDimension::Texture2DMS : TextureDimension::Texture2D;
//        desc.keepInitialState = true;
//
//        desc.useClearValue = false;
//        desc.clearValue = Color(0.f);
//        desc.isTypeless = false;
//        desc.isUAV = true;
//        desc.isRenderTarget = true;
//        desc.format = Format::RGBA32_FLOAT;
//        desc.initialState = ResourceStates::RenderTarget;
//        desc.debugName = "AccumulatedRadiance";
//        AccumulatedRadiance = device->createTexture(desc);
//
//        desc.useClearValue = true;
//        desc.format = Format::RGBA16_FLOAT;  // keep in float for now in case we need 
//        desc.debugName = "OutputColor";
//        desc.clearValue = Color(1.0f, 1.0f, 0.0f, 0.0f);   // avoid the debug layer warnings... not actually cleared except for debug purposes
//        desc.isUAV = true;
//        OutputColor = device->createTexture(desc);
//
//        desc.format = Format::R8_UNORM;
//        desc.isUAV = true;
//        desc.debugName = "DenoiserDisocclusionThresholdMix";
//        DenoiserDisocclusionThresholdMix = device->createTexture(desc);
//        desc.debugName = "CombinedHistoryClampRelax";
//        CombinedHistoryClampRelax = device->createTexture(desc);
//
//        // !!! NOTE !!! setting desc.width/desc.height to display size (was render size!)
//        desc.width = displaySize.x;
//        desc.height = displaySize.y;
//
//        desc.format = Format::RGBA16_FLOAT;
//        desc.debugName = "ProcessedOutputColor";
//        ProcessedOutputColor = device->createTexture(desc);
//        desc.format = Format::RGBA16_SNORM;
//        desc.debugName = "TemporalFeedback1";
//        TemporalFeedback1 = device->createTexture(desc);
//        desc.debugName = "TemporalFeedback2";
//        TemporalFeedback2 = device->createTexture(desc);
//
//        desc.format = Format::SRGBA8_UNORM;
//        desc.isUAV = false;
//        desc.debugName = "LdrColor";
//        LdrColor = device->createTexture(desc);
//
//        desc.debugName = "PreUIColor";
//        PreUIColor = device->createTexture(desc);
//
//        if (desc.isVirtual)
//        {
//            uint64_t heapSize = 0;
//            ITexture* const textures[] = {
//                //HdrColor,
//                //ResolvedColor,
//                //TemporalFeedback1,
//                //TemporalFeedback2,
//                LdrColor,
//                OutputColor,
//                PreUIColor,
//                //AmbientOcclusion
//            };
//
//            for (auto texture : textures)
//            {
//                MemoryRequirements memReq = device->getTextureMemoryRequirements(texture);
//                heapSize = align(heapSize, memReq.alignment);
//                heapSize += memReq.size;
//            }
//
//            HeapDesc heapDesc;
//            heapDesc.type = HeapType::DeviceLocal;
//            heapDesc.capacity = heapSize;
//            heapDesc.debugName = "RenderTargetHeap";
//
//            Heap = device->createHeap(heapDesc);
//
//            uint64_t offset = 0;
//            for (auto texture : textures)
//            {
//                MemoryRequirements memReq = device->getTextureMemoryRequirements(texture);
//                offset = align(offset, memReq.alignment);
//
//                device->bindTextureMemory(texture, Heap, offset);
//
//                offset += memReq.size;
//            }
//        }
//
//        LdrFramebuffer = std::make_shared<donut::engine::FramebufferFactory>(device);
//        LdrFramebuffer->RenderTargets = { LdrColor };
//
//        { // Stable planes
//            BufferDesc bufferDesc;
//            bufferDesc.isVertexBuffer = false;
//            bufferDesc.isConstantBuffer = false;
//            bufferDesc.isVolatile = false;
//            bufferDesc.canHaveUAVs = true;
//            bufferDesc.cpuAccess = CpuAccessMode::None;
//            bufferDesc.keepInitialState = true;
//            bufferDesc.initialState = ResourceStates::Common;
//
//            bufferDesc.structStride = sizeof(StablePlane);
//            bufferDesc.byteSize = sizeof(StablePlane) * GenericTSComputeStorageElementCount(m_RenderSize.x, m_RenderSize.y, cStablePlaneCount);
//            bufferDesc.debugName = "StablePlanesBuffer";
//            StablePlanesBuffer = device->createBuffer(bufferDesc);
//        }
//
//        {
//            BufferDesc surfaceBufferDesc;
//            surfaceBufferDesc.byteSize = sizeof(PackedPathTracerSurfaceData) * 2 * renderSize.x * renderSize.y; // *2 is for history!
//            surfaceBufferDesc.byteSize = sizeof(PackedPathTracerSurfaceData) * GenericTSComputeStorageElementCount(m_RenderSize.x, m_RenderSize.y, 2); // *2 is for history!
//            surfaceBufferDesc.structStride = sizeof(PackedPathTracerSurfaceData);
//            surfaceBufferDesc.initialState = ResourceStates::UnorderedAccess;
//            surfaceBufferDesc.keepInitialState = true;
//            surfaceBufferDesc.debugName = "SurfaceData(GBuffer)";
//            surfaceBufferDesc.canHaveUAVs = true;
//            SurfaceDataBuffer = device->createBuffer(surfaceBufferDesc);
//        }
    }

    [[nodiscard]] bool RenderTargets::IsUpdateRequired(donut::math::int2 renderSize, donut::math::int2 displaySize, donut::math::uint sampleCount) const
    {
     /*   if (any(m_RenderSize != renderSize) || any(m_DisplaySize != displaySize) || m_SampleCount != sampleCount) return true;*/
        return false;
    }

    void RenderTargets::Clear(ICommandList* commandList)
    {
       /* const FormatInfo& depthFormatInfo = getFormatInfo(Depth->getDesc().format);

        float depthClearValue = m_UseReverseProjection ? 0.f : 1.f;
        commandList->clearTextureFloat(Depth, AllSubresources, Color(depthClearValue));

        commandList->clearTextureFloat(CombinedHistoryClampRelax, AllSubresources, Color(0));*/
    }


}
