#pragma once
#include "glm/glm.hpp"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/MainCamera/MainCamera.h"
#include "BlackPearl/Renderer/Material/TextureImage2D.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include <thread>

namespace BlackPearl {
	class IBLProbesRenderer;

	/*Logical LightProbe*/
	/*每个probe 都由一个 CubeObj 代表*/
	class LightProbe
	{
	public:
		LightProbe(Object* cubeObj,Object* camera);
		virtual ~LightProbe() {

			delete m_LightProbeObj;
			m_LightProbeObj = nullptr;
		}
		
		/* probe's view matrix */
		glm::mat4	GetViewMatrix()const { return m_ViewMatrix; }

		/* Textures */
		std::shared_ptr<CubeMapTexture> GetHdrEnvironmentCubeMap()const    { return m_HdrEnvironmentCubeMap; }
		std::shared_ptr<CubeMapTexture> GetSpecularPrefilterCubeMap()const { return m_SpecularPrefilterCubeMap; }
		std::shared_ptr<CubeMapTexture> GetDiffuseIrradianceCubeMap()const { return m_DiffuseIrradianceCubeMap; }
		std::shared_ptr<Texture> GetSpecularBrdfLutMap()const       { return m_SpecularBrdfLutMap; }
		std::shared_ptr<TextureImage2D> GetSHImage() const { return m_SHImage; }
		
		/* resolution */
		unsigned int GetDiffuseCubeMapResolution() const { return m_DiffuseCubeMapResolution; }
		unsigned int GetSpecularCubeMapResolution() const { return m_SpecularCubeMapResolution; }

		/* get */
		unsigned int GetMaxMipMapLevel() const{ return m_MaxMipmapLevel; }
		glm::vec3	 GetPosition()const { return m_LightProbeObj->GetComponent<Transform>()->GetPosition(); }
		glm::vec3	 GetRotation()const { return m_LightProbeObj->GetComponent<Transform>()->GetRotation(); }

		/*set*/
		void SetPosition(glm::vec3 pos);
		void SetScale(glm::vec3 size);

		/*SH Coefficients set*/
		void SetSHCoeffs(std::vector<std::vector<float>>& SHCoeffs) { m_SHCoeffs = SHCoeffs; }
		std::vector<std::vector<float>> GetCoeffis()const {return m_SHCoeffs;}


		/*cubeObj*/
		Object* GetObj()const { return m_LightProbeObj; }

		/*每次Probe使用前都要Update Camera!!*/
		void UpdateCamera() {
			glm::vec3 objPos = m_LightProbeObj->GetComponent<Transform>()->GetPosition();
			glm::vec3 objRot = m_LightProbeObj->GetComponent<Transform>()->GetRotation();
			m_Camera->SetPosition(objPos);
			m_Camera->SetRotation(objRot);
		}
		MainCamera* GetCamera()const { return m_Camera; }

		/*threads*/
		//void StartThread(void(IBLProbesRenderer::*UpdateProbeMaps)(const LightSources& , const std::vector<Object*> , Object* , LightProbe* ),
			//const LightSources& lightSources, const std::vector<Object*> objects, Object* skyBox);
		//void JoinThread();
	private:

		/* probe's view matrix */
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		/* Textures */
		std::shared_ptr<CubeMapTexture> m_HdrEnvironmentCubeMap = nullptr;
		std::shared_ptr<CubeMapTexture> m_SpecularPrefilterCubeMap = nullptr;
		std::shared_ptr<CubeMapTexture> m_DiffuseIrradianceCubeMap = nullptr;
		std::shared_ptr<Texture>		m_SpecularBrdfLutMap = nullptr;
		std::shared_ptr<TextureImage2D>		m_SHImage = nullptr;
		unsigned int m_MaxMipmapLevel = 5;

		unsigned int					m_SampleCounts = 1024;
		unsigned int					m_EnvironmentCubeMapResolution = 128;
		unsigned int					m_DiffuseCubeMapResolution = 32;
		unsigned int					m_SpecularCubeMapResolution = 128;

		//glm::vec3 m_Center;
		glm::vec3 m_Size;
		Object* m_LightProbeObj;
		MainCamera* m_Camera;

		std::vector<std::vector<float>> m_SHCoeffs;

		/*thread*/
		//std::thread m_Thread;

	};


}


