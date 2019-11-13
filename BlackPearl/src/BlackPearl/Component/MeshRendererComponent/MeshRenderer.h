#pragma once
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/Model/Model.h"
#include "BlackPearl/Renderer/Renderer.h"

//MeshRenderer÷ª∏∫‘‰÷»æ
namespace BlackPearl {
	class MeshRenderer:public Component<MeshRenderer>
	{
	public:
		//get transformMatrix from Object's Transform Component!
		MeshRenderer(EntityManager* entityManager, Entity::Id id,const std::vector<Mesh> &meshes,glm::mat4 transformMatrix)
			:Component(entityManager, id,Component::Type::MeshRenderer),m_Meshes(meshes),m_TransformMatrix(transformMatrix){}

		MeshRenderer(EntityManager* entityManager, Entity::Id id,const Mesh &mesh,glm::mat4 transformMatrix)
			:Component(entityManager, id, Component::Type::MeshRenderer), m_TransformMatrix(transformMatrix) {
			m_Meshes.push_back(mesh);
		}
		MeshRenderer(EntityManager* entityManager, Entity::Id id, const std::shared_ptr<Model>& model, glm::mat4 transformMatrix)
			:Component(entityManager, id, Component::Type::MeshRenderer),m_Model(model), m_TransformMatrix(transformMatrix) {
			//m_Meshes = m_Model->GetMeshes();
		}
		~MeshRenderer() {};
		void UpdateTransformMatrix(glm::mat4 transformMatrix);
		void DrawMeshes();
		void DrawModel();
		void DrawLight();
		std::shared_ptr<Model> GetModel() { return m_Model; }
		std::vector<Mesh> GetMeshes() { return m_Meshes; }

		void SetTexture(unsigned int meshIndex,std::shared_ptr<Texture> texture);
	private:
		std::vector<Mesh> m_Meshes;
		std::shared_ptr<Model> m_Model = nullptr;
		glm::mat4 m_TransformMatrix;
		


	};

}

