#pragma once
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/Model/Model.h"
#include "BlackPearl/Renderer/Renderer.h"

//MeshRenderer只负责渲染
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
		}
		~MeshRenderer() {};

		void UpdateTransformMatrix(glm::mat4 transformMatrix);
	/*	void DrawMeshes();
		void DrawLight();
*/
		inline std::shared_ptr<Model>GetModel()  const { return m_Model; }
		inline std::vector<Mesh>     GetMeshes() const {
			return m_Model != nullptr ? m_Model->GetMeshes() : m_Meshes;
		}

		void SetTexture(unsigned int meshIndex, const std::shared_ptr<Texture>& texture);
		void SetTextures(const std::shared_ptr<Texture>& texture);
		void SetModelTexture(unsigned int meshIndex, const std::shared_ptr<Texture>& texture);
		void SetModelTextures(const std::shared_ptr<Texture> &texture);

		void SetShaders(const std::string& image);
		void SetShaders(const std::shared_ptr<Shader> &shader);

		void SetEnableCullFace(bool isEnable) { m_EnableCullFace = isEnable; }
		bool GetEnableCullFace()const { return m_EnableCullFace; }

		void SetEnableRender(bool enable) { m_EnableRender = enable; }
		bool GetEnableRender()const { return m_EnableRender; }
	private:
		//开启和禁止渲染
		bool m_EnableRender = true;
		std::vector<Mesh> m_Meshes;
		std::shared_ptr<Model> m_Model = nullptr;
		glm::mat4 m_TransformMatrix;
		
		bool m_EnableCullFace = true;

	};

}

