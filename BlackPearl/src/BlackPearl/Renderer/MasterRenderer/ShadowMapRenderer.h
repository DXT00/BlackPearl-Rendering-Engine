#pragma once
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
namespace BlackPearl {
	//ShadowMap的Render操作 输入数据(Objs)--->ShadowMapRenderer--->渲染ShadowMap
	class ShadowMapRenderer:public BasicRenderer
	{
	public:
		ShadowMapRenderer() {
			
			m_FrameBuffer.reset(DBG_NEW BlackPearl::FrameBuffer(s_ShadowMapSize, s_ShadowMapSize, { FrameBuffer::Attachment::DepthTexture }, true));

		};
		~ShadowMapRenderer();
		void Render(const std::vector<Object*>&objs,  ParallelLight* sun,const std::vector<Object*>&exceptObjs);
		virtual void PrepareShaderParameters(Mesh &mesh, glm::mat4 transformMatrix, bool isLight = false)override;
		glm::mat4 GetLightProjectionViewMatrix() const { return m_LightProjectionViewMatrix; }
		std::shared_ptr<FrameBuffer> GetFrameBuffer() const{ return m_FrameBuffer; }
		static float s_NearPlane;
		static float s_FarPlane;
	private:
		
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		glm::mat4 m_LightProjectionViewMatrix;
		glm::vec3 m_LightPos;
		static int s_ShadowMapSize;
		
	};
}