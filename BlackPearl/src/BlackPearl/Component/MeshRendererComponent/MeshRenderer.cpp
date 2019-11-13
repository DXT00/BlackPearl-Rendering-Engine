#include "pch.h"
#include "MeshRenderer.h"

namespace BlackPearl {

	void MeshRenderer::UpdateTransformMatrix(glm::mat4 transformMatrix) {
		m_TransformMatrix = transformMatrix;
	}
	//注意：Draw之前要UpdateTransformMatrix！
	void MeshRenderer::DrawMeshes() {

		for (Mesh mesh : m_Meshes)
			mesh.Draw(m_TransformMatrix, Renderer::GetSceneData()->LightSources);
	}
	void MeshRenderer::DrawModel() {
		//m_Model->GetShader()->Bind();
		if (m_Model == nullptr)return;
		std::vector<Mesh> meshes = m_Model->GetMeshes();

		for (Mesh mesh : meshes)
		{
			mesh.Draw(m_TransformMatrix, Renderer::GetSceneData()->LightSources);

		}
	}
	void MeshRenderer::DrawLight() {
		for (Mesh mesh : m_Meshes)
			mesh.Draw(m_TransformMatrix);
	}

	void MeshRenderer::SetTexture(unsigned int meshIndex, std::shared_ptr<Texture> texture) {
		if (meshIndex >= m_Meshes.size() - 1) {
			m_Meshes[meshIndex].SetTexture(texture);
		}
		else {
			GE_CORE_ERROR("meshIndex out of m_Meshes range!")
		}
	}
}
