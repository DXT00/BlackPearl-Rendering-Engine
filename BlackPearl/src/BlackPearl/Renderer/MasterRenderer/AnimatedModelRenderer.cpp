#include "pch.h"
#include "AnimatedModelRenderer.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Config.h"
#include "glm/glm.hpp"
#include "BlackPearl/Renderer/Model/Model.h"

namespace BlackPearl {
	void AnimatedModelRenderer::Render(Object* animatedModel, float timeInSecond)
	{

		std::shared_ptr<Model> animatedMmodel = animatedModel->GetComponent<MeshRenderer>()->GetModel();
		std::vector<glm::mat4> boneFinalTransforms = animatedMmodel->CalculateBoneTransform(timeInSecond);
		m_AnimatedShader->Bind();
		GE_ASSERT(boneFinalTransforms.size() <= Configuration::MaxJointsCount, "Joints conut larger than MaxJointCounts!");
		for (int i = 0; i < boneFinalTransforms.size(); i++)
		{
			m_AnimatedShader->SetUniformMat4f("u_JointModel[" + std::to_string(i) + "]", boneFinalTransforms[i]);
		}

		animatedModel->GetComponent<MeshRenderer>()->SetShaders(m_AnimatedShader);
		DrawObject(animatedModel, m_AnimatedShader);

	}

	void AnimatedModelRenderer::Render(Object* animatedModel, float timeInSecond, std::shared_ptr<Shader> shader)
	{
		std::shared_ptr<Model> animatedMmodel = animatedModel->GetComponent<MeshRenderer>()->GetModel();
		std::vector<glm::mat4> boneFinalTransforms = animatedMmodel->CalculateBoneTransform(timeInSecond);
		shader->Bind();
		GE_ASSERT(boneFinalTransforms.size() <= Configuration::MaxJointsCount, "Joints conut larger than MaxJointCounts!");
		for (int i = 0; i < boneFinalTransforms.size(); i++)
		{
			shader->SetUniformMat4f("u_JointModel[" + std::to_string(i) + "]", boneFinalTransforms[i]);
		}

		animatedModel->GetComponent<MeshRenderer>()->SetShaders(m_AnimatedShader);
		DrawObject(animatedModel, shader);
	}

}
