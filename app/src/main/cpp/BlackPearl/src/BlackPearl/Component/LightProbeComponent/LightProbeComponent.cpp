#include "pch.h"
#include "Component/LightProbeComponent/LightProbeComponent.h"

namespace BlackPearl {


    LightProbe::LightProbe(ProbeType type, ProbeStorageType storageType)
        :Component(Component::Type::LightProbe) {
        m_Type = type;
        m_StorageType = storageType;
        m_Zfar = 13.0f;
        if (type == ProbeType::REFLECTION_PROBE)
        {
            TextureDesc desc;
            desc.type = TextureType::CubeMap;
            desc.width = m_SpecularCubeMapResolution;
            desc.height = m_SpecularCubeMapResolution;
            desc.minFilter = FilterMode::Linear_Mip_Linear;
            desc.magFilter = FilterMode::Linear;
            desc.wrap = SamplerAddressMode::ClampToEdge;
            desc.format = Format::RGBA8_UNORM;
            desc.generateMipmap = true;

            m_SpecularPrefilterCubeMap = g_deviceManager->GetDevice()->createTexture(desc);

        }
        if (type == ProbeType::DIFFUSE_PROBE
            && (m_StorageType == PT_SH ||
                m_StorageType == CubeMap)) {

            // if type is CubeMap ,can release cubemap after create SH by ReleaseLdrEnvironmentCubeMap
            TextureDesc desc;
            desc.type = TextureType::CubeMap;
            desc.width = m_EnvironmentCubeMapResolution;
            desc.height = m_EnvironmentCubeMapResolution;
            desc.minFilter = FilterMode::Linear_Mip_Linear;
            desc.magFilter = FilterMode::Linear;
            desc.wrap = SamplerAddressMode::ClampToEdge;
            desc.format = Format::RGBA8_UNORM;
            desc.generateMipmap = true;
            desc.mipLevelsCnt = m_MaxMipmapLevel;
            desc.dimension = TextureDimension::TextureCube;
            m_LdrEnvironmentCubeMap = g_deviceManager->GetDevice()->createTexture(desc);


           
            m_SHCoeffs.assign(9, float4(0.0f));

        }


        //for diffuse and specular probe
        TextureDesc depthDesc;
        depthDesc.width = m_EnvironmentCubeMapResolution;
        depthDesc.height = m_EnvironmentCubeMapResolution;
        depthDesc.mipLevelsCnt = m_MaxMipmapLevel;
        depthDesc.sampleCount = Configuration::MSAA_SAMPLES;
        depthDesc.format = Format::D32S8;
        depthDesc.type = TextureType::DepthMap;
        depthDesc.dimension = TextureDimension::Texture2D;

        m_DepthBuffer = g_deviceManager->GetDevice()->createTexture(depthDesc);
        static_assert(sizeof(LightProbeConstants) == 160, "Size mismatch with GLSL std140!");

    }

	
	void LightProbe::ReleaseHdrEnvironmentCubeMap()
	{
        if(m_HdrEnvironmentCubeMap.Get())
            delete m_HdrEnvironmentCubeMap.Get();

        m_HdrEnvironmentCubeMap = nullptr;
    }

	void LightProbe::ReleaseLdrEnvironmentCubeMap()
	{
        if (m_LdrEnvironmentCubeMap.Get())
            delete m_LdrEnvironmentCubeMap.Get();

        m_LdrEnvironmentCubeMap = nullptr;

	}

	void LightProbe::FillLightProbeConstants(const float3& pos, LightProbeConstants& lightProbeConstants) const
	{
		//TODO::
		//lightProbeConstants.diffuseArrayIndex = 0;// diffuseArrayIndex;
		//lightProbeConstants.specularArrayIndex = 0; //specularArrayIndex;
		//lightProbeConstants.diffuseScale = 0;// diffuseScale;
		//lightProbeConstants.specularScale = 0;// specularScale;
		//lightProbeConstants.mipLevels = 0.0f;// specularMap ? static_cast<float>(specularMap->getDesc().mipLevels) : 0.f;

		//for (uint32_t nPlane = 0; nPlane < 6; nPlane++)
		//{
		//	lightProbeConstants.frustumPlanes[nPlane] = float4(0.0);// float4(bounds.planes[nPlane].normal, bounds.planes[nPlane].distance);
		//}

        for (size_t i = 0; i < 9; i++)
        {
            lightProbeConstants.SHCoeffs[i] = m_SHCoeffs[i];
        }
        lightProbeConstants.pos = pos;
	}

}