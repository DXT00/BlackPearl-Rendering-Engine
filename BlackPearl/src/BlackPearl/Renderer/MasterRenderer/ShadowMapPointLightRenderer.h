#pragma once
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
namespace BlackPearl {
	//need a CubeMap Attachment
	class ShadowMapPointLightRenderer:public BasicRenderer
	{
	public:
		ShadowMapPointLightRenderer() {
			m_FrameBuffer.reset(DBG_NEW FrameBuffer(s_ShadowMapPointLightWidth, s_ShadowMapPointLightHeight,
				{ FrameBuffer::Attachment::CubeMapDepthTexture },0, true));
			m_LightProjectionViewMatries.assign(6, glm::mat4(1.0));
		};
		~ShadowMapPointLightRenderer();
		void Render(const std::vector<Object*>&objs, Object* pointLight, const std::vector<Object*>&exceptObjs);
		virtual void PrepareShaderParameters(Mesh &mesh, glm::mat4 transformMatrix, std::shared_ptr<Shader> shader,bool isLight = false)override;
		std::shared_ptr<FrameBuffer> GetFrameBuffer() const { return m_FrameBuffer; }

		
		static const float s_NearPlane;
		static float s_FarPlane;
		static float s_FOV;

	private:
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		static int s_ShadowMapPointLightWidth;
		static int s_ShadowMapPointLightHeight;
		std::vector<glm::mat4> m_LightProjectionViewMatries;
		glm::vec3 m_LightPos;
	};
}


