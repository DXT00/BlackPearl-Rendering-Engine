//// Copyright Epic Games, Inc. All Rights Reserved.
//
///*=============================================================================
//	LumenRadiosity.cpp
//=============================================================================*/
#include "pch.h"
//#include "BlackPearl/Math/Math.h"
//#include "BlackPearl/Lumen/Lumen.h"
//#include "BlackPearl/Scene/Scene.h"
//namespace BlackPearl {
//	//Whether to enable the Radiosity, which is an indirect lighting gather from the Surface Cache that provides multibounce diffuse
//	int32_t GLumenRadiosity = 1;
//	//Distance between probes, in Surface Cache texels
//	int32_t GLumenRadiosityProbeSpacing = 4;
//	//Number of traces along one dimension of the hemisphere probe layout
//	int32_t GLumenRadiosityHemisphereProbeResolution = 4;
//	
//	
//	namespace LumenRadiosity {
//
//		void AddRadiosityPass(
//			const Scene* Scene,
//			const FViewInfo& View,
//			bool bRenderSkylight,
//			LumenSceneData& LumenSceneData,
//			FRDGTextureRef RadiosityAtlas,
//			FRDGTextureRef RadiosityNumFramesAccumulatedAtlas,
//			const FLumenCardTracingInputs& TracingInputs,
//			const FLumenCardUpdateContext& CardUpdateContext) {
//
//			const int32_t ProbeSpacing = LumenRadiosity::GetRadiosityProbeSpacing(View);
//			const int32_t HemisphereProbeResolution = LumenRadiosity::GetHemisphereProbeResolution(View);
//			const uint32_t RadiosityTileSize = Lumen::CardTileSize / ProbeSpacing;
//
//			FIntPoint RadiosityProbeAtlasSize;
//			RadiosityProbeAtlasSize.X = FMath::DivideAndRoundUp<uint32>(LumenSceneData.GetPhysicalAtlasSize().X, ProbeSpacing);
//			RadiosityProbeAtlasSize.Y = FMath::DivideAndRoundUp<uint32>(LumenSceneData.GetPhysicalAtlasSize().Y, ProbeSpacing);
//
//			FIntPoint RadiosityProbeTracingAtlasSize = RadiosityProbeAtlasSize * FIntPoint(HemisphereProbeResolution, HemisphereProbeResolution);
//
//			FRDGTextureRef TraceRadianceAtlas = RegisterOrCreateRadiosityAtlas(
//				//GraphBuilder,
//				LumenSceneData.RadiosityTraceRadianceAtlas,
//				TEXT("Lumen.Radiosity.TraceRadianceAtlas"),
//				RadiosityProbeTracingAtlasSize,
//				PF_FloatRGB);
//
//			const bool bUseProbeOcclusion = GRadiosityFilteringProbeOcclusion != 0
//				// Self intersection from grazing angle traces causes noise that breaks probe occlusion
//				&& Lumen::UseHardwareRayTracedRadiosity(*View.Family);
//
//			FRDGTextureRef TraceHitDistanceAtlas = nullptr;
//
//			if (bUseProbeOcclusion)
//			{
//				TraceHitDistanceAtlas = RegisterOrCreateRadiosityAtlas(
//					GraphBuilder,
//					LumenSceneData.RadiosityTraceHitDistanceAtlas,
//					TEXT("Lumen.Radiosity.TraceHitDistanceAtlas"),
//					RadiosityProbeTracingAtlasSize,
//					PF_R16F);
//			}
//			else
//			{
//				TraceHitDistanceAtlas = GraphBuilder.CreateTexture(FRDGTextureDesc::Create2D(FIntPoint(1, 1), PF_R16F, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV), TEXT("Dummy"));
//			}
//
//			const uint32_t MaxCardTilesX = FMath::DivideAndRoundUp<uint32>(LumenSceneData.GetPhysicalAtlasSize().X, Lumen::CardTileSize);
//			const uint32_t MaxCardTilesY = FMath::DivideAndRoundUp<uint32>(LumenSceneData.GetPhysicalAtlasSize().Y, Lumen::CardTileSize);
//			const uint32_t MaxCardTiles = MaxCardTilesX * MaxCardTilesY;
//
//			FRDGBufferRef CardTileAllocator = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(uint32), 1), TEXT("Lumen.Radiosity.CardTileAllocator"));
//			FRDGBufferRef CardTiles = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(uint32), MaxCardTiles), TEXT("Lumen.Radiosity.CardTiles"));
//			AddClearUAVPass(GraphBuilder, GraphBuilder.CreateUAV(CardTileAllocator), 0);
//
//			// Setup common radiosity tracing parameters
//			FLumenRadiosityTexelTraceParameters RadiosityTexelTraceParameters;
//			{
//				RadiosityTexelTraceParameters.CardTileAllocator = GraphBuilder.CreateSRV(CardTileAllocator);
//				RadiosityTexelTraceParameters.CardTileData = GraphBuilder.CreateSRV(CardTiles);
//				RadiosityTexelTraceParameters.TraceRadianceAtlas = TraceRadianceAtlas;
//				RadiosityTexelTraceParameters.TraceHitDistanceAtlas = TraceHitDistanceAtlas;
//				RadiosityTexelTraceParameters.RadiosityAtlasSize = LumenSceneData.GetRadiosityAtlasSize();
//				RadiosityTexelTraceParameters.ProbeSpacingInRadiosityTexels = ProbeSpacing;
//				RadiosityTexelTraceParameters.ProbeSpacingInRadiosityTexelsDivideShift = FMath::FloorLog2(ProbeSpacing);
//				RadiosityTexelTraceParameters.RadiosityTileSize = RadiosityTileSize;
//				RadiosityTexelTraceParameters.HemisphereProbeResolution = HemisphereProbeResolution;
//				RadiosityTexelTraceParameters.NumTracesPerProbe = HemisphereProbeResolution * HemisphereProbeResolution;
//				RadiosityTexelTraceParameters.UseProbeOcclusion = bUseProbeOcclusion ? 1 : 0;
//				RadiosityTexelTraceParameters.FixedJitterIndex = GLumenRadiosityFixedJitterIndex;
//				RadiosityTexelTraceParameters.MaxFramesAccumulated = LumenRadiosity::UseTemporalAccumulation() ? GLumenRadiosityTemporalMaxFramesAccumulated : 1;
//			}
//
//			// Build a list of radiosity tiles for future processing
//			{
//				FBuildRadiosityTilesCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FBuildRadiosityTilesCS::FParameters>();
//				PassParameters->IndirectArgBuffer = CardUpdateContext.DispatchCardPageIndicesIndirectArgs;
//				PassParameters->View = View.ViewUniformBuffer;
//				PassParameters->LumenCardScene = TracingInputs.LumenCardSceneUniformBuffer;
//				PassParameters->RWCardTileAllocator = GraphBuilder.CreateUAV(CardTileAllocator);
//				PassParameters->RWCardTileData = GraphBuilder.CreateUAV(CardTiles);
//				PassParameters->CardPageIndexAllocator = GraphBuilder.CreateSRV(CardUpdateContext.CardPageIndexAllocator);
//				PassParameters->CardPageIndexData = GraphBuilder.CreateSRV(CardUpdateContext.CardPageIndexData);
//				auto ComputeShader = View.ShaderMap->GetShader<FBuildRadiosityTilesCS>();
//
//				FComputeShaderUtils::AddPass(
//					GraphBuilder,
//					RDG_EVENT_NAME("BuildRadiosityTiles"),
//					ComputeShader,
//					PassParameters,
//					CardUpdateContext.DispatchCardPageIndicesIndirectArgs,
//					FLumenCardUpdateContext::EIndirectArgOffset::ThreadPerTile);
//			}
//
//			FRDGBufferRef RadiosityIndirectArgs = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateIndirectDesc<FRHIDispatchIndirectParameters>((int)ERadiosityIndirectArgs::MAX), TEXT("Lumen.RadiosityIndirectArgs"));
//
//			// Setup indirect args for future passes
//			{
//				FLumenRadiosityIndirectArgsCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FLumenRadiosityIndirectArgsCS::FParameters>();
//				PassParameters->RWIndirectArgs = GraphBuilder.CreateUAV(RadiosityIndirectArgs);
//				PassParameters->RadiosityTexelTraceParameters = RadiosityTexelTraceParameters;
//
//				auto ComputeShader = View.ShaderMap->GetShader<FLumenRadiosityIndirectArgsCS>();
//
//				FComputeShaderUtils::AddPass(
//					GraphBuilder,
//					RDG_EVENT_NAME("IndirectArgs"),
//					ComputeShader,
//					PassParameters,
//					FIntVector(1, 1, 1));
//			}
//
//			// Trace rays from surface cache texels
//			if (Lumen::UseHardwareRayTracedRadiosity(*View.Family))
//			{
//#if RHI_RAYTRACING
//
//				FLumenRadiosityHardwareRayTracingRGS::FParameters* PassParameters = GraphBuilder.AllocParameters<FLumenRadiosityHardwareRayTracingRGS::FParameters>();
//				SetLumenHardwareRayTracingSharedParameters(
//					GraphBuilder,
//					GetSceneTextureParameters(GraphBuilder),
//					View,
//					TracingInputs,
//					&PassParameters->SharedParameters
//				);
//				PassParameters->HardwareRayTracingIndirectArgs = RadiosityIndirectArgs;
//
//				PassParameters->RadiosityTexelTraceParameters = RadiosityTexelTraceParameters;
//				PassParameters->RWTraceRadianceAtlas = GraphBuilder.CreateUAV(TraceRadianceAtlas);
//				PassParameters->RWTraceHitDistanceAtlas = GraphBuilder.CreateUAV(TraceHitDistanceAtlas);
//
//				const uint32 NumThreadsToDispatch = GRHIPersistentThreadGroupCount * FLumenRadiosityHardwareRayTracingRGS::GetGroupSize();
//				PassParameters->NumThreadsToDispatch = NumThreadsToDispatch;
//				PassParameters->SurfaceBias = FMath::Clamp(GLumenRadiosityHardwareRayTracingSurfaceSlopeBias, 0.0f, 1000.0f);
//				PassParameters->HeightfieldSurfaceBias = Lumen::GetHeightfieldReceiverBias();
//				PassParameters->AvoidSelfIntersectionTraceDistance = FMath::Clamp(GLumenRadiosityAvoidSelfIntersectionTraceDistance, 0.0f, 1000000.0f);
//				PassParameters->MaxRayIntensity = FMath::Clamp(GLumenRadiosityMaxRayIntensity, 0.0f, 1000000.0f);
//				PassParameters->MinTraceDistance = FMath::Clamp(GLumenRadiosityHardwareRayTracingSurfaceBias, 0.0f, 1000.0f);
//				PassParameters->MaxTraceDistance = Lumen::GetMaxTraceDistance(View);
//				PassParameters->MinTraceDistanceToSampleSurface = GLumenRadiosityMinTraceDistanceToSampleSurface;
//				PassParameters->MaxTranslucentSkipCount = Lumen::GetMaxTranslucentSkipCount();
//				PassParameters->MaxTraversalIterations = LumenHardwareRayTracing::GetMaxTraversalIterations();
//
//				FLumenRadiosityHardwareRayTracingRGS::FPermutationDomain PermutationVector;
//				PermutationVector.Set<FLumenRadiosityHardwareRayTracingRGS::FIndirectDispatchDim>(IsHardwareRayTracingRadiosityIndirectDispatch());
//				PermutationVector.Set<FLumenRadiosityHardwareRayTracingRGS::FAvoidSelfIntersectionTrace>(GLumenRadiosityAvoidSelfIntersectionTraceDistance > 0.0f);
//				TShaderRef<FLumenRadiosityHardwareRayTracingRGS> RayGenerationShader = View.ShaderMap->GetShader<FLumenRadiosityHardwareRayTracingRGS>(PermutationVector);
//
//				ClearUnusedGraphResources(RayGenerationShader, PassParameters);
//
//				const FIntPoint DispatchResolution = FIntPoint(NumThreadsToDispatch, 1);
//				FString Resolution = FString::Printf(TEXT("%ux%u"), DispatchResolution.X, DispatchResolution.Y);
//				if (IsHardwareRayTracingRadiosityIndirectDispatch())
//				{
//					Resolution = FString::Printf(TEXT("<indirect>"));
//				}
//				GraphBuilder.AddPass(
//					RDG_EVENT_NAME("HardwareRayTracing %s %ux%u probes at %u spacing", *Resolution, HemisphereProbeResolution, HemisphereProbeResolution, ProbeSpacing),
//					PassParameters,
//					ERDGPassFlags::Compute,
//					[PassParameters, &View, RayGenerationShader, DispatchResolution](FRHIRayTracingCommandList& RHICmdList)
//					{
//						FRayTracingShaderBindingsWriter GlobalResources;
//						SetShaderParameters(GlobalResources, RayGenerationShader, *PassParameters);
//
//						FRHIRayTracingScene* RayTracingSceneRHI = View.GetRayTracingSceneChecked();
//						FRayTracingPipelineState* RayTracingPipeline = View.LumenHardwareRayTracingMaterialPipeline;
//
//						if (IsHardwareRayTracingRadiosityIndirectDispatch())
//						{
//							PassParameters->HardwareRayTracingIndirectArgs->MarkResourceAsUsed();
//							RHICmdList.RayTraceDispatchIndirect(RayTracingPipeline, RayGenerationShader.GetRayTracingShader(), RayTracingSceneRHI, GlobalResources,
//								PassParameters->HardwareRayTracingIndirectArgs->GetIndirectRHICallBuffer(), (int)ERadiosityIndirectArgs::HardwareRayTracingThreadPerTrace);
//						}
//						else
//						{
//							RHICmdList.RayTraceDispatch(RayTracingPipeline, RayGenerationShader.GetRayTracingShader(), RayTracingSceneRHI, GlobalResources,
//								DispatchResolution.X, DispatchResolution.Y);
//						}
//					}
//				);
//#endif
//			}
//			else
//			{
//				FLumenRadiosityDistanceFieldTracingCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FLumenRadiosityDistanceFieldTracingCS::FParameters>();
//				PassParameters->IndirectArgs = RadiosityIndirectArgs;
//				PassParameters->RadiosityTexelTraceParameters = RadiosityTexelTraceParameters;
//				PassParameters->RWTraceRadianceAtlas = GraphBuilder.CreateUAV(TraceRadianceAtlas);
//				PassParameters->RWTraceHitDistanceAtlas = GraphBuilder.CreateUAV(TraceHitDistanceAtlas);
//
//				GetLumenCardTracingParameters(View, TracingInputs, PassParameters->TracingParameters);
//				SetupLumenDiffuseTracingParametersForProbe(View, PassParameters->IndirectTracingParameters, 0.0f);
//				PassParameters->IndirectTracingParameters.SurfaceBias = FMath::Clamp(GLumenRadiosityDistanceFieldSurfaceSlopeBias, 0.0f, 1000.0f);
//				PassParameters->IndirectTracingParameters.MinTraceDistance = FMath::Clamp(GLumenRadiosityDistanceFieldSurfaceBias, 0.0f, 1000.0f);
//				PassParameters->IndirectTracingParameters.MaxTraceDistance = Lumen::GetMaxTraceDistance(View);
//				PassParameters->IndirectTracingParameters.VoxelStepFactor = FMath::Clamp(GLumenRadiosityVoxelStepFactor, 0.1f, 10.0f);
//				PassParameters->MaxRayIntensity = FMath::Clamp(GLumenRadiosityMaxRayIntensity, 0.0f, 1000000.0f);
//
//				FLumenRadiosityDistanceFieldTracingCS::FPermutationDomain PermutationVector;
//				PermutationVector.Set<FLumenRadiosityDistanceFieldTracingCS::FTraceGlobalSDF>(Lumen::UseGlobalSDFTracing(*View.Family));
//				auto ComputeShader = View.ShaderMap->GetShader<FLumenRadiosityDistanceFieldTracingCS>(PermutationVector);
//
//				FComputeShaderUtils::AddPass(
//					GraphBuilder,
//					RDG_EVENT_NAME("DistanceFieldTracing %ux%u probes at %u spacing", HemisphereProbeResolution, HemisphereProbeResolution, ProbeSpacing),
//					ComputeShader,
//					PassParameters,
//					RadiosityIndirectArgs,
//					(int)ERadiosityIndirectArgs::ThreadPerTrace);
//			}
//
//			FRDGTextureRef RadiosityProbeSHRedAtlas = RegisterOrCreateRadiosityAtlas(
//				GraphBuilder,
//				LumenSceneData.RadiosityProbeSHRedAtlas,
//				TEXT("Lumen.Radiosity.ProbeSHRedAtlas"),
//				RadiosityProbeAtlasSize,
//				PF_FloatRGBA);
//
//			FRDGTextureRef RadiosityProbeSHGreenAtlas = RegisterOrCreateRadiosityAtlas(
//				GraphBuilder,
//				LumenSceneData.RadiosityProbeSHGreenAtlas,
//				TEXT("Lumen.Radiosity.ProbeSHGreenAtlas"),
//				RadiosityProbeAtlasSize,
//				PF_FloatRGBA);
//
//			FRDGTextureRef RadiosityProbeSHBlueAtlas = RegisterOrCreateRadiosityAtlas(
//				GraphBuilder,
//				LumenSceneData.RadiosityProbeSHBlueAtlas,
//				TEXT("Lumen.Radiosity.ProbeSHBlueAtlas"),
//				RadiosityProbeAtlasSize,
//				PF_FloatRGBA);
//
//			if (GLumenRadiositySpatialFilterProbes && GLumenRadiositySpatialFilterProbesKernelSize > 0)
//			{
//				{
//					//@todo - use temporary buffer based off of CardUpdateContext.UpdateAtlasSize which is smaller
//					FRDGTextureRef FilteredTraceRadianceAtlas = GraphBuilder.CreateTexture(
//						FRDGTextureDesc::Create2D(RadiosityProbeTracingAtlasSize, PF_FloatRGB, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV),
//						TEXT("Lumen.Radiosity.FilteredTraceRadianceAtlas"));
//
//					FLumenRadiositySpatialFilterProbeRadiance::FParameters* PassParameters = GraphBuilder.AllocParameters<FLumenRadiositySpatialFilterProbeRadiance::FParameters>();
//					PassParameters->RWFilteredTraceRadianceAtlas = GraphBuilder.CreateUAV(FilteredTraceRadianceAtlas);
//					PassParameters->IndirectArgs = RadiosityIndirectArgs;
//					PassParameters->View = View.ViewUniformBuffer;
//					PassParameters->LumenCardScene = TracingInputs.LumenCardSceneUniformBuffer;
//					PassParameters->RadiosityTexelTraceParameters = RadiosityTexelTraceParameters;
//					PassParameters->ProbePlaneWeightingDepthScale = GRadiosityProbePlaneWeightingDepthScale;
//
//					FLumenRadiositySpatialFilterProbeRadiance::FPermutationDomain PermutationVector;
//					PermutationVector.Set<FLumenRadiositySpatialFilterProbeRadiance::FPlaneWeighting>(GRadiosityFilteringProbePlaneWeighting != 0);
//					PermutationVector.Set<FLumenRadiositySpatialFilterProbeRadiance::FProbeOcclusion>(bUseProbeOcclusion);
//					PermutationVector.Set<FLumenRadiositySpatialFilterProbeRadiance::FKernelSize>(FMath::Clamp<int32>(GLumenRadiositySpatialFilterProbesKernelSize, 0, 2));
//					auto ComputeShader = View.ShaderMap->GetShader<FLumenRadiositySpatialFilterProbeRadiance>(PermutationVector);
//
//					FComputeShaderUtils::AddPass(
//						GraphBuilder,
//						RDG_EVENT_NAME("SpatialFilterProbes"),
//						ComputeShader,
//						PassParameters,
//						RadiosityIndirectArgs,
//						(int)ERadiosityIndirectArgs::ThreadPerTrace);
//
//					RadiosityTexelTraceParameters.TraceRadianceAtlas = FilteredTraceRadianceAtlas;
//				}
//			}
//
//			// Convert traces to SH and store in persistent SH atlas
//			{
//				FLumenRadiosityConvertToSH::FParameters* PassParameters = GraphBuilder.AllocParameters<FLumenRadiosityConvertToSH::FParameters>();
//				PassParameters->RWRadiosityProbeSHRedAtlas = GraphBuilder.CreateUAV(RadiosityProbeSHRedAtlas);
//				PassParameters->RWRadiosityProbeSHGreenAtlas = GraphBuilder.CreateUAV(RadiosityProbeSHGreenAtlas);
//				PassParameters->RWRadiosityProbeSHBlueAtlas = GraphBuilder.CreateUAV(RadiosityProbeSHBlueAtlas);
//				PassParameters->IndirectArgs = RadiosityIndirectArgs;
//				PassParameters->View = View.ViewUniformBuffer;
//				PassParameters->LumenCardScene = TracingInputs.LumenCardSceneUniformBuffer;
//				PassParameters->RadiosityTexelTraceParameters = RadiosityTexelTraceParameters;
//
//				auto ComputeShader = View.ShaderMap->GetShader<FLumenRadiosityConvertToSH>();
//
//				FComputeShaderUtils::AddPass(
//					GraphBuilder,
//					RDG_EVENT_NAME("ConvertToSH"),
//					ComputeShader,
//					PassParameters,
//					RadiosityIndirectArgs,
//					(int)ERadiosityIndirectArgs::ThreadPerProbe);
//			}
//
//			{
//				FLumenRadiosityIntegrateCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FLumenRadiosityIntegrateCS::FParameters>();
//				PassParameters->IndirectArgs = RadiosityIndirectArgs;
//				PassParameters->View = View.ViewUniformBuffer;
//				PassParameters->LumenCardScene = TracingInputs.LumenCardSceneUniformBuffer;
//				PassParameters->RadiosityTexelTraceParameters = RadiosityTexelTraceParameters;
//				PassParameters->RWRadiosityAtlas = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(RadiosityAtlas));
//				PassParameters->RWRadiosityNumFramesAccumulatedAtlas = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(RadiosityNumFramesAccumulatedAtlas));
//				PassParameters->RadiosityProbeSHRedAtlas = RadiosityProbeSHRedAtlas;
//				PassParameters->RadiosityProbeSHGreenAtlas = RadiosityProbeSHGreenAtlas;
//				PassParameters->RadiosityProbeSHBlueAtlas = RadiosityProbeSHBlueAtlas;
//				PassParameters->ProbePlaneWeightingDepthScale = GRadiosityProbePlaneWeightingDepthScale;
//
//				FLumenRadiosityIntegrateCS::FPermutationDomain PermutationVector;
//				PermutationVector.Set<FLumenRadiosityIntegrateCS::FPlaneWeighting>(GRadiosityFilteringProbePlaneWeighting != 0);
//				PermutationVector.Set<FLumenRadiosityIntegrateCS::FProbeOcclusion>(bUseProbeOcclusion);
//				PermutationVector.Set<FLumenRadiosityIntegrateCS::FTemporalAccumulation>(LumenRadiosity::UseTemporalAccumulation());
//				auto ComputeShader = View.ShaderMap->GetShader<FLumenRadiosityIntegrateCS>(PermutationVector);
//
//				FComputeShaderUtils::AddPass(
//					GraphBuilder,
//					RDG_EVENT_NAME("Integrate"),
//					ComputeShader,
//					PassParameters,
//					RadiosityIndirectArgs,
//					(int)ERadiosityIndirectArgs::ThreadPerRadiosityTexel);
//			}
//
//			// Note: extracting source TraceRadianceAtlas and not the filtered one
//			LumenSceneData.RadiosityTraceRadianceAtlas = GraphBuilder.ConvertToExternalTexture(TraceRadianceAtlas);
//			LumenSceneData.RadiosityTraceHitDistanceAtlas = GraphBuilder.ConvertToExternalTexture(TraceHitDistanceAtlas);
//			LumenSceneData.RadiosityProbeSHRedAtlas = GraphBuilder.ConvertToExternalTexture(RadiosityProbeSHRedAtlas);
//			LumenSceneData.RadiosityProbeSHGreenAtlas = GraphBuilder.ConvertToExternalTexture(RadiosityProbeSHGreenAtlas);
//			LumenSceneData.RadiosityProbeSHBlueAtlas = GraphBuilder.ConvertToExternalTexture(RadiosityProbeSHBlueAtlas);
//
//
//
//
//		}
//
//
//		uint32_t GetRadiosityProbeSpacing(const FViewInfo& View)
//		{
//			int32_t RadiosityProbeSpacing = GLumenRadiosityProbeSpacing;
//
//			/*if (View.FinalPostProcessSettings.LumenSceneLightingQuality >= 6)
//			{
//				RadiosityProbeSpacing /= 2;
//			}*/
//
//			return Math::RoundUpToPowerOfTwo(Math::Clamp<uint32>(RadiosityProbeSpacing, 1, Lumen::CardTileSize));
//		}
//
//		int32_t GetHemisphereProbeResolution(const FViewInfo& View)
//		{
//			const float LumenSceneLightingQuality = Math::Clamp<float>(View.FinalPostProcessSettings.LumenSceneLightingQuality, .5f, 4.0f);
//			return Math::Clamp<int32>(GLumenRadiosityHemisphereProbeResolution * Math::Sqrt(LumenSceneLightingQuality), 1, 16);
//		}
//
//
//		uint32_t GetRadiosityAtlasDownsampleFactor()
//		{
//			// Must match RADIOSITY_ATLAS_DOWNSAMPLE_FACTOR
//			return 1;
//		}
//
//	}
//
//	
//
//
//}
