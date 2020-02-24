#pragma once
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
namespace BlackPearl {
	//need a CubeMap Attachment
	class ShadowMapPointLightRenderer:public BasicRenderer
	{
	public:
		ShadowMapPointLightRenderer();
		~ShadowMapPointLightRenderer();
		void RenderCubeMap(const std::vector<Object*>&objs ,LightSources* lightSources );
		void Render(const std::vector<Object*> backgroundObjs , LightSources* lightSources);
		//virtual void PrepareShaderParameters(Mesh mesh, glm::mat4 transformMatrix, std::shared_ptr<Shader> shader,bool isLight = false)override;
		std::shared_ptr<FrameBuffer> GetFrameBuffer() const { return m_FrameBuffer; }

		
		static const float s_NearPlane;
		static float s_FarPlane;
		static float s_FOV;

	private:

		std::shared_ptr<Shader> m_SimpleDepthShader;
		std::shared_ptr<Shader> m_ShadowMapShader;

		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		//std::shared_ptr<CubeMapTexture> m_DepthCubeMap;


		static int s_ShadowMapPointLightWidth;
		static int s_ShadowMapPointLightHeight;
		std::vector<glm::mat4> m_LightProjectionViewMatries;
		//glm::vec3 m_LightPos;
	};
}


