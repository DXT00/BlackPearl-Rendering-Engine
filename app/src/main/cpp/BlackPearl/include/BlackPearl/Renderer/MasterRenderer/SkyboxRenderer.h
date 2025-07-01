#pragma once
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Shader/MaterialShader.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "hlsl/core/sky_cb.h"
#include "Component/LightComponent/DirectionLight.h"

namespace BlackPearl {

	struct SkyParameters
	{
		math::float3 skyColor{ 0.17f, 0.37f, 0.65f };
		math::float3 horizonColor{ 0.50f, 0.70f, 0.92f };
		math::float3 groundColor{ 1.0f, 1.0f, 1.0f };
		math::float3 directionUp{ 0.f, 1.f, 0.f };
		float brightness = 0.1f; // scaler for sky brightness
		float horizonSize = 30.f; // +/- degrees
		float glowSize = 5.f; // degrees, starting from the edge of the light disk
		float glowIntensity = 0.1f; // [0-1] relative to light intensity
		float glowSharpness = 4.f; // [1-10] is the glow power exponent
		float maxLightRadiance = 100.f; // clamp for light radiance derived from its angular size, 0 = no clamp
	};

	class SkyboxRenderer: public BasicRenderer
	{
	public:

		SkyboxRenderer(IDevice* device);

		void Init();
		void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);

		void FillShaderParameters(
			const DirectionLight& light,
			const SkyParameters& input,
			SkyConstants& output);
        void SetCustomView(SceneData* view);

        static float m_TotalTimeIntervalS;//second
        static float m_StateIntervalS;

	private:





		MaterialShader* m_SkyboxShader = nullptr;
		TextureHandle   m_SkyboxTexture[3];
		SkyParameters   m_SkyParam;
		BufferHandle    m_SkyCB;

		//TODO:: Skybox Material;
		BindingLayoutHandle m_SkyboxBindingLayout;
		BindingSetHandle    m_SkyboxBindingSet;
		GraphicsPipelineHandle m_SkyboxPso = nullptr;

        SceneData* m_CustumView = nullptr;

	};
}


