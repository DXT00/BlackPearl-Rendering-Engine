#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/MainCamera/MainCamera.h"

namespace BlackPearl {
	class RayTracingRenderer:public BasicRenderer
	{
	public:
		RayTracingRenderer();
		~RayTracingRenderer() = default;
		void Init(Object* quad);
		void Render();
		void RenderSpheres(MainCamera* mainCamera);
	private:
		std::shared_ptr<Shader> m_BasicShader;
		std::shared_ptr<Shader> m_BasicSystemShader;

		std::shared_ptr<Shader> m_ScreenShader;
		std::shared_ptr<GBuffer> m_GBuffers[2];
		bool m_ReadBuffer = false;
		bool m_WriteBuffer = true;

		int m_LoopNum = 1;
		float m_RayNumMax = 10000.0f;


		/* Object */
		Object* m_Quad;

	};

}

