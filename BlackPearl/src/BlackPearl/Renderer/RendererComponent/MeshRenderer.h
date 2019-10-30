#pragma once
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/Model/Model.h"
#include "BlackPearl/Renderer/Renderer.h"

namespace BlackPearl {
	class MeshRenderer:public Component<MeshRenderer>
	{
	public:
		//get transformMatrix from Object's Transform Component!
		MeshRenderer(EntityManager* entityManager, Entity::Id id,const std::vector<Mesh> &meshes,glm::mat4 transformMatrix)
			:Component(entityManager, id),m_Meshes(meshes),m_TransformMatrix(transformMatrix){}
		MeshRenderer(EntityManager* entityManager, Entity::Id id, const std::shared_ptr<Model>& model, glm::mat4 transformMatrix)
			:Component(entityManager, id),m_Model(model), m_TransformMatrix(transformMatrix) {
			//m_Meshes = m_Model->GetMeshes();
		}
		~MeshRenderer() {};

		void DrawMeshes() {
			
			for (Mesh mesh : m_Meshes)
				mesh.Draw(m_TransformMatrix,Renderer::GetSceneData()->LightSources);
		}
		void DrawModel() {
			m_Model->GetShader()->Bind();
			std::vector<Mesh> meshes = m_Model->GetMeshes();
			for (Mesh mesh : meshes)
			{
				mesh.Draw(m_TransformMatrix, Renderer::GetSceneData()->LightSources);

			}
		}
		std::shared_ptr<Model> GetModel() { return m_Model; }
	private:
		std::vector<Mesh> m_Meshes;
		std::shared_ptr<Model> m_Model;
		glm::mat4 m_TransformMatrix;
		


	};

}
