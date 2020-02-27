#pragma once
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/LightProbes/LightProbes.h"
#include "AnimatedModelRenderer.h"

namespace BlackPearl {
	class GBufferRenderer:public BasicRenderer
	{
	public:
		GBufferRenderer();
		/* ScreenQuad render scene to the screen*/
		void Init(Object* ScreenQuad, Object* surroundSphere, Object* GIQuad);
		void Render(std::vector<Object*> objects, Object* gBufferDebugQuad, LightSources* lightSources);
		void DrawGBuffer(Object* gBufferDebugQuad);
		float CalculateSphereRadius(Object* pointLight);
		/* probes for ambient light render (GI) */
		void RenderSceneWithGBufferAndProbes(std::vector<Object*> staticObjects, std::vector<Object*> dynamicObjects, float timeInSecond, std::vector<Object*> backGroundObjs, Object* gBufferDebugQuad, LightSources* lightSources,
			std::vector<LightProbe*> probes, std::shared_ptr<Texture> specularBrdfLUTTexture, Object* skyBox);
		std::vector<LightProbe*> FindKnearProbes(glm::vec3 objPos, std::vector<LightProbe*> probes);

	private:
		bool m_IsInitialized = false;

		Object* m_FinalScreenQuad = nullptr;
		/* ������Ⱦ����ȫ�ֹ��� AmbientGI pass */
		Object* m_GIQuad= nullptr;
		/* pointLight ��Χ��,ֻ��Ⱦ���е�fragment  pontlight pass */
		Object* m_SurroundSphere = nullptr;



		/**** buffer ****/
		std::shared_ptr<GBuffer> m_GBffer;
		/* ���� tone mapping �� hdr ���ڴ��� */
		std::shared_ptr<FrameBuffer> m_HDRFrameBuffer;
		std::shared_ptr<Texture> m_HDRPostProcessTexture;


		/**** shader ****/
		/* write pos,normal,color to gBuffer */
		std::shared_ptr<Shader> m_GBufferShader;

		/* ����Debug--������Χ�� */
		std::shared_ptr<Shader> m_SphereDeBugShader;

		/* draw lights according to gBuffer data */
		//�ֿ����pass����Ⱦ-->ȥ��if��֧��GPU��if,for���Ѻ�
		std::shared_ptr<Shader> m_AmbientGIPassShader;
		std::shared_ptr<Shader> m_PointLightPassShader;
		std::shared_ptr<Shader> m_DirectionLightPassShader;

		std::shared_ptr<Shader> m_FinalScreenShader;


		unsigned int m_TextureWidth = Configuration::WindowWidth;
		unsigned int m_TexxtureHeight = Configuration::WindowHeight;

		/* 0.0f <m_AttenuationItensity< 256.0f ,m_AttenuationItensityԽ��˥��Խ�죬��Χ��m_SurroundSphere �뾶ԽС */
		float m_AttenuationItensity = 100.0f;

		/* m_K near probes for specular objs rendering */
		/*ע�� ����blending ���(m_K��) probes��diffuse SH,��ֻȡ�����һ��Probe��specular Map*/
		unsigned int m_K = 2;

		/*renderer for dynamic objects*/
		AnimatedModelRenderer* m_AnimatedModelRenderer;

	};

}
