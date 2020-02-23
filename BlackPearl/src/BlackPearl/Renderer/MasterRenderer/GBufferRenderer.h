#pragma once
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Config.h"
namespace BlackPearl {
	class GBufferRenderer:public BasicRenderer
	{
	public:
		GBufferRenderer();
		/* ScreenQuad render scene to the screen*/
		void Init(Object* ScreenQuad, Object* surroundSphere);
		void Render(std::vector<Object*> objects, Object* gBufferDebugQuad, LightSources* lightSources);
		void DrawGBuffer(Object* gBufferDebugQuad);
		float CalculateSphereRadius(Object* pointLight);


	private:
		bool m_IsInitialized = false;

		Object* m_ScreenQuad= nullptr;
		/* pointLight 包围球,只渲染球中的fragment */
		Object* m_SurroundSphere = nullptr;

		std::shared_ptr<GBuffer> m_GBffer;
		/* write pos,normal,color to gBuffer */
		std::shared_ptr<Shader> m_GBufferShader;

		/* 用于Debug--画出包围球 */
		std::shared_ptr<Shader> m_SphereDeBugShader;


		/* draw lights according to gBuffer data */
		//分开两个pass来渲染-->去除if分支，GPU对if,for不友好
		std::shared_ptr<Shader> m_PointLightPassShader;
		std::shared_ptr<Shader> m_DirectionLightPassShader;

		unsigned int m_TextureWidth = Configuration::WindowWidth;
		unsigned int m_TexxtureHeight = Configuration::WindowHeight;

		/* 0.0f <m_AttenuationItensity< 256.0f ,m_AttenuationItensity越大，衰减越快，包围球m_SurroundSphere 半径越小 */
		float m_AttenuationItensity = 100.0f;
	};

}

