#include "pch.h"
#include "LightProbeComponent.h"
namespace BlackPearl {

	//LightProbeComponent::LightProbeComponent(ProbeType type)
	//{
	//	//GE_ASSERT(cubeObj, "cubeObj is nullptr");
	//	m_Type = type;
	////	m_LightProbeObj = cubeObj;
	//	/*与相机视角对应*/
	///*	m_LightProbeObj->GetComponent<Transform>()->SetRotation({ 0.0f, -90.0f, 0.0f });
	//	m_LightProbeObj->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);
	//	m_LightProbeObj->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(false);*/

	//	/*	m_Camera = DBG_NEW MainCamera(camera);
	//		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetFov(90.0f);
	//		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetWidth(m_EnvironmentCubeMapResolution);
	//		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetHeight(m_EnvironmentCubeMapResolution);
	//		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetZfar(13.0f);*/

	//	//SetScale({ 0.3,0.3,0.3 });


	//	//m_HdrEnvironmentCubeMap.reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, m_EnvironmentCubeMapResolution, m_EnvironmentCubeMapResolution, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
	////	m_DiffuseIrradianceCubeMap.reset(DBG_NEW CubeMapTexture(Texture::CubeMap, m_DiffuseCubeMapResolution, m_DiffuseCubeMapResolution, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
	//	if (type == ProbeType::REFLECTION_PROBE)
	//		m_SpecularPrefilterCubeMap.reset(DBG_NEW CubeMapTexture(Texture::CubeMap, m_SpecularCubeMapResolution, m_SpecularCubeMapResolution, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
	//	if (type == ProbeType::DIFFUSE_PROBE)
	//		m_SHCoeffs.assign(9, std::vector<float>(3, 0.0f));

	//}

	//void LightProbeComponent::SetPosition(glm::vec3 pos)
	//{
	//	m_LightProbeObj->GetComponent<Transform>()->SetPosition(pos);
	//}

	//void LightProbeComponent::SetScale(glm::vec3 size)
	//{
	//	m_Size = size;
	//	m_LightProbeObj->GetComponent<Transform>()->SetScale(size);
	//}


	
}