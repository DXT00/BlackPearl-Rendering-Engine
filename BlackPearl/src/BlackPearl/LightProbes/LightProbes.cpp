#include "pch.h"
#include "LightProbes.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
namespace BlackPearl {

	LightProbe::LightProbe(Object* cubeObj, Object* camera)
	{
		GE_ASSERT(cubeObj, "cubeObj is nullptr");
		m_LightProbeObj = cubeObj;
		/*与相机视角对应*/
		m_LightProbeObj->GetComponent<Transform>()->SetRotation({ 0.0f, -90.0f, 0.0f});

		m_Camera = DBG_NEW MainCamera(camera);
		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetFov(90.0f);
		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetWidth(512);
		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetHeight(512);


		m_HdrEnvironmentCubeMap.reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, m_EnvironmentCubeMapResolution, m_EnvironmentCubeMapResolution, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_UNSIGNED_BYTE));
		m_DiffuseIrradianceCubeMap.reset(DBG_NEW CubeMapTexture(Texture::CubeMap, m_DiffuseCubeMapResolution, m_DiffuseCubeMapResolution, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
		m_SpecularPrefilterCubeMap.reset(DBG_NEW CubeMapTexture(Texture::CubeMap, m_SpecularCubeMapResolution, m_SpecularCubeMapResolution, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
	}

	void LightProbe::SetPosition(glm::vec3 pos)
	{
		//m_Center = pos;
		m_LightProbeObj->GetComponent<Transform>()->SetPosition(pos);
	}

	void LightProbe::SetScale(glm::vec3 size)
	{
		m_Size = size;
		m_LightProbeObj->GetComponent<Transform>()->SetScale(size);
	}

}
