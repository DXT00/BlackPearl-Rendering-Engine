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
		void Init(Object* ScreenQuad);
		void Render(std::vector<Object*> objects, Object* gBufferDebugQuad);
		void DrawGBuffer(Object* gBufferDebugQuad);
	private:
		bool m_IsInitialized = false;

		Object* m_ScreenQuad= nullptr;

		std::shared_ptr<GBuffer> m_GBffer;
		/* write pos,normal,color to gBuffer */
		std::shared_ptr<Shader> m_GBufferShader;
		/* draw lights according to gBuffer data */
		std::shared_ptr<Shader> m_LightingShader;

		unsigned int m_TextureWidth = Configuration::WindowWidth;
		unsigned int m_TexxtureHeight = Configuration::WindowHeight;
	};

}

