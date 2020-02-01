#pragma once
#include "glm/glm.hpp"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Object/Object.h"
namespace BlackPearl {

	/*每个probe 都由一个 CubeObj 代表*/
	class LightProbe
	{
	public:
		LightProbe(Object* cubeObj);
		virtual ~LightProbe() {

			delete m_CubeObj;
			m_CubeObj = nullptr;
		}
		
		/* probe's view matrix */
		glm::mat4	GetViewMatrix()const { return m_ViewMatrix; }

		/* Textures */
		std::shared_ptr<CubeMapTexture> GetHdrEnvironmentCubeMap()const    { return m_HdrEnvironmentCubeMap; }
		std::shared_ptr<CubeMapTexture> GetSpecularPrefilterCubeMap()const { return m_SpecularPrefilterCubeMap; }
		std::shared_ptr<CubeMapTexture> GetDiffuseIrradianceCubeMap()const { return m_DiffuseIrradianceCubeMap; }
		std::shared_ptr<Texture> GetSpecularBrdfLutMap()const       { return m_SpecularBrdfLutMap; }
		/* resolution */
		unsigned int GetDiffuseCubeMapResolution() const { return m_DiffuseCubeMapResolution; }
		unsigned int GetSpecularCubeMapResolution() const { return m_SpecularCubeMapResolution; }

		/* */
		unsigned int GetMaxMipMapLevel() const{ return m_MaxMipmapLevel; }
		glm::vec3	 GetPosition()const { return m_CubeObj->GetComponent<Transform>()->GetPosition(); }

		/*set*/
		void SetPosition(glm::vec3 pos);
		void SetScale(glm::vec3 size);
		/*cubeObj*/
		Object* GetCubeObj()const { return m_CubeObj; }

	private:

		/* probe's view matrix */
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		/* Textures */
		std::shared_ptr<CubeMapTexture> m_HdrEnvironmentCubeMap = nullptr;
		std::shared_ptr<CubeMapTexture> m_SpecularPrefilterCubeMap = nullptr;
		std::shared_ptr<CubeMapTexture> m_DiffuseIrradianceCubeMap = nullptr;
		std::shared_ptr<Texture>		m_SpecularBrdfLutMap = nullptr;

		unsigned int m_MaxMipmapLevel = 5;

		unsigned int					m_SampleCounts = 1024;
		unsigned int					m_EnvironmentCubeMapResolution = 512;
		unsigned int					m_DiffuseCubeMapResolution = 128;
		unsigned int					m_SpecularCubeMapResolution = 128;

		//glm::vec3 m_Center;
		glm::vec3 m_Size;
		Object* m_CubeObj;
	};


}


