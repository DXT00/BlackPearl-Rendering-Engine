#include "pch.h"
#include "LightProbes.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
namespace BlackPearl {

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
		if(type == Type::REFLECTION)
			m_SpecularPrefilterCubeMap.reset(DBG_NEW CubeMapTexture(TextureType::CubeMap, m_SpecularCubeMapResolution, m_SpecularCubeMapResolution, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
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
