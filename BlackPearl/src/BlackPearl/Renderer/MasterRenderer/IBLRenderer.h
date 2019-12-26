#pragma once
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Renderer/Material/HDRTexture.h"
#include"BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
namespace BlackPearl {
	class IBLRenderer:public BasicRenderer
	{
	public:
		IBLRenderer();
		void Init(Object* cubeObj);
		~IBLRenderer();
		void Render(std::vector<Object*> objs);
		std::shared_ptr<FrameBuffer> GetFrameBuffer() { return m_FrameBuffer; }

	private:
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
	
		static int s_Width;
		static int s_Height;
		
		//Hdr Texture
		std::shared_ptr<HDRTexture> m_HdrTexture;

		//Shader
		std::shared_ptr<Shader> m_HdrMapToCubeShader;
		std::shared_ptr<Shader> m_PbrShader;
		std::shared_ptr<Shader> m_BlackGroundShader;

		//CubeMap's cube
		Object* m_CubeObj=nullptr;

		bool m_IsInitialize = false;

	};

}