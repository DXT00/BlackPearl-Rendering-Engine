#include "pch.h"
#include "LightProbes.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
namespace BlackPearl {

	LightProbe::LightProbe(Object* cubeObj)
	{
		GE_ASSERT(cubeObj, "cubeObj is nullptr");
		m_CubeObj = cubeObj;
		m_Center = cubeObj->GetComponent<Transform>()->GetPosition();
		m_HdrEnvironmentCubeMap.reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, m_EnvironmentCubeMapResolution, m_EnvironmentCubeMapResolution, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_UNSIGNED_BYTE));
		m_DiffuseIrradianceCubeMap.reset(DBG_NEW CubeMapTexture(Texture::CubeMap, m_DiffuseCubeMapResolution, m_DiffuseCubeMapResolution, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
		m_SpecularPrefilterCubeMap.reset(DBG_NEW CubeMapTexture(Texture::CubeMap, m_SpecularCubeMapResolution, m_SpecularCubeMapResolution, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
	}

	void LightProbe::SetPosition(glm::vec3 pos)
	{
		m_Center = pos;
		m_CubeObj->GetComponent<Transform>()->SetPosition(pos);
	}

}
