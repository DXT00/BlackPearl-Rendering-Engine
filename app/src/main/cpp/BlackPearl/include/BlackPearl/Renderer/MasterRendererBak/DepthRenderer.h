#pragma once
#include "BlackPearl/MainCamera/MainCamera.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"


namespace BlackPearl {
	class DepthRenderer : public BasicRenderer
	{
	public:
		DepthRenderer();
		~DepthRenderer();
		void Init();
		void RenderDepthMap(Object* Quad, const std::vector<Object*> backgroundObjs);

		TextureHandle GetDepthTexture() const { return m_DepthTexture; }

	private:

		std::shared_ptr<Shader> m_DepthShader;
		TextureHandle m_DepthTexture;

		std::shared_ptr<Shader> m_HizShader;
		std::shared_ptr<FrameBuffer> m_HizFrameBuffer;
		uint32_t m_MipmapLevel;


	};

}

