#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Timestep/Timestep.h"

namespace BlackPearl {
	class WaterRenderer
	{
	public:
		struct GridVertex
		{
			glm::vec3 position;
			glm::vec2 texcoords;
		};
		WaterRenderer();
		~WaterRenderer();
		void Init(Scene* scene);

		void Render(Timestep ts, Camera* camera);

	private:
		const int mGridDim = 1024;
		const int mTextureResolution = 512.0f;
		const int mWorkGroupDim = 32.0;

		std::shared_ptr<VertexArray> m_VAO;
		std::shared_ptr<Shader> m_CSInitialSpectrumShader;
		std::shared_ptr<Shader> m_CSPhaseShader;
		std::shared_ptr<Shader> m_CSSpectrumShader;
		std::shared_ptr<Shader> m_CSFFTHorizontalShader;
		std::shared_ptr<Shader> m_CSFFTVerticalShader;
		std::shared_ptr<Shader> m_CSNormalMapShader;

		std::shared_ptr<Shader> m_OceanShader;

		TextureHandle m_InitialSpectrumTexture;
		TextureHandle m_PingPhaseTexture;
		TextureHandle m_PongPhaseTexture;
		TextureHandle m_SpectrumTexture;
		TextureHandle m_TempTexture;

		TextureHandle m_NormalMapTexture;
		bool m_is_ping_phase = true;
		float m_wind_magnitude = 14.142135f;
		float m_wind_angle = 90.f;
		float m_choppiness = 1.5f;
		bool m_wireframe_mode = false;
		int sun_elevation = 0;
		int sun_azimuth = 90;

	};
}
