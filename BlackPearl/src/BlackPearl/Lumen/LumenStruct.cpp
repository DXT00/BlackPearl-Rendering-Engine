#include "pch.h"
//#include "LumenStruct.h"
//#include "LumenSceneData.h"
//namespace BlackPearl {
//	FLumenCardTracingInputs::FLumenCardTracingInputs(FRDGBuilder& GraphBuilder, const FScene* Scene, const FViewInfo& View, FLumenSceneFrameTemporaries& FrameTemporaries, bool bSurfaceCacheFeedback)
//	{
//
//		LumenSceneData& LumenSceneData = *Scene->LumenSceneData;
//
//		{
//			FLumenCardScene* LumenCardSceneParameters = GraphBuilder.AllocParameters<FLumenCardScene>();
//			SetupLumenCardSceneParameters(GraphBuilder, Scene, *LumenCardSceneParameters);
//			LumenCardSceneUniformBuffer = GraphBuilder.CreateUniformBuffer(LumenCardSceneParameters);
//		}
//
//		check(LumenSceneData.FinalLightingAtlas);
//
//		AlbedoAtlas = GraphBuilder.RegisterExternalTexture(LumenSceneData.AlbedoAtlas);
//		OpacityAtlas = GraphBuilder.RegisterExternalTexture(LumenSceneData.OpacityAtlas);
//		NormalAtlas = GraphBuilder.RegisterExternalTexture(LumenSceneData.NormalAtlas);
//		EmissiveAtlas = GraphBuilder.RegisterExternalTexture(LumenSceneData.EmissiveAtlas);
//		DepthAtlas = GraphBuilder.RegisterExternalTexture(LumenSceneData.DepthAtlas);
//
//		DirectLightingAtlas = GraphBuilder.RegisterExternalTexture(LumenSceneData.DirectLightingAtlas);
//		IndirectLightingAtlas = GraphBuilder.RegisterExternalTexture(LumenSceneData.IndirectLightingAtlas);
//		RadiosityNumFramesAccumulatedAtlas = GraphBuilder.RegisterExternalTexture(LumenSceneData.RadiosityNumFramesAccumulatedAtlas);
//		FinalLightingAtlas = GraphBuilder.RegisterExternalTexture(LumenSceneData.FinalLightingAtlas);
//
//		if (View.ViewState && View.ViewState->Lumen.VoxelLighting)
//		{
//			VoxelLighting = GraphBuilder.RegisterExternalTexture(View.ViewState->Lumen.VoxelLighting);
//			VoxelGridResolution = View.ViewState->Lumen.VoxelGridResolution;
//			NumClipmapLevels = View.ViewState->Lumen.NumClipmapLevels;
//
//			for (int32_t ClipmapIndex = 0; ClipmapIndex < NumClipmapLevels; ++ClipmapIndex)
//			{
//				const FLumenVoxelLightingClipmapState& Clipmap = View.ViewState->Lumen.VoxelLightingClipmapState[ClipmapIndex];
//
//				ClipmapWorldToUVScale[ClipmapIndex] = FVector(1.0f) / (2.0f * Clipmap.Extent);
//				ClipmapWorldToUVBias[ClipmapIndex] = -(Clipmap.Center - Clipmap.Extent) * ClipmapWorldToUVScale[ClipmapIndex];
//				ClipmapVoxelSizeAndRadius[ClipmapIndex] = FVector4f((FVector3f)Clipmap.VoxelSize, Clipmap.VoxelRadius);
//				ClipmapWorldCenter[ClipmapIndex] = Clipmap.Center;
//				ClipmapWorldExtent[ClipmapIndex] = Clipmap.Extent;
//				ClipmapWorldSamplingExtent[ClipmapIndex] = Clipmap.Extent - 0.5f * Clipmap.VoxelSize;
//			}
//		}
//		else
//		{
//			VoxelLighting = GraphBuilder.RegisterExternalTexture(GSystemTextures.VolumetricBlackDummy);
//			VoxelGridResolution = FIntVector(1);
//			NumClipmapLevels = 0;
//		}
//
//		if (FrameTemporaries.CardPageLastUsedBuffer && FrameTemporaries.CardPageHighResLastUsedBuffer)
//		{
//			CardPageLastUsedBufferUAV = GraphBuilder.CreateUAV(FrameTemporaries.CardPageLastUsedBuffer, ERDGUnorderedAccessViewFlags::SkipBarrier);
//			CardPageHighResLastUsedBufferUAV = GraphBuilder.CreateUAV(FrameTemporaries.CardPageHighResLastUsedBuffer, ERDGUnorderedAccessViewFlags::SkipBarrier);
//		}
//		else
//		{
//			CardPageLastUsedBufferUAV = GraphBuilder.CreateUAV(GraphBuilder.RegisterExternalBuffer(GWhiteVertexBufferWithRDG->Buffer), PF_R32_UINT);
//			CardPageHighResLastUsedBufferUAV = GraphBuilder.CreateUAV(GraphBuilder.RegisterExternalBuffer(GWhiteVertexBufferWithRDG->Buffer), PF_R32_UINT);
//		}
//
//		if (FrameTemporaries.SurfaceCacheFeedbackResources.Buffer && bSurfaceCacheFeedback)
//		{
//			SurfaceCacheFeedbackBufferAllocatorUAV = GraphBuilder.CreateUAV(FrameTemporaries.SurfaceCacheFeedbackResources.BufferAllocator, ERDGUnorderedAccessViewFlags::SkipBarrier);
//			SurfaceCacheFeedbackBufferUAV = GraphBuilder.CreateUAV(FrameTemporaries.SurfaceCacheFeedbackResources.Buffer, ERDGUnorderedAccessViewFlags::SkipBarrier);
//			SurfaceCacheFeedbackBufferSize = FrameTemporaries.SurfaceCacheFeedbackResources.BufferSize;
//			SurfaceCacheFeedbackBufferTileJitter = LumenSceneData.SurfaceCacheFeedback.GetFeedbackBufferTileJitter();
//			SurfaceCacheFeedbackBufferTileWrapMask = Lumen::GetFeedbackBufferTileWrapMask();
//		}
//		else
//		{
//			SurfaceCacheFeedbackBufferAllocatorUAV = LumenSceneData.SurfaceCacheFeedback.GetDummyFeedbackAllocatorUAV(GraphBuilder);
//			SurfaceCacheFeedbackBufferUAV = LumenSceneData.SurfaceCacheFeedback.GetDummyFeedbackUAV(GraphBuilder);
//			SurfaceCacheFeedbackBufferSize = 0;
//			SurfaceCacheFeedbackBufferTileJitter = FIntPoint(0, 0);
//			SurfaceCacheFeedbackBufferTileWrapMask = 0;
//		}
//	}
//
//}
