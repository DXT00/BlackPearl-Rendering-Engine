#pragma once
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Map/MapManager.h"
#include "RHI/RHITexture.h"
namespace BlackPearl {

	class GBufferRenderer: public BasicRenderer
	{

	public:
		GBufferRenderer(IDevice* device);

		void Init();
        void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);


        void Render(std::vector<Object*> objects, Object* gBufferDebugQuad, LightSources* lightSources);
		void RenderSceneWithGBufferAndProbes(
			std::vector<Object*> staticObjects, 
			std::vector<Object*> dynamicObjects,
			float timeInSecond, 
			std::vector<Object*> backGroundObjs, 
			Object* gBufferDebugQuad,
			LightSources* lightSources,
			std::vector<Object*>diffuseProbes,
			std::vector<Object*>reflectionProbes,
			TextureHandle specularBrdfLUTTexture,
			Object* skyBox, 
			MapManager* mapManager,
			TextureHandle depthTexture,
			bool enableSSR);


	private:
		bool m_IsInitialized = false;
		bool m_SortProbes = false;
		bool m_ProsessGridChanged = false;

		/**** buffer ****/
		std::shared_ptr<GBuffer> m_GBuffer;


		/**** shader ****/
		/* write pos,normal,color to gBuffer */
		MaterialShader* m_GBufferShader;

		
	
		unsigned int m_K = 3;// 5;// 5;// 5;// 2;
		//

		///*renderer for dynamic objects*/
		//AnimatedModelRenderer* m_AnimatedModelRenderer;
		//SkyboxRenderer* m_SkyboxRenderer;


	};

}

