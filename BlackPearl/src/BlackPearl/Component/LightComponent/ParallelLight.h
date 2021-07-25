#pragma once
#include "Light.h"
namespace BlackPearl {

	class ParallelLight :public Light
	{
	public:
		ParallelLight(Props props = Props())
			:Light(),m_Direction({ -0.2f, -1.0f, -0.3f }) {
			SetProps(props);

			Init();
		}
		virtual ~ParallelLight() = default;
		virtual void Init() override;

		inline void SetDirection(const glm::vec3& direction) { m_Direction = direction; }
		inline glm::vec3 GetDirection() { return m_Direction; }
		void UpdateMesh(Props props) {
			SetProps(props);
			/*		std::shared_ptr<Material> lightMaterial = m_Mesh.GetMaterial();
					lightMaterial->SetMaterialColor({ props.ambient,props.diffuse,props.specular ,props.emission});*/


		}
		virtual inline LightType GetType() override { return LightType::ParallelLight; }
		//	virtual std::shared_ptr<VertexArray> GetVertexArray()override { return m_VertexArray; };
		//	virtual std::shared_ptr<Shader> GetShader() override { return m_Shader; };
	private:
		glm::vec3 m_Direction;

		//	std::shared_ptr<VertexArray> m_VertexArray;
		//	std::shared_ptr<Shader> m_Shader;
	};

}