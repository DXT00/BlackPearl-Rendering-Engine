#pragma once
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Config.h"
#include "AnimatedModelRenderer.h"
#include "SkyboxRenderer.h"
#include "BlackPearl/Map/MapManager.h"
namespace BlackPearl {

	class GBufferRenderer:public BasicRenderer
	{

	public:
		GBufferRenderer();
		~GBufferRenderer() {
			GE_SAVE_DELETE(m_AnimatedModelRenderer);
			GE_SAVE_DELETE(m_SkyboxRenderer);

			m_LightPassFrameBuffer->CleanUp();
		};
		void Init(Object* ScreenQuad, Object* surroundSphere, Object* GIQuad);
		void Render(std::vector<Object*> objects, Object* gBufferDebugQuad, LightSources* lightSources);
		void DrawGBuffer(Object* gBufferDebugQuad);
		static float CalculateSphereRadius(Object* pointLight);
		void RenderSceneWithGBufferAndProbes(
			std::vector<Object*> staticObjects, 
			std::vector<Object*> dynamicObjects,
			float timeInSecond, 
			std::vector<Object*> backGroundObjs, 
			Object* gBufferDebugQuad,
			LightSources* lightSources,
			std::vector<Object*>diffuseProbes,
			std::vector<Object*>reflectionProbes,
			std::shared_ptr<Texture>specularBrdfLUTTexture,
			Object* skyBox, 
			MapManager* mapManager);

		std::vector<Object*> FindKnearProbes(glm::vec3 objPos, std::vector<Object*> probes,unsigned int k);
		std::vector<unsigned int> FindKnearAreaProbes(glm::vec3 objPos, std::vector<Object*> probes, unsigned int k,MapManager* mapManager);
		
		static float s_GICoeffs;
		static bool s_HDR;
		static float s_AttenuationItensity;// 100.0f;

	private:
		bool m_IsInitialized = false;
		bool m_SortProbes = false;
		bool m_ProsessGridChanged = false;

		Object* m_FinalScreenQuad = nullptr;
		/* 用于渲染环境全局光照 AmbientGI pass */
		Object* m_GIQuad= nullptr;
		/* pointLight 包围球,只渲染球中的fragment  pontlight pass */
		Object* m_SurroundSphere = nullptr;



		/**** buffer ****/
		std::shared_ptr<GBuffer> m_GBuffer;
		std::shared_ptr<FrameBuffer> m_LightPassFrameBuffer;

		/* 用作 tone mapping 和 hdr 后期处理 */
		std::shared_ptr<FrameBuffer> m_HDRFrameBuffer;
		std::shared_ptr<Texture> m_HDRPostProcessTexture;


		/**** shader ****/
		/* write pos,normal,color to gBuffer */
		std::shared_ptr<Shader> m_GBufferShader;

		/* 用于Debug--画出包围球 */
		std::shared_ptr<Shader> m_SphereDeBugShader;

		/* draw lights according to gBuffer data */
		//分开多个pass来渲染-->去除if分支，GPU对if,for不友好
		std::shared_ptr<Shader> m_AmbientGIPassShader;
		std::shared_ptr<Shader> m_PointLightPassShader;
		std::shared_ptr<Shader> m_DirectionLightPassShader;

		std::shared_ptr<Shader> m_FinalScreenShader;


		unsigned int m_TextureWidth = Configuration::WindowWidth;
		unsigned int m_TexxtureHeight = Configuration::WindowHeight;

		/* 0.0f <m_AttenuationItensity< 256.0f ,m_AttenuationItensity越大，衰减越快，包围球m_SurroundSphere 半径越小 */

		/* m_K near probes for specular objs rendering */
		/*注意 物体blending 多个(m_K个) probes的diffuse SH,但只取最近的一个Probe的specular Map*/
		unsigned int m_K = 3;// 5;// 5;// 5;// 2;
		//

		/*renderer for dynamic objects*/
		AnimatedModelRenderer* m_AnimatedModelRenderer;
		SkyboxRenderer* m_SkyboxRenderer;

	};

}

