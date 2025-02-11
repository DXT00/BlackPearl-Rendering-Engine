#pragma once
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Buffer/Buffer.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Renderer/MasterRenderer/AnimatedModelRenderer.h"
namespace BlackPearl {
	//need a CubeMap Attachment
	class ShadowMapPointLightRenderer:public BasicRenderer
	{
	public:
		enum ShadowType {
			PCF,
			PCSS,
			VSSM //Variance soft shadow mapping
		};
		ShadowMapPointLightRenderer();
		~ShadowMapPointLightRenderer();
		void RenderCubeMap(const std::vector<Object*>& staticObjs , const std::vector<Object*>& dynamicObjs, float timeInSecond, LightSources* lightSources );
		void Render(const std::vector<Object*> backgroundObjs , LightSources* lightSources);
		//virtual void PrepareShaderParameters(Mesh mesh, glm::mat4 transformMatrix, std::shared_ptr<Shader> shader,bool isLight = false)override;
		std::shared_ptr<FrameBuffer> GetFrameBuffer() const { return m_FrameBuffer; }
		bool JudgeUpdate(const LightSources* lightSources, std::vector<Object*> staticObjs,std::vector<Object*> dynamicObjs);
		
		static const float s_NearPlane;
		static float s_FarPlane;
		static float s_FOV;
		static int s_PCFSamplesCnt;

		static bool  s_UpdateShadowMap;
	private:

		AnimatedModelRenderer* m_AnimatedModelRenderer;

		std::shared_ptr<Shader> m_SimpleDepthShader;
		std::shared_ptr<Shader> m_SimpleDepthAnimatedObjShader;

		std::shared_ptr<Shader> m_ShadowMapShader;

		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		//std::shared_ptr<CubeMapTexture> m_DepthCubeMap;


		static int s_ShadowMapPointLightWidth;
		static int s_ShadowMapPointLightHeight;

		/*对于每个 PointLight 在 m_ShadowRaduis 球体范围内的物体才会画出shadow */
		float m_ShadowRaduis = Configuration::ShadowMapPointLightRadius;
		std::vector<glm::mat4> m_LightProjectionViewMatries;
		//glm::vec3 m_LightPos;

		ShadowType m_ShadowType;
	};
}


