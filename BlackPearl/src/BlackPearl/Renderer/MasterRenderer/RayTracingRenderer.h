#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/MainCamera/MainCamera.h"
#include "BlackPearl/Renderer/Material/TextureImage2D.h"
#include "BlackPearl/RayTracing/GenData.h"
#include "BlackPearl/RayTracing/Group.h"
#include "BlackPearl/Scene/SceneBuilder.h"

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

		void InitScene(std::shared_ptr<GenData> scene);
		void RenderGroup(MainCamera* mainCamera, Object* group);
		void RenderBVHNode(MainCamera* mainCamera);
	private:
		std::shared_ptr<SceneBuilder> m_SceneBuilder;
		std::shared_ptr<Shader> m_BasicShader;
		std::shared_ptr<Shader> m_BasicSystemShader;
		std::shared_ptr<Shader> m_MaterialShader;
		std::shared_ptr<Shader> m_GroupShader;
		std::shared_ptr<Shader> m_BVHNodeShader;


		std::shared_ptr<Shader> m_ScreenShader;
		std::shared_ptr<GBuffer> m_GBuffers[2];
		bool m_ReadBuffer = false;
		bool m_WriteBuffer = true;

		int m_LoopNum = 1;
		const size_t m_MaxLoopNum = 5000;
		size_t m_AllLoopNum = 0;
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
		std::shared_ptr<TextureImage2D> m_SkyBoxTex;
		std::map<std::shared_ptr<Texture>, size_t> m_Tex2RenderIdMap;
		std::map<std::shared_ptr<CubeMapTexture>, size_t> m_CubeMap2RenderIdMap;

		std::shared_ptr<GenData> m_GenData;

		/* Object */
		Object* m_Quad;

	};

}

