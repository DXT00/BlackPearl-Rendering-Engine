#pragma once
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "BlackPearl/Renderer/CommonRenderPass.h"
#include "BlackPearl/Renderer/RenderTargets.h"
#include "BlackPearl/Renderer/MasterRenderer/RayTraceRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/RTXDI/RtxdiResources.h"

namespace BlackPearl {
	class RtxdiRenderer
	{
	public:
		RtxdiRenderer(
			IDevice* device,
			std::shared_ptr<ShaderFactory> shaderFactory,
			std::shared_ptr<CommonRenderPasses> commonRenderPasses,
			BindingLayoutHandle bindlessLayout);
		~RtxdiRenderer();

		void Reset();
		void PrepareResources(
			CommandListHandle commandList,
			const RenderTargets& renderTargets,
			//std::shared_ptr<EnvMapBaker> envMap,
			//EnvMapSceneParams envMapSceneParams,
			//const std::shared_ptr<ExtendedScene> scene,
			//const RtxdiBridgeParameters& bridgeParams,
			const BindingLayoutHandle extraBindingLayout,
			Scene* scene);
		void BeginFrame(
			CommandListHandle commandList,
			const RenderTargets& renderTargets,
			const BindingLayoutHandle extraBindingLayout,
			BindingSetHandle extraBindingSet);
		void Execute(
			CommandListHandle commandList,
			BindingSetHandle extraBindingSet, bool skipFinal);
		void ExecuteGI(CommandListHandle commandList,
			BindingSetHandle extraBindingSet, bool skipFinal);
		void ExecuteFusedDIGIFinal(CommandListHandle commandList,
			BindingSetHandle extraBindingSet);
		void EndFrame();

	private:
		void _ExecuteRayTracingPass(CommandListHandle& commandList,
			RayTraceRenderer& pass,
			const char* passName,
			int2 dispatchSize,
			IBindingSet* extraBindingSet /* = nullptr */);
		//void _CreateRenderTagets();
		void _CreateRTBindingLayout();
		void _CreateRTBindingSets();


	private:
		std::shared_ptr<ShaderFactory> m_ShaderFactory;

		RayTraceRenderer* m_RayTracePass;
		BindingLayoutHandle m_BindingLayout;
		BindingLayoutHandle m_BindlessLayout;
		BindingLayoutHandle m_ExtraBindingLayout;
		RtxdiResources* m_RtxdiResources;

		BufferHandle m_RtxdiConstantBuffer;
		std::shared_ptr<CommonRenderPasses> m_CommonPasses;
		// shared bindings require them
		BindingSetHandle m_BindingSet;
		BindingSetHandle m_PrevBindingSet;
		IDevice* m_Device = nullptr;

		Scene* m_Scene = nullptr;

	};
}

