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
		enum Type {
			DIFFUSE,
			REFLECTION
		};
		LightProbe(Object* cubeObj,Type type);
		virtual ~LightProbe() {

		}
		
		/* probe's view matrix */
		glm::mat4	GetViewMatrix()const { return m_ViewMatrix; }

		/* Textures */
		//std::shared_ptr<CubeMapTexture> GetHdrEnvironmentCubeMap()const    { return m_HdrEnvironmentCubeMap; }
		std::shared_ptr<CubeMapTexture> GetSpecularPrefilterCubeMap()const { GE_ASSERT(m_Type == Type::REFLECTION, "is not a reflection probe") return m_SpecularPrefilterCubeMap; }
		//std::shared_ptr<CubeMapTexture> GetDiffuseIrradianceCubeMap()const { return m_DiffuseIrradianceCubeMap; }
		std::shared_ptr<ITexture> GetSpecularBrdfLutMap()const       { return m_SpecularBrdfLutMap; }
		
		/* resolution */
		//unsigned int GetDiffuseCubeMapResolution() const { return m_DiffuseCubeMapResolution; }
		unsigned int GetSpecularCubeMapResolution() const { GE_ASSERT(m_Type==Type::REFLECTION,"is not a reflection probe") return m_SpecularCubeMapResolution; }
		unsigned int GetEnvironmentCubeMapResolution() const {  return m_EnvironmentCubeMapResolution; }

		/* get */
		unsigned int GetMaxMipMapLevel() const{ return m_MaxMipmapLevel; }
		glm::vec3	 GetPosition()const { return m_LightProbeObj->GetComponent<Transform>()->GetPosition(); }
		glm::vec3	 GetRotation()const { return m_LightProbeObj->GetComponent<Transform>()->GetRotation(); }

		/*set*/
		void SetPosition(glm::vec3 pos);
		void SetScale(glm::vec3 size);

		/*SH Coefficients set*/
		void SetSHCoeffs(std::vector<std::vector<float>>& SHCoeffs) { GE_ASSERT(m_Type == Type::DIFFUSE, "is not a diffuse probe"); m_SHCoeffs = SHCoeffs; }
		std::vector<std::vector<float>> GetCoeffis()const { GE_ASSERT(m_Type == Type::DIFFUSE, "is not a diffuse probe"); return m_SHCoeffs; }

		Type GetType()const { return m_Type; }
		/*cubeObj*/
		Object* GetObj()const { return m_LightProbeObj; }

		void SetAreaId(unsigned int areaId) { m_AreaId = areaId; }
		unsigned int GetAreaId()const { return m_AreaId; }
	private:

		/* probe's view matrix */
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		/* Textures */
		std::shared_ptr<CubeMapTexture> m_HdrEnvironmentCubeMap = nullptr;
		std::shared_ptr<CubeMapTexture> m_SpecularPrefilterCubeMap = nullptr;
		std::shared_ptr<ITexture>		m_SpecularBrdfLutMap = nullptr;
		unsigned int m_MaxMipmapLevel = 5;

		unsigned int					m_SampleCounts = 1024;
		unsigned int					m_EnvironmentCubeMapResolution = Configuration::EnvironmantMapResolution;// 512;// 128;
		unsigned int					m_SpecularCubeMapResolution = Configuration::EnvironmantMapResolution;// 512;// 128;

		glm::vec3 m_Size;
		Object* m_LightProbeObj;

		std::vector<std::vector<float>> m_SHCoeffs;
		Type m_Type;
		
		//记录这个probe在哪个区域,只有diffuse probe划分区域
		unsigned int m_AreaId;
	};


}


