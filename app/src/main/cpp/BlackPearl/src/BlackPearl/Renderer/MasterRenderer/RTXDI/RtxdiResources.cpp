#include "pch.h"
#include "BlackPearl/Core.h"
#include "RtxdiResources.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "ShaderParameters.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"

namespace BlackPearl {
	RtxdiResources::RtxdiResources(
		IDevice* device,
		uint32_t maxEmissiveMeshes, 
		uint32_t maxEmissiveTriangles, 
		uint32_t maxPrimitiveLights,
		uint32_t maxGeometryInstances) : m_MaxEmissiveMeshes(maxEmissiveMeshes)
		, m_MaxEmissiveTriangles(maxEmissiveTriangles)
		, m_MaxPrimitiveLights(maxPrimitiveLights)
		, m_MaxGeometryInstances(maxGeometryInstances)
	{
        BufferDesc taskBufferDesc;
        taskBufferDesc.byteSize = sizeof(PrepareLightsTask) * std::max((maxEmissiveMeshes + maxPrimitiveLights), 1u);
        taskBufferDesc.structStride = sizeof(PrepareLightsTask);
        taskBufferDesc.initialState = ResourceStates::ShaderResource;
        taskBufferDesc.keepInitialState = true;
        taskBufferDesc.debugName = "TaskBuffer";
        taskBufferDesc.canHaveUAVs = true;
        TaskBuffer = device->createBuffer(taskBufferDesc);


        BufferDesc primitiveLightBufferDesc;
        primitiveLightBufferDesc.byteSize = sizeof(PolymorphicLightInfo) * std::max(maxPrimitiveLights, 1u);
        primitiveLightBufferDesc.structStride = sizeof(PolymorphicLightInfo);
        primitiveLightBufferDesc.initialState = ResourceStates::ShaderResource;
        primitiveLightBufferDesc.keepInitialState = true;
        primitiveLightBufferDesc.debugName = "PrimitiveLightBuffer";
        PrimitiveLightBuffer = device->createBuffer(primitiveLightBufferDesc);


        //BufferDesc risBufferDesc;
        //risBufferDesc.byteSize = sizeof(uint32_t) * 2 * std::max(risBufferSegmentAllocator.getTotalSizeInElements(), 1u); // RG32_UINT per element
        //risBufferDesc.format = Format::RG32_UINT;
        //risBufferDesc.canHaveTypedViews = true;
        //risBufferDesc.initialState = ResourceStates::ShaderResource;
        //risBufferDesc.keepInitialState = true;
        //risBufferDesc.debugName = "RisBuffer";
        //risBufferDesc.canHaveUAVs = true;
        //RisBuffer = device->createBuffer(risBufferDesc);


        //risBufferDesc.byteSize = sizeof(uint32_t) * 8 * std::max(risBufferSegmentAllocator.getTotalSizeInElements(), 1u); // RGBA32_UINT x 2 per element
        //risBufferDesc.format = Format::RGBA32_UINT;
        //risBufferDesc.debugName = "RisLightDataBuffer";
        //RisLightDataBuffer = device->createBuffer(risBufferDesc);


        uint32_t maxLocalLights = maxEmissiveTriangles + maxPrimitiveLights;
        uint32_t lightBufferElements = maxLocalLights * 2;

        BufferDesc lightBufferDesc;
        lightBufferDesc.byteSize = sizeof(PolymorphicLightInfo) * std::max(lightBufferElements, 1u);
        lightBufferDesc.structStride = sizeof(PolymorphicLightInfo);
        lightBufferDesc.initialState = ResourceStates::ShaderResource;
        lightBufferDesc.keepInitialState = true;
        lightBufferDesc.debugName = "LightDataBuffer";
        lightBufferDesc.canHaveUAVs = true;
        LightDataBuffer = device->createBuffer(lightBufferDesc);


        BufferDesc geometryInstanceToLightBufferDesc;
        geometryInstanceToLightBufferDesc.byteSize = sizeof(uint32_t) * maxGeometryInstances;
        geometryInstanceToLightBufferDesc.structStride = sizeof(uint32_t);
        geometryInstanceToLightBufferDesc.initialState = ResourceStates::ShaderResource;
        geometryInstanceToLightBufferDesc.keepInitialState = true;
        geometryInstanceToLightBufferDesc.debugName = "GeometryInstanceToLightBuffer";
        GeometryInstanceToLightBuffer = device->createBuffer(geometryInstanceToLightBufferDesc);


        BufferDesc lightIndexMappingBufferDesc;
        lightIndexMappingBufferDesc.byteSize = sizeof(uint32_t) * std::max(lightBufferElements, 1u);
        lightIndexMappingBufferDesc.format = Format::R32_UINT;
        lightIndexMappingBufferDesc.canHaveTypedViews = true;
        lightIndexMappingBufferDesc.initialState = ResourceStates::ShaderResource;
        lightIndexMappingBufferDesc.keepInitialState = true;
        lightIndexMappingBufferDesc.debugName = "LightIndexMappingBuffer";
        lightIndexMappingBufferDesc.canHaveUAVs = true;
        LightIndexMappingBuffer = device->createBuffer(lightIndexMappingBufferDesc);


        //BufferDesc neighborOffsetBufferDesc;
        //neighborOffsetBufferDesc.byteSize = context.getStaticParameters().NeighborOffsetCount * 2;
        //neighborOffsetBufferDesc.format = Format::RG8_SNORM;
        //neighborOffsetBufferDesc.canHaveTypedViews = true;
        //neighborOffsetBufferDesc.debugName = "NeighborOffsets";
        //neighborOffsetBufferDesc.initialState = ResourceStates::ShaderResource;
        //neighborOffsetBufferDesc.keepInitialState = true;
        //NeighborOffsetsBuffer = device->createBuffer(neighborOffsetBufferDesc);


        //BufferDesc lightReservoirBufferDesc;
        //lightReservoirBufferDesc.byteSize = sizeof(RTXDI_PackedDIReservoir) * context.getReservoirBufferParameters().reservoirArrayPitch * rtxdi::c_NumReSTIRDIReservoirBuffers;
        //lightReservoirBufferDesc.structStride = sizeof(RTXDI_PackedDIReservoir);
        //lightReservoirBufferDesc.initialState = ResourceStates::UnorderedAccess;
        //lightReservoirBufferDesc.keepInitialState = true;
        //lightReservoirBufferDesc.debugName = "LightReservoirBuffer";
        //lightReservoirBufferDesc.canHaveUAVs = true;
        //LightReservoirBuffer = device->createBuffer(lightReservoirBufferDesc);


        //BufferDesc giReservoirBufferDesc;
        //giReservoirBufferDesc.byteSize = sizeof(RTXDI_PackedGIReservoir) * context.getReservoirBufferParameters().reservoirArrayPitch * rtxdi::c_NumReSTIRDIReservoirBuffers;
        //giReservoirBufferDesc.structStride = sizeof(RTXDI_PackedGIReservoir);
        //giReservoirBufferDesc.initialState = ResourceStates::UnorderedAccess;
        //giReservoirBufferDesc.keepInitialState = true;
        //giReservoirBufferDesc.debugName = "GIReservoirBuffer";
        //giReservoirBufferDesc.canHaveUAVs = true;
        //GIReservoirBuffer = device->createBuffer(giReservoirBufferDesc);

        //TextureDesc localLightPdfDesc;
        //rtxdi::ComputePdfTextureSize(maxLocalLights, localLightPdfDesc.width, localLightPdfDesc.height, localLightPdfDesc.mipLevels);
        //assert(localLightPdfDesc.width * localLightPdfDesc.height >= maxLocalLights);
        //localLightPdfDesc.isUAV = true;
        //localLightPdfDesc.debugName = "LocalLightPdf";
        //localLightPdfDesc.initialState = ResourceStates::ShaderResource;
        //localLightPdfDesc.keepInitialState = true;
        //localLightPdfDesc.format = Format::R32_FLOAT; // Use FP32 here to allow a wide range of flux values, esp. when downsampled.
        //LocalLightPdfTexture = device->createTexture(localLightPdfDesc);
	}
	void RtxdiResources::InitializeNeighborOffsets(ICommandList* commandList, uint32_t neighborOffsetCount)
	{
	}
}
