#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Scene/Scene.h"
#include "BlackPearl/Lumen/LumenSceneData.h"
#include "BlackPearl/Lumen/LumenStruct.h"
#include "BlackPearl/Renderer/Shader/Shader.h"

namespace BlackPearl {
	class LumenRenderer : public BasicRenderer
	{
	public:
		void RenderLumenSceneLighting(Scene* scene);

		//void BuildCardUpdateContext(
		//	const LumenSceneData& LumenSceneData,
		//	TRDGUniformBufferRef<FLumenCardScene> LumenCardSceneUniformBuffer,
		//	FLumenCardUpdateContext& DirectLightingCardUpdateContext,
		//	FLumenCardUpdateContext& IndirectLightingCardUpdateContext
		//);

		//void RenderDirectLightingForLumenScene(
		//	Scene* scene,
		//	const FLumenCardTracingInputs& TracingInputs,
		//	std::map<int, Shader>* GlobalShaderMap,
		//	const FLumenCardUpdateContext& CardUpdateContext
		//);
		//void RenderRadiosityForLumenScene(
		//	const class FLumenCardTracingInputs& TracingInputs,
		//	std::map<int, Shader>* GlobalShaderMap,
		//	FRDGTextureRef RadiosityAtlas,
		//	FRDGTextureRef RadiosityNumFramesAccumulatedAtlas,
		//	const FLumenCardUpdateContext& CardUpdateContext
		//);


		////**************************
		//void CullDirectLightingTiles(
		//	const FLumenCardUpdateContext& CardUpdateContext,
		//	TRDGUniformBufferRef<FLumenCardScene> LumenCardSceneUniformBuffer,
		//	const std::vector<FLumenGatheredLight>& GatheredLights,
		//	FRDGBufferRef LumenPackedLights,
		//	FLightTileCullContext& CullContext);
		//void RenderDirectLightIntoLumenCards(
		//	const Scene* Scene,
		//	const FLumenCardTracingInputs& TracingInputs,
		//	const FEngineShowFlags& EngineShowFlags,
		//	TRDGUniformBufferRef<FLumenCardScene> LumenCardSceneUniformBuffer,
		//	const FLumenGatheredLight& Light,
		//	const FLumenLightTileScatterParameters& LightTileScatterParameters,
		//	FRDGBufferSRVRef ShadowMaskTilesSRV);
		//
		//void CombineLumenSceneLighting(
		//	Scene* Scene,
		//	const FLumenCardTracingInputs& TracingInputs,
		//	const FLumenCardUpdateContext& CardUpdateContext);
		//
		//
		//template<typename VertexShaderType, typename PixelShaderType, typename PassParametersType, typename SetParametersLambdaType>
		//void DrawQuadsToAtlas(
		//	FIntPoint ViewportSize,
		//	TShaderRefBase<VertexShaderType, FShaderMapPointerTable> VertexShader,
		//	TShaderRefBase<PixelShaderType, FShaderMapPointerTable> PixelShader,
		//	const PassParametersType* PassParameters,
		//	FGlobalShaderMap* GlobalShaderMap,
		//	FRHIBlendState* BlendState,
		//	FRHICommandList& RHICmdList,
		//	SetParametersLambdaType&& SetParametersLambda,
		//	FRDGBufferRef DrawIndirectArgs,
		//	uint32 DrawIndirectArgOffset)
		//{
		//	FGraphicsPipelineStateInitializer GraphicsPSOInit;
		//	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

		//	RHICmdList.SetViewport(0, 0, 0.0f, ViewportSize.X, ViewportSize.Y, 1.0f);

		//	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
		//	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		//	GraphicsPSOInit.BlendState = BlendState;

		//	GraphicsPSOInit.PrimitiveType = GRHISupportsRectTopology ? PT_RectList : PT_TriangleList;

		//	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GEmptyVertexDeclaration.VertexDeclarationRHI;
		//	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		//	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

		//	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

		//	SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), PassParameters->VS);
		//	SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), PassParameters->PS);
		//	SetParametersLambda(RHICmdList, PixelShader, PixelShader.GetPixelShader(), PassParameters->PS);

		//	RHICmdList.DrawPrimitiveIndirect(DrawIndirectArgs->GetIndirectRHICallBuffer(), DrawIndirectArgOffset);
		//}


		//主要函数，对应UE void FDeferredShadingSceneRenderer::UpdateLumenScene(FRDGBuilder& GraphBuilder)
		void UpdateLumenScene(Scene* scene);

		void UpdateCardSceneBuffer(Scene *scene);

		//void UpdateLumenMeshCards(Scene& Scene, const FDistanceFieldSceneData& DistanceFieldSceneData, FLumenSceneData& LumenSceneData, FRDGBuilder& GraphBuilder);



	};
}
