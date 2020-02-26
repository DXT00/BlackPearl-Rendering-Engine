#include "pch.h"
#include "LightProbes.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"

namespace BlackPearl {

	LightProbe::LightProbe(Object* cubeObj, Object* camera)
	{
		GE_ASSERT(cubeObj, "cubeObj is nullptr");
		m_LightProbeObj = cubeObj;
		m_SHCoeffs.assign(9, std::vector<float>(3,0.0f));
		/*与相机视角对应*/
		m_LightProbeObj->GetComponent<Transform>()->SetRotation({ 0.0f, -90.0f, 0.0f});

		m_Camera = DBG_NEW MainCamera(camera);
		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetFov(90.0f);
		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetWidth(512);
		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetHeight(512);
		m_Camera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetZfar(50.0f);

		SetScale({ 0.3,0.3,0.3 });


		m_HdrEnvironmentCubeMap.reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, m_EnvironmentCubeMapResolution, m_EnvironmentCubeMapResolution, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
		m_DiffuseIrradianceCubeMap.reset(DBG_NEW CubeMapTexture(Texture::CubeMap, m_DiffuseCubeMapResolution, m_DiffuseCubeMapResolution, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
		m_SpecularPrefilterCubeMap.reset(DBG_NEW CubeMapTexture(Texture::CubeMap, m_SpecularCubeMapResolution, m_SpecularCubeMapResolution, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
		m_SHImage.reset(DBG_NEW TextureImage2D(9,1, GL_LINEAR, GL_LINEAR,GL_RGBA32F,GL_RGBA,GL_CLAMP_TO_EDGE,GL_UNSIGNED_BYTE,GL_READ_WRITE));
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

	//void LightProbe::StartThread(void(IBLProbesRenderer::*UpdateProbeMaps)(const LightSources&, const std::vector<Object*>, Object*, LightProbe*),
	//	const LightSources& lightSources, const std::vector<Object*> objects, Object* skyBox)
	//{
	//	m_Thread = std::thread(UpdateProbeMaps,this, lightSources, objects,  skyBox, this);
	//}

	//void LightProbe::JoinThread()
	//{
	//	m_Thread.join();
	//}

}
