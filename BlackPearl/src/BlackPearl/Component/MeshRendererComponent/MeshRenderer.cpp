#include "pch.h"
#include "MeshRenderer.h"

namespace BlackPearl {

	void MeshRenderer::UpdateTransformMatrix(glm::mat4 transformMatrix) {
		m_TransformMatrix = transformMatrix;
	}
	//注意：Draw之前要UpdateTransformMatrix！
	//void MeshRenderer::DrawMeshes() {
	//	for (auto mesh : GetMeshes())
	//		mesh.Draw(m_TransformMatrix, Renderer::GetSceneData()->LightSources);
	//}
	
	//void MeshRenderer::DrawLight() {
	//	for (Mesh mesh : m_Meshes)
	//		mesh.Draw(m_TransformMatrix);
	//}

	void MeshRenderer::SetTexture(unsigned int meshIndex, const std::shared_ptr<Texture>& texture) {
		if (meshIndex >= GetMeshes().size() - 1) {
			GetMeshes()[meshIndex].SetTexture(texture);
		}
		else {
			GE_CORE_ERROR("meshIndex out of m_Meshes range!")
		}
	}
	void MeshRenderer::SetTextures(const std::shared_ptr<Texture>& texture)
	{
		GE_ASSERT(!GetMeshes().empty(), "m_Meshes is empty,MeshRenderer::SetTextures doesn't work!");
		for (auto mesh : GetMeshes()) {
			mesh.SetTexture(texture);
		}

	}
	void MeshRenderer::SetModelTexture(unsigned int meshIndex, const std::shared_ptr<Texture>&texture)
	{
		GE_ASSERT(m_Model, "m_Model is nullptr");
		GE_ASSERT(!m_Model->GetMeshes().empty(), "Model has no textures!");
		GE_ASSERT(m_Model->GetMeshes().size() > meshIndex, "meshIndex out of m_Meshes range!");
		m_Model->GetMeshes()[meshIndex].SetTexture(texture);
	}
	void MeshRenderer::SetModelTextures(const std::shared_ptr<Texture>& texture)
	{
		GE_ASSERT(m_Model, "m_Model is nullptr");
		GE_ASSERT(!m_Model->GetMeshes().empty(), "Model has no textures!");
		for (auto mesh : m_Model->GetMeshes()) {
			mesh.SetTexture(texture);

		}
	}
	void MeshRenderer::SetShaders(const std::string & image)
	{
		GE_ASSERT(!GetMeshes().empty(), "m_Meshes is empty,MeshRenderer::SetShaders doesn't work!");
		for (int i = 0; i < GetMeshes().size(); i++){
			GetMeshes()[i].SetShader(image);
		}

	}
	void MeshRenderer::SetShaders(const std::shared_ptr<Shader>& shader)
	{
		GE_ASSERT(!GetMeshes().empty(), "m_Meshes is empty,MeshRenderer::SetShaders doesn't work!");
		for (int i = 0; i < GetMeshes().size(); i++) {
			GetMeshes()[i].SetShader(shader);
		}
	}
}
