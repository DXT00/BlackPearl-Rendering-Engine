#include "pch.h"
#include "LightProbes.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/RHI/RHITexture.h"
namespace BlackPearl {
	extern DeviceManager* g_deviceManager;

	LightProbe::LightProbe(Object* cubeObj,Type type)
	{
		GE_ASSERT(cubeObj, "cubeObj is nullptr");
		m_Type = type;
		m_LightProbeObj = cubeObj;
		/*与相机视角对应*/
		m_LightProbeObj->GetComponent<Transform>()->SetRotation({ 0.0f, -90.0f, 0.0f});
		m_LightProbeObj->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);
		m_LightProbeObj->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(false);

		SetScale({ 0.3,0.3,0.3 });
		if (type == Type::REFLECTION) {
			TextureDesc desc;
			desc.type = TextureType::CubeMap;
			desc.width = m_SpecularCubeMapResolution;
			desc.height = m_SpecularCubeMapResolution;
			desc.minFilter = FilterMode::Linear_Mip_Linear;
			desc.magFilter = FilterMode::Linear;
			desc.wrap = SamplerAddressMode::ClampToEdge;
			desc.format = Format::RGB16_FLOAT;
			desc.generateMipmap = true;

			m_SpecularPrefilterCubeMap = g_deviceManager->GetDevice()->createTexture(desc);
			//			m_SpecularPrefilterCubeMap.reset(DBG_NEW CubeMapTexture(TextureType::CubeMap, m_SpecularCubeMapResolution, m_SpecularCubeMapResolution, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));

		}
		if (type == Type::DIFFUSE)
			m_SHCoeffs.assign(9, std::vector<float>(3, 0.0f));
	}

	void LightProbe::SetPosition(glm::vec3 pos)
	{
		m_LightProbeObj->GetComponent<Transform>()->SetPosition(pos);
	}

	void LightProbe::SetScale(glm::vec3 size)
	{
		m_Size = size;
		m_LightProbeObj->GetComponent<Transform>()->SetScale(size);
	}


}
