#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/MainCamera/MainCamera.h"
#include "BlackPearl/Renderer/Material/TextureImage2D.h"
#include "BlackPearl/RayTracing/GenData.h"
#include "BlackPearl/RayTracing/Group.h"

namespace BlackPearl {
	class RayTracingRenderer:public BasicRenderer
	{
	public:
		RayTracingRenderer();
		~RayTracingRenderer() = default;
		void Init(Object* quad);
		void Render();
		void RenderSpheres(MainCamera* mainCamera);
		void RenderMaterialSpheres(MainCamera* mainCamera);

		void InitGroupData(Group* group);
		void RenderGroup(MainCamera* mainCamera, Object* group);
		

	private:
		std::shared_ptr<Shader> m_BasicShader;
		std::shared_ptr<Shader> m_BasicSystemShader;
		std::shared_ptr<Shader> m_MaterialShader;
		std::shared_ptr<Shader> m_GroupShader;

		std::shared_ptr<Shader> m_ScreenShader;
		std::shared_ptr<GBuffer> m_GBuffers[2];
		bool m_ReadBuffer = false;
		bool m_WriteBuffer = true;

		int m_LoopNum = 1;
		float m_RayNumMax = 10000.0f;

		//RenderGroup
		std::vector<float> m_SceneData;
		std::vector<float> m_TextureData;
		std::vector<float> m_MaterialData;
		std::vector<float> m_PackData;

		std::shared_ptr<TextureImage2D> m_SceneDataTex;
		std::shared_ptr<TextureImage2D> m_TexDataTex;
		std::shared_ptr<TextureImage2D> m_MaterialDataTex;
		std::shared_ptr<TextureImage2D> m_PackDataTex;

		std::shared_ptr<GenData> m_GenData;



		/* Object */
		Object* m_Quad;

	};

}

