#pragma once
#include "BlackPearl/Component/Component.h"
#include "glm/glm.hpp"
#include "BlackPearl/Core.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/Math/Math.h"
using namespace BlackPearl::math;

#include "hlsl/core/light_cb.h"
namespace BlackPearl {
	extern DeviceManager* g_deviceManager;

	enum ProbeType {
		DIFFUSE_PROBE,
		REFLECTION_PROBE
	};

    enum ProbeStorageType {
        PT_CubeMap,
        PT_SH,
        PT_Texture2D //ddgi
    };

	class LightProbe :public Component<LightProbe>
	{
	public:
		
        LightProbe(ProbeType type, ProbeStorageType storageType = ProbeStorageType::PT_SH);
		/* probe's view matrix */
		glm::mat4	GetViewMatrix()const { return m_ViewMatrix; }

		/* Textures */
        TextureHandle GetHdrEnvironmentCubeMap()const { return m_HdrEnvironmentCubeMap; }
        TextureHandle GetLdrEnvironmentCubeMap()const { return m_LdrEnvironmentCubeMap; }
        TextureHandle GetDepthBuffer() const { return m_DepthBuffer; }
        void ReleaseHdrEnvironmentCubeMap();
        void ReleaseLdrEnvironmentCubeMap();


		TextureHandle GetSpecularPrefilterCubeMap()const { GE_ASSERT(m_Type == ProbeType::REFLECTION_PROBE, "is not a reflection probe"); return m_SpecularPrefilterCubeMap; }
		TextureHandle GetSpecularBrdfLutMap()const { return m_SpecularBrdfLutMap; }

		/* resolution */
		unsigned int GetSpecularCubeMapResolution() const { GE_ASSERT(m_Type == ProbeType::REFLECTION_PROBE, "is not a reflection probe"); return m_SpecularCubeMapResolution; }
		unsigned int GetEnvironmentCubeMapResolution() const { return m_EnvironmentCubeMapResolution; }

		/* get */
		unsigned int GetMaxMipMapLevel() const { return m_MaxMipmapLevel; }
	

		/*SH Coefficients set*/
		void SetSHCoeffs(std::vector<std::vector<float>>& SHCoeffs) { GE_ASSERT(m_Type == ProbeType::DIFFUSE_PROBE, "is not a diffuse probe"); m_SHCoeffs = SHCoeffs; }
		std::vector<std::vector<float>> GetCoeffis()const { GE_ASSERT(m_Type == ProbeType::DIFFUSE_PROBE, "is not a diffuse probe"); return m_SHCoeffs; }

		ProbeType GetType() const { return m_Type; }
        ProbeStorageType GetStorageType() const { return m_StorageType; }

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
	
		void FillLightProbeConstants(LightProbeConstants& lightProbeConstants) const;

	public:
		bool enabled = true;
	
	private:

		/* probe's view matrix */
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		/* Textures */
		TextureHandle m_HdrEnvironmentCubeMap = nullptr; // get from hdr image
        TextureHandle m_LdrEnvironmentCubeMap = nullptr; // rendered by probe camera
        TextureHandle m_DepthBuffer = nullptr; // depth of environment cube map

		TextureHandle m_SpecularPrefilterCubeMap = nullptr;
		TextureHandle m_SpecularBrdfLutMap = nullptr;
		unsigned int m_MaxMipmapLevel = 2;

		unsigned int					m_SampleCounts = 1024;
		unsigned int					m_EnvironmentCubeMapResolution = Configuration::EnvironmantMapResolution;// 512;// 128;
		unsigned int					m_SpecularCubeMapResolution = Configuration::EnvironmantMapResolution;// 512;// 128;
		float       					m_Zfar;//perspective cube range from 0 to zFar

		std::vector<std::vector<float>> m_SHCoeffs;
		ProbeType m_Type;
        ProbeStorageType m_StorageType = ProbeStorageType::PT_SH;



		//记录这个probe在哪个区域,只有diffuse probe划分区域
		unsigned int m_AreaId;

		// m_ExcludeObjsId 中的objects不会被渲染到environmentCubeMap上！
		std::vector< uint64_t> m_ExcludeObjsId;

		bool m_UpdateSpecularMapEveryFrame = false;
	};
}


