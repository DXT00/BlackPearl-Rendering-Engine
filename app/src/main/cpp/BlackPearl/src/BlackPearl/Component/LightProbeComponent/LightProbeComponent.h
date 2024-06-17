#pragma once
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
#include "glm/glm.hpp"
#include "BlackPearl/Core.h"
namespace BlackPearl {
	enum ProbeType {
		DIFFUSE_PROBE,
		REFLECTION_PROBE
	};
	class LightProbe :public Component<LightProbe>
	{
	public:
		
		LightProbe(ProbeType type)
			:Component(Component::Type::LightProbe){
		
			m_Type = type;
			m_Zfar = 13.0f;
			if (type == ProbeType::REFLECTION_PROBE)
				m_SpecularPrefilterCubeMap.reset(DBG_NEW CubeMapTexture(Texture::CubeMap, m_SpecularCubeMapResolution, m_SpecularCubeMapResolution, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
			if (type == ProbeType::DIFFUSE_PROBE)
				m_SHCoeffs.assign(9, std::vector<float>(3, 0.0f));
		
		
		}
		/* probe's view matrix */
		glm::mat4	GetViewMatrix()const { return m_ViewMatrix; }

		/* Textures */
		//std::shared_ptr<CubeMapTexture> GetHdrEnvironmentCubeMap()const    { return m_HdrEnvironmentCubeMap; }
		std::shared_ptr<CubeMapTexture> GetSpecularPrefilterCubeMap()const { GE_ASSERT(m_Type == ProbeType::REFLECTION_PROBE, "is not a reflection probe"); return m_SpecularPrefilterCubeMap; }
		//std::shared_ptr<CubeMapTexture> GetDiffuseIrradianceCubeMap()const { return m_DiffuseIrradianceCubeMap; }
		std::shared_ptr<Texture> GetSpecularBrdfLutMap()const { return m_SpecularBrdfLutMap; }

		/* resolution */
		unsigned int GetSpecularCubeMapResolution() const { GE_ASSERT(m_Type == ProbeType::REFLECTION_PROBE, "is not a reflection probe"); return m_SpecularCubeMapResolution; }
		unsigned int GetEnvironmentCubeMapResolution() const { return m_EnvironmentCubeMapResolution; }

		/* get */
		unsigned int GetMaxMipMapLevel() const { return m_MaxMipmapLevel; }
	

		/*SH Coefficients set*/
		void SetSHCoeffs(std::vector<std::vector<float>>& SHCoeffs) { GE_ASSERT(m_Type == ProbeType::DIFFUSE_PROBE, "is not a diffuse probe"); m_SHCoeffs = SHCoeffs; }
		std::vector<std::vector<float>> GetCoeffis()const { GE_ASSERT(m_Type == ProbeType::DIFFUSE_PROBE, "is not a diffuse probe"); return m_SHCoeffs; }

		ProbeType GetType()const { return m_Type; }
		/*cubeObj*/
	//	Object* GetObj()const { return m_LightProbeObj; }

		void SetAreaId(unsigned int areaId) { m_AreaId = areaId; }
		unsigned int GetAreaId()const { return m_AreaId; }

		float GetZfar()const { return m_Zfar; }
		void SetZfar(float zfar) { m_Zfar = zfar; }
		void AddExcludeObjectId(uint64_t id) {
			m_ExcludeObjsId.push_back(id);
		}
		std::vector<uint64_t> GetExcludeObjectsId()const { return m_ExcludeObjsId; }
		bool GetDynamicSpecularMap() { return m_UpdateSpecularMapEveryFrame; }
		void SetDynamicSpecularMap(bool dynamic) { m_UpdateSpecularMapEveryFrame = dynamic; }
	private:

		/* probe's view matrix */
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		/* Textures */
		std::shared_ptr<CubeMapTexture> m_HdrEnvironmentCubeMap = nullptr;
		std::shared_ptr<CubeMapTexture> m_SpecularPrefilterCubeMap = nullptr;
		std::shared_ptr<Texture>		m_SpecularBrdfLutMap = nullptr;
		unsigned int m_MaxMipmapLevel = 5;

		unsigned int					m_SampleCounts = 1024;
		unsigned int					m_EnvironmentCubeMapResolution = Configuration::EnvironmantMapResolution;// 512;// 128;
		unsigned int					m_SpecularCubeMapResolution = Configuration::EnvironmantMapResolution;// 512;// 128;
		float       					m_Zfar;//perspective cube range from 0 to zFar
		//glm::vec3 m_Size;
		//Object* m_LightProbeObj;

		std::vector<std::vector<float>> m_SHCoeffs;
		ProbeType m_Type;

		//��¼���probe���ĸ�����,ֻ��diffuse probe��������
		unsigned int m_AreaId;

		// m_ExcludeObjsId �е�objects���ᱻ��Ⱦ��environmentCubeMap�ϣ�
		std::vector< uint64_t> m_ExcludeObjsId;

		bool m_UpdateSpecularMapEveryFrame = false;
	};
}


