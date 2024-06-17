#include "pch.h"
#include "LumenRenderer.h"
#include "BlackPearl/Lumen/LumenSceneData.h"
#include "BlackPearl/Lumen/lumenSceneData.h"
#include "BlackPearl/Lumen/Lumen.h"
#include "BlackPearl/Math/Math.h"

namespace BlackPearl {
	void LumenRenderer::RenderLumenSceneLighting(Scene* scene)
	{
		/*LumenSceneData& LumenSceneData = *scene->LumenSceneData;

		if (LumenSceneData.GetNumCardPages() > 0)
		{
			if (LumenSceneData.bDebugClearAllCachedState)
			{
				AddClearRenderTargetPass(GraphBuilder, TracingInputs.DirectLightingAtlas);
				AddClearRenderTargetPass(GraphBuilder, TracingInputs.IndirectLightingAtlas);
				AddClearRenderTargetPass(GraphBuilder, TracingInputs.RadiosityNumFramesAccumulatedAtlas);
				AddClearRenderTargetPass(GraphBuilder, TracingInputs.FinalLightingAtlas);
			}

			FLumenCardUpdateContext DirectLightingCardUpdateContext;
			FLumenCardUpdateContext IndirectLightingCardUpdateContext;

			BuildCardUpdateContext(
				GraphBuilder,
				View,
				LumenSceneData,
				TracingInputs.LumenCardSceneUniformBuffer,
				DirectLightingCardUpdateContext,
				IndirectLightingCardUpdateContext);

			RenderDirectLightingForLumenScene(
				GraphBuilder,
				TracingInputs,
				GlobalShaderMap,
				DirectLightingCardUpdateContext);

			RenderRadiosityForLumenScene(
				GraphBuilder,
				TracingInputs,
				GlobalShaderMap,
				TracingInputs.IndirectLightingAtlas,
				TracingInputs.RadiosityNumFramesAccumulatedAtlas,
				IndirectLightingCardUpdateContext);

			LumenSceneData.DirectLightingAtlas = GraphBuilder.ConvertToExternalTexture(TracingInputs.DirectLightingAtlas);
			LumenSceneData.IndirectLightingAtlas = GraphBuilder.ConvertToExternalTexture(TracingInputs.IndirectLightingAtlas);
			LumenSceneData.RadiosityNumFramesAccumulatedAtlas = GraphBuilder.ConvertToExternalTexture(TracingInputs.RadiosityNumFramesAccumulatedAtlas);
			LumenSceneData.FinalLightingAtlas = GraphBuilder.ConvertToExternalTexture(TracingInputs.FinalLightingAtlas);

			LumenSceneData.bFinalLightingAtlasContentsValid = true;
		}

		ComputeLumenSceneVoxelLighting(GraphBuilder, TracingInputs, GlobalShaderMap);

		ComputeLumenTranslucencyGIVolume(GraphBuilder, TracingInputs, GlobalShaderMap);*/
	}
	/*void LumenRenderer::BuildCardUpdateContext(const LumenSceneData& LumenSceneData, TRDGUniformBufferRef<FLumenCardScene> LumenCardSceneUniformBuffer, FLumenCardUpdateContext& DirectLightingCardUpdateContext, FLumenCardUpdateContext& IndirectLightingCardUpdateContext)
	{
	}*/
	//void LumenRenderer::RenderDirectLightingForLumenScene(Scene* scene, const FLumenCardTracingInputs& TracingInputs, std::map<int, Shader>* GlobalShaderMap, const FLumenCardUpdateContext& CardUpdateContext)
	//{

		////if (GLumenDirectLighting)
		////{
		//	//RDG_EVENT_SCOPE(GraphBuilder, "DirectLighting");
		//	//QUICK_SCOPE_CYCLE_COUNTER(RenderDirectLightingForLumenScene);

		//	const FViewInfo& View = Views[0];
		//	LumenSceneData& LumenSceneData = *scene->LumenSceneData;

		//	TRDGUniformBufferRef<FLumenCardScene> LumenCardSceneUniformBuffer = TracingInputs.LumenCardSceneUniformBuffer;

		//	ClearLumenSceneDirectLighting(
		//		View,
		//		GraphBuilder,
		//		LumenSceneData,
		//		TracingInputs,
		//		CardUpdateContext);

		//	std::vector<FLumenGatheredLight> GatheredLights;

		//	for (auto LightIt = scene->Lights.CreateConstIterator(); LightIt; ++LightIt)
		//	{
		//		const FLightSceneInfoCompact& LightSceneInfoCompact = *LightIt;
		//		const FLightSceneInfo* LightSceneInfo = LightSceneInfoCompact.LightSceneInfo;

		//		if (LightSceneInfo->ShouldRenderLightViewIndependent()
		//			&& LightSceneInfo->ShouldRenderLight(View, true)
		//			&& LightSceneInfo->Proxy->GetIndirectLightingScale() > 0.0f)
		//		{
		//			const FLumenGatheredLight GatheredLight(LightSceneInfo, /*LightIndex*/ GatheredLights.size());
		//			GatheredLights.push_back(GatheredLight);
		//		}
		//	}

		//	FRDGBufferRef LumenPackedLights = CreateLumenLightDataBuffer(GraphBuilder, View, GatheredLights);

		//	FLightTileCullContext CullContext;
		//	CullDirectLightingTiles(View, GraphBuilder, CardUpdateContext, LumenCardSceneUniformBuffer, GatheredLights, LumenPackedLights, CullContext);

		//	// 2 bits per shadow mask texel
		//	const uint32 ShadowMaskTilesSize = Math::Max(4 * CullContext.MaxCulledCardTiles, 1024u);
		//	FRDGBufferRef ShadowMaskTiles = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(uint32), ShadowMaskTilesSize), TEXT("Lumen.DirectLighting.ShadowMaskTiles"));

			//// Apply shadow map
			//{
			//	RDG_EVENT_SCOPE(GraphBuilder, "Shadow map");

			//	FRDGBufferUAVRef ShadowMaskTilesUAV = GraphBuilder.CreateUAV(ShadowMaskTiles, ERDGUnorderedAccessViewFlags::SkipBarrier);

			//	for (int32 LightIndex = 0; LightIndex < GatheredLights.Num(); ++LightIndex)
			//	{
			//		const FLumenGatheredLight& GatheredLight = GatheredLights[LightIndex];
			//		if (GatheredLight.bHasShadows)
			//		{
			//			SampleShadowMap(
			//				GraphBuilder,
			//				Scene,
			//				View,
			//				LumenCardSceneUniformBuffer,
			//				VisibleLightInfos,
			//				VirtualShadowMapArray,
			//				GatheredLight,
			//				CullContext.LightTileScatterParameters,
			//				ShadowMaskTilesUAV);
			//		}
			//	}
			//}

			//// Offscreen shadowing
			//{
			//	RDG_EVENT_SCOPE(GraphBuilder, "Offscreen shadows");

			//	FRDGBufferUAVRef ShadowMaskTilesUAV = GraphBuilder.CreateUAV(ShadowMaskTiles, ERDGUnorderedAccessViewFlags::SkipBarrier);

			//	if (Lumen::UseHardwareRayTracedDirectLighting())
			//	{
			//		TraceLumenHardwareRayTracedDirectLightingShadows(
			//			GraphBuilder,
			//			Scene,
			//			View,
			//			TracingInputs,
			//			CullContext.DispatchLightTilesIndirectArgs,
			//			CullContext.LightTileAllocator,
			//			CullContext.LightTiles,
			//			LumenPackedLights,
			//			ShadowMaskTilesUAV);
			//	}
			//	else
			//	{
			//		for (int32 LightIndex = 0; LightIndex < GatheredLights.Num(); ++LightIndex)
			//		{
			//			const FLumenGatheredLight& GatheredLight = GatheredLights[LightIndex];
			//			if (GatheredLight.bHasShadows)
			//			{
			//				TraceDistanceFieldShadows(
			//					GraphBuilder,
			//					Scene,
			//					View,
			//					LumenCardSceneUniformBuffer,
			//					GatheredLight,
			//					CullContext.LightTileScatterParameters,
			//					ShadowMaskTilesUAV);
			//			}
			//		}
			//	}
			//}

			// Apply lights
		//	{
		//		//RDG_EVENT_SCOPE(GraphBuilder, "Lights");

		//		FRDGBufferSRVRef ShadowMaskTilesSRV = GraphBuilder.CreateSRV(ShadowMaskTiles);

		//		for (int32_t LightIndex = 0; LightIndex < GatheredLights.size(); ++LightIndex)
		//		{
		//			RenderDirectLightIntoLumenCards(
		//				GraphBuilder,
		//				Scene,
		//				View,
		//				TracingInputs,
		//				ViewFamily.EngineShowFlags,
		//				LumenCardSceneUniformBuffer,
		//				GatheredLights[LightIndex],
		//				CullContext.LightTileScatterParameters,
		//				ShadowMaskTilesSRV);
		//		}
		//	}

		//	// Update Final Lighting
		//	CombineLumenSceneLighting(
		//		Scene,
		//		View,
		//		GraphBuilder,
		//		TracingInputs,
		//		CardUpdateContext);
		////}


	//}
	//void LumenRenderer::RenderRadiosityForLumenScene(const FLumenCardTracingInputs& TracingInputs, std::map<int, Shader>* GlobalShaderMap, FRDGTextureRef RadiosityAtlas, FRDGTextureRef RadiosityNumFramesAccumulatedAtlas, const FLumenCardUpdateContext& CardUpdateContext)
	//{
	//}

	//void LumenRenderer::CullDirectLightingTiles(const FLumenCardUpdateContext& CardUpdateContext, TRDGUniformBufferRef<FLumenCardScene> LumenCardSceneUniformBuffer, const std::vector<FLumenGatheredLight>& GatheredLights, FRDGBufferRef LumenPackedLights, FLightTileCullContext& CullContext)
	//{
	//	GE_CORE_INFO("CullTiles %d lights", GatheredLights.size());

	//	const uint32_t MaxLightTilesTilesX = Math::DivideAndRoundUp<uint32_t>(CardUpdateContext.UpdateAtlasSize.x, Lumen::CardTileSize);
	//	const uint32_t MaxLightTilesTilesY = Math::DivideAndRoundUp<uint32_t>(CardUpdateContext.UpdateAtlasSize.y, Lumen::CardTileSize);
	//	const uint32_t MaxLightTiles = MaxLightTilesTilesX * MaxLightTilesTilesY;
	//	const uint32_t NumLightsRoundedUp = (int)(log2(Math::Max((int)GatheredLights.size(), 1)));
	//	const uint32_t MaxLightsPerTile = Math::Max(GLumenDirectLightingMaxLightsPerTile, 1);
	//	const uint32_t MaxCulledCardTiles = MaxLightsPerTile * MaxLightTiles;

	//	FRDGBufferRef CardTileAllocator = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(uint32_t), 1), TEXT("Lumen.DirectLighting.CardTileAllocator"));
	//	FRDGBufferRef CardTiles = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(uint32_t), MaxLightTiles), TEXT("Lumen.DirectLighting.CardTiles"));
	//	AddClearUAVPass(GraphBuilder, GraphBuilder.CreateUAV(CardTileAllocator), 0);

	//	// Splice card pages into card tiles
	//	{
	//		FSpliceCardPagesIntoTilesCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FSpliceCardPagesIntoTilesCS::FParameters>();
	//		PassParameters->IndirectArgBuffer = CardUpdateContext.DispatchCardPageIndicesIndirectArgs;
	//		PassParameters->View = View.ViewUniformBuffer;
	//		PassParameters->LumenCardScene = LumenCardSceneUniformBuffer;
	//		PassParameters->RWCardTileAllocator = GraphBuilder.CreateUAV(CardTileAllocator);
	//		PassParameters->RWCardTiles = GraphBuilder.CreateUAV(CardTiles);
	//		PassParameters->CardPageIndexAllocator = GraphBuilder.CreateSRV(CardUpdateContext.CardPageIndexAllocator);
	//		PassParameters->CardPageIndexData = GraphBuilder.CreateSRV(CardUpdateContext.CardPageIndexData);
	//		auto ComputeShader = View.ShaderMap->GetShader<FSpliceCardPagesIntoTilesCS>();

	//		FComputeShaderUtils::AddPass(
	//			GraphBuilder,
	//			RDG_EVENT_NAME("SpliceCardPagesIntoTiles"),
	//			ComputeShader,
	//			PassParameters,
	//			CardUpdateContext.DispatchCardPageIndicesIndirectArgs,
	//			FLumenCardUpdateContext::EIndirectArgOffset::ThreadPerTile);
	//	}

	//	// Setup indirect args for card tile processing
	//	FRDGBufferRef DispatchCardTilesIndirectArgs = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateIndirectDesc<FRHIDispatchIndirectParameters>(1), TEXT("Lumen.DirectLighting.DispatchCardTilesIndirectArgs"));
	//	{
	//		FInitializeCardTileIndirectArgsCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FInitializeCardTileIndirectArgsCS::FParameters>();
	//		PassParameters->RWDispatchCardTilesIndirectArgs = GraphBuilder.CreateUAV(DispatchCardTilesIndirectArgs);
	//		PassParameters->View = View.ViewUniformBuffer;
	//		PassParameters->CardTileAllocator = GraphBuilder.CreateSRV(CardTileAllocator);

	//		auto ComputeShader = View.ShaderMap->GetShader<FInitializeCardTileIndirectArgsCS>();

	//		FComputeShaderUtils::AddPass(
	//			GraphBuilder,
	//			RDG_EVENT_NAME("InitializeCardTileIndirectArgs"),
	//			ComputeShader,
	//			PassParameters,
	//			FIntVector(1, 1, 1));
	//	}

	//	FRDGBufferRef LightTileAllocator = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(uint32), 1), TEXT("Lumen.DirectLighting.LightTileAllocator"));
	//	FRDGBufferRef LightTiles = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(2 * sizeof(uint32), MaxCulledCardTiles), TEXT("Lumen.DirectLighting.LightTiles"));
	//	AddClearUAVPass(GraphBuilder, GraphBuilder.CreateUAV(LightTileAllocator), 0);

	//	FRDGBufferRef LightTileAllocatorPerLight = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(uint32), NumLightsRoundedUp), TEXT("Lumen.DirectLighting.LightTileAllocatorPerLight"));
	//	FRDGBufferRef LightTileOffsetsPerLight = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(uint32), NumLightsRoundedUp), TEXT("Lumen.DirectLighting.LightTileOffsetsPerLight"));
	//	AddClearUAVPass(GraphBuilder, GraphBuilder.CreateUAV(LightTileAllocatorPerLight), 0);

	//	// Build a list of light tiles for future processing
	//	{
	//		FBuildLightTilesCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FBuildLightTilesCS::FParameters>();
	//		PassParameters->IndirectArgBuffer = DispatchCardTilesIndirectArgs;
	//		PassParameters->View = View.ViewUniformBuffer;
	//		PassParameters->LumenCardScene = LumenCardSceneUniformBuffer;
	//		PassParameters->LumenPackedLights = GraphBuilder.CreateSRV(LumenPackedLights);
	//		PassParameters->RWLightTileAllocator = GraphBuilder.CreateUAV(LightTileAllocator);
	//		PassParameters->RWLightTiles = GraphBuilder.CreateUAV(LightTiles);
	//		PassParameters->RWLightTileAllocatorPerLight = GraphBuilder.CreateUAV(LightTileAllocatorPerLight);
	//		PassParameters->CardTileAllocator = GraphBuilder.CreateSRV(CardTileAllocator);
	//		PassParameters->CardTiles = GraphBuilder.CreateSRV(CardTiles);
	//		PassParameters->MaxLightsPerTile = MaxLightsPerTile;
	//		PassParameters->NumLights = GatheredLights.Num();
	//		auto ComputeShader = View.ShaderMap->GetShader<FBuildLightTilesCS>();

	//		FComputeShaderUtils::AddPass(
	//			GraphBuilder,
	//			RDG_EVENT_NAME("BuildLightTiles"),
	//			ComputeShader,
	//			PassParameters,
	//			DispatchCardTilesIndirectArgs,
	//			0);
	//	}

	//	// Compute prefix sum for card tile array
	//	{
	//		FComputeLightTileOffsetsPerLightCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FComputeLightTileOffsetsPerLightCS::FParameters>();
	//		PassParameters->View = View.ViewUniformBuffer;
	//		PassParameters->RWLightTileOffsetsPerLight = GraphBuilder.CreateUAV(LightTileOffsetsPerLight);
	//		PassParameters->LightTileAllocatorPerLight = GraphBuilder.CreateSRV(LightTileAllocatorPerLight);
	//		PassParameters->NumLights = GatheredLights.Num();

	//		auto ComputeShader = View.ShaderMap->GetShader<FComputeLightTileOffsetsPerLightCS>();

	//		FComputeShaderUtils::AddPass(
	//			GraphBuilder,
	//			RDG_EVENT_NAME("ComputeLightTileOffsetsPerLight"),
	//			ComputeShader,
	//			PassParameters,
	//			FIntVector(1, 1, 1));
	//	}

	//	enum class EDispatchTilesIndirectArgOffset
	//	{
	//		GroupPerTile = 0 * sizeof(FRHIDispatchIndirectParameters),
	//		ThreadPerTile = 0 * sizeof(FRHIDispatchIndirectParameters),
	//		MAX = 2,
	//	};

	//	// Initialize indirect args for culled tiles
	//	FRDGBufferRef DispatchLightTilesIndirectArgs = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateIndirectDesc<FRHIDispatchIndirectParameters>((int32)EDispatchTilesIndirectArgOffset::MAX), TEXT("Lumen.DirectLighting.DispatchLightTilesIndirectArgs"));
	//	FRDGBufferRef DrawTilesPerLightIndirectArgs = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateIndirectDesc<FRHIDispatchIndirectParameters>(NumLightsRoundedUp), TEXT("Lumen.DirectLighting.DrawTilesPerLightIndirectArgs"));
	//	FRDGBufferRef DispatchTilesPerLightIndirectArgs = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateIndirectDesc<FRHIDispatchIndirectParameters>(NumLightsRoundedUp), TEXT("Lumen.DirectLighting.DispatchTilesPerLightIndirectArgs"));
	//	{
	//		FInitializeLightTileIndirectArgsCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FInitializeLightTileIndirectArgsCS::FParameters>();
	//		PassParameters->View = View.ViewUniformBuffer;
	//		PassParameters->RWDispatchLightTilesIndirectArgs = GraphBuilder.CreateUAV(DispatchLightTilesIndirectArgs);
	//		PassParameters->RWDrawTilesPerLightIndirectArgs = GraphBuilder.CreateUAV(DrawTilesPerLightIndirectArgs);
	//		PassParameters->RWDispatchTilesPerLightIndirectArgs = GraphBuilder.CreateUAV(DispatchTilesPerLightIndirectArgs);
	//		PassParameters->LightTileAllocator = GraphBuilder.CreateSRV(LightTileAllocator);
	//		PassParameters->LightTileAllocatorPerLight = GraphBuilder.CreateSRV(LightTileAllocatorPerLight);
	//		PassParameters->VertexCountPerInstanceIndirect = GRHISupportsRectTopology ? 3 : 6;
	//		PassParameters->NumLights = GatheredLights.Num();

	//		auto ComputeShader = View.ShaderMap->GetShader<FInitializeLightTileIndirectArgsCS>();

	//		const FIntVector GroupSize = FComputeShaderUtils::GetGroupCount(GatheredLights.Num(), FInitializeLightTileIndirectArgsCS::GetGroupSize());

	//		FComputeShaderUtils::AddPass(
	//			GraphBuilder,
	//			RDG_EVENT_NAME("InitializeLightTileIndirectArgs"),
	//			ComputeShader,
	//			PassParameters,
	//			GroupSize);
	//	}

	//	// Compact card tile array
	//	{
	//		FRDGBufferRef CompactedLightTiles = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(2 * sizeof(uint32), MaxCulledCardTiles), TEXT("Lumen.DirectLighting.CompactedLightTiles"));
	//		FRDGBufferRef CompactedLightTileAllocatorPerLight = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(uint32), NumLightsRoundedUp), TEXT("Lumen.DirectLighting.CompactedLightTileAllocatorPerLight"));
	//		AddClearUAVPass(GraphBuilder, GraphBuilder.CreateUAV(CompactedLightTileAllocatorPerLight), 0);

	//		FCompactLightTilesCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FCompactLightTilesCS::FParameters>();
	//		PassParameters->IndirectArgBuffer = DispatchLightTilesIndirectArgs;
	//		PassParameters->View = View.ViewUniformBuffer;
	//		PassParameters->RWCompactedLightTiles = GraphBuilder.CreateUAV(CompactedLightTiles);
	//		PassParameters->RWCompactedLightTileAllocatorPerLight = GraphBuilder.CreateUAV(CompactedLightTileAllocatorPerLight);
	//		PassParameters->LightTileAllocator = GraphBuilder.CreateSRV(LightTileAllocator);
	//		PassParameters->LightTiles = GraphBuilder.CreateSRV(LightTiles);
	//		PassParameters->LightTileOffsetsPerLight = GraphBuilder.CreateSRV(LightTileOffsetsPerLight);
	//		PassParameters->NumLights = GatheredLights.Num();

	//		auto ComputeShader = View.ShaderMap->GetShader<FCompactLightTilesCS>();

	//		FComputeShaderUtils::AddPass(
	//			GraphBuilder,
	//			RDG_EVENT_NAME("CompactLightTiles"),
	//			ComputeShader,
	//			PassParameters,
	//			DispatchLightTilesIndirectArgs,
	//			(int32)EDispatchTilesIndirectArgOffset::ThreadPerTile);

	//		LightTiles = CompactedLightTiles;
	//	}

	//	CullContext.LightTileScatterParameters.DrawIndirectArgs = DrawTilesPerLightIndirectArgs;
	//	CullContext.LightTileScatterParameters.DispatchIndirectArgs = DispatchTilesPerLightIndirectArgs;
	//	CullContext.LightTileScatterParameters.LightTileAllocator = GraphBuilder.CreateSRV(LightTileAllocator);
	//	CullContext.LightTileScatterParameters.LightTiles = GraphBuilder.CreateSRV(LightTiles);
	//	CullContext.LightTileScatterParameters.LightTileOffsetsPerLight = GraphBuilder.CreateSRV(LightTileOffsetsPerLight);

	//	CullContext.LightTiles = LightTiles;
	//	CullContext.LightTileAllocator = LightTileAllocator;
	//	CullContext.DispatchLightTilesIndirectArgs = DispatchLightTilesIndirectArgs;
	//	CullContext.MaxCulledCardTiles = MaxCulledCardTiles;
	//}

	//void LumenRenderer::RenderDirectLightIntoLumenCards(const Scene* Scene, const FLumenCardTracingInputs& TracingInputs, const FEngineShowFlags& EngineShowFlags, TRDGUniformBufferRef<FLumenCardScene> LumenCardSceneUniformBuffer, const FLumenGatheredLight& Light, const FLumenLightTileScatterParameters& LightTileScatterParameters, FRDGBufferSRVRef ShadowMaskTilesSRV)
	//{
	//	LumenSceneData& LumenSceneData = *Scene->LumenSceneData;

	//	FLumenCardDirectLighting* PassParameters = GraphBuilder.AllocParameters<FLumenCardDirectLighting>();
	//	{
	//		PassParameters->RenderTargets[0] = FRenderTargetBinding(TracingInputs.DirectLightingAtlas, ERenderTargetLoadAction::ELoad);
	//		PassParameters->VS.LumenCardScene = LumenCardSceneUniformBuffer;
	//		PassParameters->VS.LightTileScatterParameters = LightTileScatterParameters;
	//		PassParameters->VS.LightIndex = Light.LightIndex;

	//		PassParameters->PS.View = View.ViewUniformBuffer;
	//		PassParameters->PS.LumenCardScene = LumenCardSceneUniformBuffer;
	//		Lumen::SetDirectLightingDeferredLightUniformBuffer(View, Light.LightSceneInfo, PassParameters->PS.DeferredLightUniforms);

	//		//SetupLightFunctionParameters(View, Light.LightSceneInfo, 1.0f, PassParameters->PS.LightFunctionParameters);

	//		PassParameters->PS.ShadowMaskTiles = ShadowMaskTilesSRV;

	//		// IES profile
	//		{
	//			FTexture* IESTextureResource = Light.LightSceneInfo->Proxy->GetIESTextureResource();

	//			if (View.Family->EngineShowFlags.TexturedLightProfiles && IESTextureResource)
	//			{
	//				PassParameters->PS.UseIESProfile = 1;
	//				PassParameters->PS.IESTexture = IESTextureResource->TextureRHI;
	//			}
	//			else
	//			{
	//				PassParameters->PS.UseIESProfile = 0;
	//				PassParameters->PS.IESTexture = GWhiteTexture->TextureRHI;
	//			}

	//			PassParameters->PS.IESTextureSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	//		}
	//	}

	//	auto VertexShader = View.ShaderMap->GetShader<FRasterizeToLightTilesVS>();

	//	const FMaterialRenderProxy* LightFunctionMaterialProxy = Light.LightSceneInfo->Proxy->GetLightFunctionMaterial();
	//	bool bUseLightFunction = true;

	//	if (!LightFunctionMaterialProxy
	//		|| !LightFunctionMaterialProxy->GetIncompleteMaterialWithFallback(Scene->GetFeatureLevel()).IsLightFunction()
	//		|| !EngineShowFlags.LightFunctions)
	//	{
	//		bUseLightFunction = false;
	//		LightFunctionMaterialProxy = UMaterial::GetDefaultMaterial(MD_LightFunction)->GetRenderProxy();
	//	}

	//	const bool bUseCloudTransmittance = false;/* SetupLightCloudTransmittanceParameters(
	//		GraphBuilder,
	//		Scene,
	//		View,
	//		GLumenDirectLightingCloudTransmittance != 0 ? Light.LightSceneInfo : nullptr,
	//		PassParameters->PS.LightCloudTransmittanceParameters);*/

	//	FLumenCardDirectLightingPS::FPermutationDomain PermutationVector;
	//	PermutationVector.Set<FLumenCardDirectLightingPS::FLightType>(Light.Type);
	//	PermutationVector.Set<FLumenCardDirectLightingPS::FShadowMask>(Light.bHasShadows);
	//	PermutationVector.Set<FLumenCardDirectLightingPS::FLightFunction>(bUseLightFunction);
	//	PermutationVector.Set<FLumenCardDirectLightingPS::FCloudTransmittance>(bUseCloudTransmittance);
	//	PermutationVector = FLumenCardDirectLightingPS::RemapPermutation(PermutationVector);

	//	const FMaterial& Material = LightFunctionMaterialProxy->GetMaterialWithFallback(Scene->GetFeatureLevel(), LightFunctionMaterialProxy);
	//	const FMaterialShaderMap* MaterialShaderMap = Material.GetRenderingThreadShaderMap();
	//	auto PixelShader = MaterialShaderMap->GetShader<FLumenCardDirectLightingPS>(PermutationVector);

	//	ClearUnusedGraphResources(PixelShader, &PassParameters->PS);

	//	const uint32 DrawIndirectArgOffset = Light.LightIndex * sizeof(FRHIDrawIndirectParameters);

	//	GraphBuilder.AddPass(
	//		RDG_EVENT_NAME("%s", *Light.Name),
	//		PassParameters,
	//		ERDGPassFlags::Raster,
	//		[MaxAtlasSize = LumenSceneData.GetPhysicalAtlasSize(), PassParameters, VertexShader, PixelShader, GlobalShaderMap = View.ShaderMap, LightFunctionMaterialProxy, &Material, &View, DrawIndirectArgOffset](FRHICommandList& RHICmdList)
	//	{
	//		DrawQuadsToAtlas(
	//			MaxAtlasSize,
	//			VertexShader,
	//			PixelShader,
	//			PassParameters,
	//			GlobalShaderMap,
	//			TStaticBlendState<CW_RGBA, BO_Add, BF_One, BF_One>::GetRHI(),
	//			RHICmdList,
	//			[LightFunctionMaterialProxy, &Material, &View](FRHICommandList& RHICmdList, TShaderRefBase<FLumenCardDirectLightingPS, FShaderMapPointerTable> Shader, FRHIPixelShader* ShaderRHI, const FLumenCardDirectLightingPS::FParameters& Parameters)
	//			{
	//				Shader->SetParameters(RHICmdList, ShaderRHI, LightFunctionMaterialProxy, Material, View);
	//			},
	//			PassParameters->VS.LightTileScatterParameters.DrawIndirectArgs,
	//				DrawIndirectArgOffset);
	//	});
	//}
	//void LumenRenderer::CombineLumenSceneLighting(Scene* Scene, const FLumenCardTracingInputs& TracingInputs, const FLumenCardUpdateContext& CardUpdateContext)
	//{
	//	FLumenSceneData& lumenSceneData = *Scene->LumenSceneData;

	//	FLumenCardCombineLighting* PassParameters = GraphBuilder.AllocParameters<FLumenCardCombineLighting>();

	//	PassParameters->RenderTargets[0] = FRenderTargetBinding(TracingInputs.FinalLightingAtlas, ERenderTargetLoadAction::ELoad);
	//	PassParameters->VS.LumenCardScene = TracingInputs.LumenCardSceneUniformBuffer;
	//	PassParameters->VS.DrawIndirectArgs = CardUpdateContext.DrawCardPageIndicesIndirectArgs;
	//	PassParameters->VS.CardPageIndexAllocator = GraphBuilder.CreateSRV(CardUpdateContext.CardPageIndexAllocator);
	//	PassParameters->VS.CardPageIndexData = GraphBuilder.CreateSRV(CardUpdateContext.CardPageIndexData);
	//	PassParameters->VS.IndirectLightingAtlasSize = lumenSceneData.GetRadiosityAtlasSize();
	//	PassParameters->PS.View = View.ViewUniformBuffer;
	//	PassParameters->PS.LumenCardScene = TracingInputs.LumenCardSceneUniformBuffer;
	//	PassParameters->PS.AlbedoAtlas = TracingInputs.AlbedoAtlas;
	//	PassParameters->PS.EmissiveAtlas = TracingInputs.EmissiveAtlas;
	//	PassParameters->PS.DirectLightingAtlas = TracingInputs.DirectLightingAtlas;
	//	PassParameters->PS.IndirectLightingAtlas = TracingInputs.IndirectLightingAtlas;
	//	PassParameters->PS.OpacityAtlas = TracingInputs.OpacityAtlas;
	//	PassParameters->PS.DiffuseReflectivityOverride = LumenSurfaceCache::GetDiffuseReflectivityOverride();

	//	GraphBuilder.AddPass(
	//		RDG_EVENT_NAME("CombineLighting"),
	//		PassParameters,
	//		ERDGPassFlags::Raster,
	//		[ViewportSize = Scene->LumenSceneData->GetPhysicalAtlasSize(), PassParameters, GlobalShaderMap = View.ShaderMap](FRHICommandList& RHICmdList)
	//	{
	//		auto PixelShader = GlobalShaderMap->GetShader<FLumenCardCombineLightingPS>();
	//		auto VertexShader = GlobalShaderMap->GetShader<FRasterizeToCardsVS>();

	//		DrawQuadsToAtlas(
	//			ViewportSize,
	//			VertexShader,
	//			PixelShader,
	//			PassParameters,
	//			GlobalShaderMap,
	//			TStaticBlendState<>::GetRHI(),
	//			RHICmdList,
	//			[](FRHICommandList& RHICmdList, TShaderRefBase<FLumenCardCombineLightingPS, FShaderMapPointerTable> Shader, FRHIPixelShader* ShaderRHI, const typename FLumenCardCombineLightingPS::FParameters& Parameters) {},
	//			PassParameters->VS.DrawIndirectArgs,
	//			0);
	//	});
	//}
	//
	//
	void LumenRenderer::UpdateLumenScene(Scene* scene)
	{
		UpdateCardSceneBuffer(scene);
	}
	void LumenRenderer::UpdateCardSceneBuffer(Scene* scene)
	{
		LumenSceneData& lumenSceneData = *scene->LumenSceneData;

		// CardBuffer
		{
			bool bResourceResized = false;
			{
				const int32_t NumCardEntries = lumenSceneData.m_Cards.size();
				/*const uint32_t CardSceneNumFloat4s = NumCardEntries * FLumenCardGPUData::DataStrideInFloat4s;
				const uint32_t CardSceneNumBytes = Math::DivideAndRoundUp(CardSceneNumFloat4s, 16384u) * 16384 * sizeof(FVector4f);
				bResourceResized = ResizeResourceIfNeeded(RHICmdList, lumenSceneData.CardBuffer, Math::RoundUpToPowerOfTwo(CardSceneNumFloat4s) * sizeof(FVector4f), TEXT("Lumen.Cards"));*/
			}

			//if (GLumenSceneUploadEveryFrame)
			{
				lumenSceneData.m_CardIndicesToUpdateInBuffer.clear();

				for (int32_t i = 0; i < lumenSceneData.m_Cards.size(); i++)
				{
					lumenSceneData.m_CardIndicesToUpdateInBuffer.insert(i);
				}
			}

			const int32_t NumCardDataUploads = lumenSceneData.m_CardIndicesToUpdateInBuffer.size();

			if (NumCardDataUploads > 0)
			{
				LumenCard NullCard;

				//	lumenSceneData.UploadBuffer.Init(NumCardDataUploads, FLumenCardGPUData::DataStrideInBytes, true, TEXT("Lumen.UploadBuffer"));

				//	for (int32_t Index : lumenSceneData.CardIndicesToUpdateInBuffer)
				//	{
				//		if (Index < lumenSceneData.Cards.Num())
				//		{
				//			const LumenCard& Card = lumenSceneData.Cards.IsAllocated(Index) ? lumenSceneData.Cards[Index] : NullCard;

				//			FVector4f* Data = (FVector4f*)lumenSceneData.UploadBuffer.Add_GetRef(Index);
				//			FLumenCardGPUData::FillData(Card, Data);
				//		}
				//	}

				//	RHICmdList.Transition(FRHITransitionInfo(lumenSceneData.CardBuffer.UAV, ERHIAccess::Unknown, ERHIAccess::UAVCompute));
				//	lumenSceneData.UploadBuffer.ResourceUploadTo(RHICmdList, lumenSceneData.CardBuffer, false);
				//	RHICmdList.Transition(FRHITransitionInfo(lumenSceneData.CardBuffer.UAV, ERHIAccess::UAVCompute, ERHIAccess::SRVMask));
				//}
				//else if (bResourceResized)
				//{
				//	RHICmdList.Transition(FRHITransitionInfo(lumenSceneData.CardBuffer.UAV, ERHIAccess::UAVCompute | ERHIAccess::UAVGraphics, ERHIAccess::SRVMask));
				//}
			}

			//UpdateLumenMeshCards(*scene, scene->DistanceFieldSceneData, lumenSceneData, GraphBuilder);

			//const uint32_t MaxUploadBufferSize = 64 * 1024;
			//if (lumenSceneData.UploadBuffer.GetNumBytes() > MaxUploadBufferSize)
			//{
			//	lumenSceneData.UploadBuffer.Release();
			//}
		}

	}

	//void LumenRenderer::UpdateLumenMeshCards(Scene& Scene, const FDistanceFieldSceneData& DistanceFieldSceneData, LumenSceneData& LumenSceneData, FRDGBuilder& GraphBuilder)
	//{
	//	//LLM_SCOPE_BYTAG(Lumen);
	//	//QUICK_SCOPE_CYCLE_COUNTER(UpdateLumenMeshCards);

	//	//FRHICommandListImmediate& RHICmdList = GraphBuilder.RHICmdList;

	//	extern int32_t GLumenSceneUploadEveryFrame;
	//	if (GLumenSceneUploadEveryFrame)
	//	{
	//		LumenSceneData.HeightfieldIndicesToUpdateInBuffer.Reset();
	//		for (int32_t i = 0; i < LumenSceneData.Heightfields.Num(); ++i)
	//		{
	//			LumenSceneData.HeightfieldIndicesToUpdateInBuffer.Add(i);
	//		}

	//		LumenSceneData.MeshCardsIndicesToUpdateInBuffer.Reset();
	//		for (int32_t i = 0; i < LumenSceneData.MeshCards.Num(); ++i)
	//		{
	//			LumenSceneData.MeshCardsIndicesToUpdateInBuffer.Add(i);
	//		}
	//	}

	//	// Upload MeshCards
	//	{
	//		//QUICK_SCOPE_CYCLE_COUNTER(UpdateMeshCards);

	//		const uint32_t NumMeshCards = LumenSceneData.MeshCards.Num();
	//		const uint32_t MeshCardsNumFloat4s = Math::RoundUpToPowerOfTwo(NumMeshCards * FLumenMeshCardsGPUData::DataStrideInFloat4s);
	//		const uint32_t MeshCardsNumBytes = MeshCardsNumFloat4s * sizeof(FVector4f);
	//		const bool bResourceResized = ResizeResourceIfNeeded(RHICmdList, LumenSceneData.MeshCardsBuffer, MeshCardsNumBytes, TEXT("Lumen.MeshCards"));

	//		const int32_t NumMeshCardsUploads = LumenSceneData.MeshCardsIndicesToUpdateInBuffer.Num();

	//		if (NumMeshCardsUploads > 0)
	//		{
	//			FLumenMeshCards NullMeshCards;
	//			NullMeshCards.Initialize(FMatrix::Identity, FBox(FVector(-1.0f), FVector(-1.0f)), -1, 0, 0, false, false, false);

	//			LumenSceneData.UploadBuffer.Init(NumMeshCardsUploads, FLumenMeshCardsGPUData::DataStrideInBytes, true, TEXT("Lumen.UploadBuffer"));

	//			for (int32_t Index : LumenSceneData.MeshCardsIndicesToUpdateInBuffer)
	//			{
	//				if (Index < LumenSceneData.MeshCards.Num())
	//				{
	//					const FLumenMeshCards& MeshCards = LumenSceneData.MeshCards.IsAllocated(Index) ? LumenSceneData.MeshCards[Index] : NullMeshCards;

	//					FVector4f* Data = (FVector4f*)LumenSceneData.UploadBuffer.Add_GetRef(Index);
	//					FLumenMeshCardsGPUData::FillData(MeshCards, Data);
	//				}
	//			}

	//			RHICmdList.Transition(FRHITransitionInfo(LumenSceneData.MeshCardsBuffer.UAV, ERHIAccess::Unknown, ERHIAccess::UAVCompute));
	//			LumenSceneData.UploadBuffer.ResourceUploadTo(RHICmdList, LumenSceneData.MeshCardsBuffer, false);
	//			RHICmdList.Transition(FRHITransitionInfo(LumenSceneData.MeshCardsBuffer.UAV, ERHIAccess::UAVCompute, ERHIAccess::SRVMask));
	//		}
	//		else if (bResourceResized)
	//		{
	//			RHICmdList.Transition(FRHITransitionInfo(LumenSceneData.MeshCardsBuffer.UAV, ERHIAccess::UAVCompute | ERHIAccess::UAVGraphics, ERHIAccess::SRVMask));
	//		}
	//	}

	//	// Upload Heightfields
	//	{
	//		QUICK_SCOPE_CYCLE_COUNTER(UpdateHeightfields);

	//		const uint32_t NumHeightfields = LumenSceneData.Heightfields.Num();
	//		const uint32_t HeightfieldsNumFloat4s = Math::RoundUpToPowerOfTwo(NumHeightfields * FLumenHeightfieldGPUData::DataStrideInFloat4s);
	//		const uint32_t HeightfieldsNumBytes = HeightfieldsNumFloat4s * sizeof(FVector4f);
	//		const bool bResourceResized = ResizeResourceIfNeeded(RHICmdList, LumenSceneData.HeightfieldBuffer, HeightfieldsNumBytes, TEXT("Lumen.HeigthfieldBuffer"));

	//		const int32_t NumHeightfieldsUploads = LumenSceneData.HeightfieldIndicesToUpdateInBuffer.Num();

	//		if (NumHeightfieldsUploads > 0)
	//		{
	//			FLumenHeightfield NullHeightfield;

	//			LumenSceneData.UploadBuffer.Init(NumHeightfieldsUploads, FLumenHeightfieldGPUData::DataStrideInBytes, true, TEXT("Lumen.UploadBuffer"));

	//			for (int32_t Index : LumenSceneData.HeightfieldIndicesToUpdateInBuffer)
	//			{
	//				if (Index < LumenSceneData.Heightfields.Num())
	//				{
	//					const FLumenHeightfield& Heightfield = LumenSceneData.Heightfields.IsAllocated(Index) ? LumenSceneData.Heightfields[Index] : NullHeightfield;

	//					FVector4f* Data = (FVector4f*)LumenSceneData.UploadBuffer.Add_GetRef(Index);
	//					FLumenHeightfieldGPUData::FillData(Heightfield, LumenSceneData.MeshCards, Data);
	//				}
	//			}

	//			RHICmdList.Transition(FRHITransitionInfo(LumenSceneData.HeightfieldBuffer.UAV, ERHIAccess::Unknown, ERHIAccess::UAVCompute));
	//			LumenSceneData.UploadBuffer.ResourceUploadTo(RHICmdList, LumenSceneData.HeightfieldBuffer, false);
	//			RHICmdList.Transition(FRHITransitionInfo(LumenSceneData.HeightfieldBuffer.UAV, ERHIAccess::UAVCompute, ERHIAccess::SRVMask));
	//		}
	//		else if (bResourceResized)
	//		{
	//			RHICmdList.Transition(FRHITransitionInfo(LumenSceneData.HeightfieldBuffer.UAV, ERHIAccess::UAVCompute | ERHIAccess::UAVGraphics, ERHIAccess::SRVMask));
	//		}
	//	}

	//	// Upload SceneInstanceIndexToMeshCardsIndexBuffer
	//	{
	//		QUICK_SCOPE_CYCLE_COUNTER(UpdateSceneInstanceIndexToMeshCardsIndexBuffer);

	//		if (GLumenSceneUploadEveryFrame)
	//		{
	//			LumenSceneData.PrimitivesToUpdateMeshCards.Reset();

	//			for (int32_t PrimitiveIndex = 0; PrimitiveIndex < Scene.Primitives.Num(); ++PrimitiveIndex)
	//			{
	//				LumenSceneData.PrimitivesToUpdateMeshCards.Add(PrimitiveIndex);
	//			}
	//		}

	//		const int32_t NumIndices = Math::Max(Math::RoundUpToPowerOfTwo(Scene.GPUScene.InstanceSceneDataAllocator.GetMaxSize()), 1024u);
	//		const uint32_t IndexSizeInBytes = GPixelFormats[PF_R32_UINT].BlockBytes;
	//		const uint32_t IndicesSizeInBytes = NumIndices * IndexSizeInBytes;
	//		ResizeResourceIfNeeded(RHICmdList, LumenSceneData.SceneInstanceIndexToMeshCardsIndexBuffer, IndicesSizeInBytes, TEXT("SceneInstanceIndexToMeshCardsIndexBuffer"));

	//		uint32_t NumIndexUploads = 0;

	//		for (int32_t PrimitiveIndex : LumenSceneData.PrimitivesToUpdateMeshCards)
	//		{
	//			if (PrimitiveIndex < Scene.Primitives.Num())
	//			{
	//				const FPrimitiveSceneInfo* PrimitiveSceneInfo = Scene.Primitives[PrimitiveIndex];
	//				NumIndexUploads += PrimitiveSceneInfo->GetNumInstanceSceneDataEntries();
	//			}
	//		}

	//		if (NumIndexUploads > 0)
	//		{
	//			LumenSceneData.ByteBufferUploadBuffer.Init(NumIndexUploads, IndexSizeInBytes, false, TEXT("LumenUploadBuffer"));

	//			for (int32_t PrimitiveIndex : LumenSceneData.PrimitivesToUpdateMeshCards)
	//			{
	//				if (PrimitiveIndex < Scene.Primitives.Num())
	//				{
	//					const FPrimitiveSceneInfo* PrimitiveSceneInfo = Scene.Primitives[PrimitiveIndex];
	//					const int32_t NumInstances = PrimitiveSceneInfo->GetNumInstanceSceneDataEntries();

	//					for (int32_t InstanceIndex = 0; InstanceIndex < NumInstances; ++InstanceIndex)
	//					{
	//						const int32_t MeshCardsIndex = LumenSceneData.GetMeshCardsIndex(PrimitiveSceneInfo, InstanceIndex);

	//						LumenSceneData.ByteBufferUploadBuffer.Add(PrimitiveSceneInfo->GetInstanceSceneDataOffset() + InstanceIndex, &MeshCardsIndex);
	//					}
	//				}
	//			}

	//			RHICmdList.Transition(FRHITransitionInfo(LumenSceneData.SceneInstanceIndexToMeshCardsIndexBuffer.UAV, ERHIAccess::Unknown, ERHIAccess::UAVCompute));
	//			LumenSceneData.ByteBufferUploadBuffer.ResourceUploadTo(RHICmdList, LumenSceneData.SceneInstanceIndexToMeshCardsIndexBuffer, false);
	//			RHICmdList.Transition(FRHITransitionInfo(LumenSceneData.SceneInstanceIndexToMeshCardsIndexBuffer.UAV, ERHIAccess::UAVCompute, ERHIAccess::SRVMask));
	//		}
	//	}

	//	// Reset arrays, but keep allocated memory for 1024 elements
	//	LumenSceneData.HeightfieldIndicesToUpdateInBuffer.Empty(1024);
	//	LumenSceneData.MeshCardsIndicesToUpdateInBuffer.Empty(1024);
	//	LumenSceneData.PrimitivesToUpdateMeshCards.Empty(1024);
	//}
}