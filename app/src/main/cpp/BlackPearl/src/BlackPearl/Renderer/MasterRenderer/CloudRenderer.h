#pragma once
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/MainCamera/MainCamera.h"
#include "BlackPearl/Renderer/Material/Texture3D.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Component/BoundingBoxComponent/BoundingBox.h"

namespace BlackPearl {
	class CloudRenderer :public BasicRenderer
	{

	public:
		CloudRenderer();
		
		void Init(Scene* scene);
		void Render(MainCamera* mainCamera, Object* obj, Object* boundingBoxObj, const std::shared_ptr<Texture> postProcessTexture);
		void RenderCombineScene(MainCamera* mainCamera, Object* obj, const std::shared_ptr<Texture> postProcessTexture);
		void RenderScene(MainCamera* mainCamera, Object* obj, const std::shared_ptr<Texture> postProcessTexture);

		std::shared_ptr<Shader> GetShader() { return m_CloudShader; }
		void ShowNoise3DTexture(Object* cube);

		void RenderDepthMap();
		~CloudRenderer();

		static float s_rayStep;
		static float s_step;
		static glm::vec3 s_colA;
		static glm::vec3 s_colB;
		static float s_colorOffset1;
		static float s_colorOffset2;
		static float s_NoiseTexture3DSize;
		static float s_densityOffset;
		static glm::vec3 s_boundsMin;
		static glm::vec3 s_boundsMax;
		static float s_lightAbsorptionTowardSun;
		static float s_densityMultiplier;

	private:
		void _CreateNoise3DTexture();
		Scene* m_Scene;
		std::shared_ptr<Texture> m_DepthTexture;
		std::shared_ptr<Texture> m_CloudTexture;

		std::shared_ptr<Shader> m_CloudShader;
		std::shared_ptr<Shader> m_DepthShader;
		std::shared_ptr<Shader> m_NoiseGenShader;
		std::shared_ptr<Shader> m_NoiseDebugShader;
		std::shared_ptr<Shader> m_SceneRenderShader;// combine cloud and scene

		std::shared_ptr<FrameBuffer> m_DepthFrameBuffer;
		std::shared_ptr<FrameBuffer> m_CloudFrameBuffer;

		std::shared_ptr<Texture> m_WeatherTexture;

		//************** noise ********************/
		//float m_NoiseTexture3DSize = 32.0;
		Texture3D* m_NoiseTexture = nullptr;


	};


}