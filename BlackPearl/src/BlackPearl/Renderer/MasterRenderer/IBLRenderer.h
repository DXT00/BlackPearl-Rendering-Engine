#pragma once
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Renderer/Material/HDRTexture.h"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
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
		void RenderHdrMapToEnvironmentCubeMap();
		void RenderIrradianceMap();
		void RenderSpheres(Object* sphere);
		void Render(std::vector<Object*>objs);
		std::shared_ptr<FrameBuffer> GetFrameBuffer() { return m_FrameBuffer; }
		unsigned int GetHdrCubeMapID() { return m_HdrCubeMapID; }
		unsigned int GetIrradianceCubeMap() { return m_IrradianceCubeMapID; }

	private:
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		std::shared_ptr<FrameBuffer> m_IrradianceFrameBuffer;
	
		static int s_Width;
		static int s_Height;
		
		//Hdr Texture
		std::shared_ptr<HDRTexture> m_HdrTexture;

		//Shader
		std::shared_ptr<Shader> m_HdrMapToCubeShader;//create environmentMap from hdr texture
		std::shared_ptr<Shader> m_IBLShader; //scene renderer
		std::shared_ptr<Shader> m_BlackGroundShader; //render skybox
		std::shared_ptr<Shader> m_IrradianceShader;  //create irradianceCubeMap

		//HdrCubeMap's cube
		Object* m_CubeObj=nullptr;

		bool m_IsInitialize = false;

		glm::mat4 m_CaptureProjection;
		std::vector<glm::mat4> m_CaptureProjectionViews;
		unsigned int m_HdrCubeMapID;
		unsigned int m_IrradianceCubeMapID;
	};

}