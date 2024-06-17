#pragma once
#include "BlackPearl/Renderer/Buffer/Buffer.h"
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
			MapManager* mapManager,
			std::shared_ptr<Texture> depthTexture,
			bool enableSSR);

		std::vector<Object*> FindKnearProbes(glm::vec3 objPos, std::vector<Object*> probes,unsigned int k);
		std::vector<unsigned int> FindKnearAreaProbes(glm::vec3 objPos, std::vector<Object*> probes, unsigned int k,MapManager* mapManager);
		
		static float s_GICoeffs;
		static float s_SSRGICoeffs;

		static bool s_HDR;
		static float s_AttenuationItensity;// 100.0f;

	private:
		bool m_IsInitialized = false;
		bool m_SortProbes = false;
		bool m_ProsessGridChanged = false;

		Object* m_FinalScreenQuad = nullptr;
		/* ������Ⱦ����ȫ�ֹ��� AmbientGI pass */
		Object* m_GIQuad= nullptr;
		/* pointLight ��Χ��,ֻ��Ⱦ���е�fragment  pontlight pass */
		Object* m_SurroundSphere = nullptr;



		/**** buffer ****/
		std::shared_ptr<GBuffer> m_GBuffer;
		std::shared_ptr<FrameBuffer> m_LightPassFrameBuffer;

		/* ���� tone mapping �� hdr ���ڴ��� */
		std::shared_ptr<FrameBuffer> m_HDRFrameBuffer;
		std::shared_ptr<Texture> m_HDRPostProcessTexture;

		std::shared_ptr<Texture> m_SSRTestTexture;

		/**** shader ****/
		/* write pos,normal,color to gBuffer */
		std::shared_ptr<Shader> m_GBufferShader;

		/* ����Debug--������Χ�� */
		std::shared_ptr<Shader> m_SphereDeBugShader;

		/* draw lights according to gBuffer data */
		//�ֿ����pass����Ⱦ-->ȥ��if��֧��GPU��if,for���Ѻ�
		std::shared_ptr<Shader> m_SSRPassShader;
		std::shared_ptr<Shader> m_AmbientGIPassShader;
		std::shared_ptr<Shader> m_PointLightPassShader;
		std::shared_ptr<Shader> m_DirectionLightPassShader;

		std::shared_ptr<Shader> m_FinalScreenShader;


		unsigned int m_TextureWidth = Configuration::WindowWidth;
		unsigned int m_TexxtureHeight = Configuration::WindowHeight;

		/* 0.0f <m_AttenuationItensity< 256.0f ,m_AttenuationItensityԽ��˥��Խ�죬��Χ��m_SurroundSphere �뾶ԽС */

		/* m_K near probes for specular objs rendering */
		/*ע�� ����blending ���(m_K��) probes��diffuse SH,��ֻȡ�����һ��Probe��specular Map*/
		unsigned int m_K = 3;// 5;// 5;// 5;// 2;
		//

		/*renderer for dynamic objects*/
		AnimatedModelRenderer* m_AnimatedModelRenderer;
		SkyboxRenderer* m_SkyboxRenderer;


	};

}

