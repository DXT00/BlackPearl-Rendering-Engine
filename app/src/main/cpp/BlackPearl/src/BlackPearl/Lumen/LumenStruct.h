//#pragma once
//#include "glm/glm.hpp"
//namespace BlackPearl {
//
//	struct FRHIDispatchIndirectParameters
//	{
//		uint32_t ThreadGroupCountX;
//		uint32_t ThreadGroupCountY;
//		uint32_t ThreadGroupCountZ;
//	};
//	struct FLumenPageTableEntry
//	{
//		//// Allocated physical page data
//		//FIntPoint PhysicalPageCoord = FIntPoint(-1, -1);
//
//		//// Allows to point to a sub-allocation inside a shared physical page
//		//FIntRect PhysicalAtlasRect;
//
//		//// Sampling data, can point to a coarser page
//		//uint32_t SamplePageIndex = 0;
//		//uint16 SampleAtlasBiasX = 0;
//		//uint16 SampleAtlasBiasY = 0;
//		//uint16 SampleCardResLevelX = 0;
//		//uint16 SampleCardResLevelY = 0;
//
//		//// CardPage for atlas operations
//		//int32_t CardIndex = -1;
//		//uint8 ResLevel = 0;
//		//FVector4f CardUVRect;
//
//		//FIntPoint SubAllocationSize = FIntPoint(-1, -1);
//
//		//bool IsSubAllocation() const
//		//{
//		//	return SubAllocationSize.X >= 0 || SubAllocationSize.Y >= 0;
//		//}
//
//		//bool IsMapped() const
//		//{
//		//	return PhysicalPageCoord.X >= 0 && PhysicalPageCoord.Y >= 0;
//		//}
//
//		//uint32_t GetNumVirtualTexels() const
//		//{
//		//	return IsSubAllocation() ? SubAllocationSize.X * SubAllocationSize.Y : Lumen::VirtualPageSize * Lumen::VirtualPageSize;
//		//}
//
//		//uint32_t GetNumPhysicalTexels() const
//		//{
//		//	return IsMapped() ? PhysicalAtlasRect.Area() : 0;
//		//}
//	};
//
//
//	class FLumenCardUpdateContext
//	{
//	public:
//		enum EIndirectArgOffset
//		{
//			ThreadPerPage = 0 * sizeof(FRHIDispatchIndirectParameters),
//			ThreadPerTile = 1 * sizeof(FRHIDispatchIndirectParameters),
//			MAX = 2,
//		};
//
//		FRDGBufferRef CardPageIndexAllocator;
//		FRDGBufferRef CardPageIndexData;
//		FRDGBufferRef DrawCardPageIndicesIndirectArgs;
//		FRDGBufferRef DispatchCardPageIndicesIndirectArgs;
//
//		glm::vec2 UpdateAtlasSize;
//		uint32_t MaxUpdateTiles;
//		uint32_t UpdateFactor;
//	};
//
//
//	class FLumenCardTracingInputs
//	{
//	public:
//
//		FLumenCardTracingInputs(FRDGBuilder& GraphBuilder, const FScene* Scene, const FViewInfo& View, FLumenSceneFrameTemporaries& FrameTemporaries, bool bSurfaceCacheFeedback = true);
//
//		FRDGTextureRef AlbedoAtlas;
//		FRDGTextureRef OpacityAtlas;
//		FRDGTextureRef NormalAtlas;
//		FRDGTextureRef EmissiveAtlas;
//		FRDGTextureRef DepthAtlas;
//
//		FRDGTextureRef DirectLightingAtlas;
//		FRDGTextureRef IndirectLightingAtlas;
//		FRDGTextureRef RadiosityNumFramesAccumulatedAtlas;
//		FRDGTextureRef FinalLightingAtlas;
//		FRDGTextureRef VoxelLighting;
//
//		// Feedback
//		FRDGBufferUAVRef CardPageLastUsedBufferUAV;
//		FRDGBufferUAVRef CardPageHighResLastUsedBufferUAV;
//		FRDGBufferUAVRef SurfaceCacheFeedbackBufferAllocatorUAV;
//		FRDGBufferUAVRef SurfaceCacheFeedbackBufferUAV;
//		uint32 SurfaceCacheFeedbackBufferSize;
//		uint32 SurfaceCacheFeedbackBufferTileWrapMask;
//		FIntPoint SurfaceCacheFeedbackBufferTileJitter;
//
//		// Voxel clipmaps
//		FIntVector VoxelGridResolution;
//		int32 NumClipmapLevels;
//		TStaticArray<FVector, MaxVoxelClipmapLevels> ClipmapWorldToUVScale;
//		TStaticArray<FVector, MaxVoxelClipmapLevels> ClipmapWorldToUVBias;
//		TStaticArray<FVector, MaxVoxelClipmapLevels> ClipmapWorldCenter;
//		TStaticArray<FVector, MaxVoxelClipmapLevels> ClipmapWorldExtent;
//		TStaticArray<FVector, MaxVoxelClipmapLevels> ClipmapWorldSamplingExtent;
//		TStaticArray<FVector4f, MaxVoxelClipmapLevels> ClipmapVoxelSizeAndRadius;
//
//		TRDGUniformBufferRef<FLumenCardScene> LumenCardSceneUniformBuffer;
//	};
//
//	struct FLumenCardScene {
//
//		uint32_t NumCards;
//			uint32_t NumMeshCards;
//			uint32_t NumCardPages;
//			uint32_t NumHeightfields;
//			uint32_t MaxConeSteps;
//			FVector2f  PhysicalAtlasSize;
//			FVector2f, InvPhysicalAtlasSize;
//			float IndirectLightingAtlasDownsampleFactor;
//			uint32_t NumDistantCards;
//			float DistantSceneMaxTraceDistance;
//			FVector3f DistantSceneDirection;
//			uint32_t DistantCardIndices[MaxDistantCards];
//			StructuredBuffer<float4> CardData;
//			StructuredBuffer<float4> CardPageData;
//			StructuredBuffer<float4> MeshCardsData;
//			StructuredBuffer<float4> HeightfieldData;
//			ByteAddressBuffer PageTableBuffer;
//			ByteAddressBuffer SceneInstanceIndexToMeshCardsIndexBuffer;
//			Texture2D OpacityAtlas;
//			Texture2D AlbedoAtlas;
//			Texture2D NormalAtlas;
//			Texture2D EmissiveAtlas;
//			Texture2D DepthAtlas;
//
//	};
//
//	struct FLumenLightTileScatterParameters {
//		RDG_BUFFER_ACCESS(DrawIndirectArgs, ERHIAccess::IndirectArgs)
//		RDG_BUFFER_ACCESS(DispatchIndirectArgs, ERHIAccess::IndirectArgs)
//		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint>, LightTileAllocator)
//		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint2>, LightTiles)
//		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint>, LightTileOffsetsPerLight)
//	};
//	struct FLightTileCullContext
//	{
//		FLumenLightTileScatterParameters LightTileScatterParameters;
//		FRDGBufferRef LightTileAllocator;
//		FRDGBufferRef LightTiles;
//		FRDGBufferRef DispatchLightTilesIndirectArgs;
//		uint32_t MaxCulledCardTiles;
//	};
//
//	struct FLumenCardDirectLighting {
//		FRasterizeToLightTilesVS::FParameters VS;
//		FLumenCardDirectLightingPS::FParameters PS;
//	};
//
//	struct FLumenRadiosityTexelTraceParameters{
//		    SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint>, CardTileAllocator)
//			SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint>, CardTileData)
//			SHADER_PARAMETER_RDG_TEXTURE(Texture2D, TraceRadianceAtlas)
//			SHADER_PARAMETER_RDG_TEXTURE(Texture2D<float>, TraceHitDistanceAtlas)
//			SHADER_PARAMETER(FIntPoint, RadiosityAtlasSize)
//			SHADER_PARAMETER(uint32, ProbeSpacingInRadiosityTexels)
//			SHADER_PARAMETER(uint32, ProbeSpacingInRadiosityTexelsDivideShift)
//			SHADER_PARAMETER(uint32, RadiosityTileSize)
//			SHADER_PARAMETER(uint32, HemisphereProbeResolution)
//			SHADER_PARAMETER(uint32, NumTracesPerProbe)
//			SHADER_PARAMETER(uint32, UseProbeOcclusion)
//			SHADER_PARAMETER(int32, FixedJitterIndex)
//			SHADER_PARAMETER(uint32, MaxFramesAccumulated)
//	}
//}
