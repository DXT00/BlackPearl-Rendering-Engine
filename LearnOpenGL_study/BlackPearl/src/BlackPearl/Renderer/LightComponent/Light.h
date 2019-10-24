#pragma once
#include"BlackPearl/Renderer/VertexArray.h"
//#include "BlackPearl/Renderer/Shader.h"
#include "BlackPearl/Component/Component.h"
#include<glm/glm.hpp>
namespace BlackPearl {

	enum class LightType {
		ParallelLight = 0,
		PointLight,
		SpotLight
	};

	class Light :public Component<Light>
	{
	public:
		struct Props {

			glm::vec3 ambient;
			glm::vec3 diffuse;
			glm::vec3 specular;
			Props() : ambient({ 1.0f,1.0f,1.0f }), diffuse({ 1.0f,1.0f,1.0f }), specular({ 1.0f,1.0f,1.0f }) {}
			Props(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : ambient(ambient), diffuse(diffuse), specular(specular) {}

		};
		//position{2.2f,1.0f,2.0f}
		Light(EntityManager* entityManager, Entity::Id id)
			:Component(entityManager,id) {};
		virtual ~Light() = default;

		inline Props GetLightProps() const { return  m_LightProp; }
		//	virtual std::shared_ptr<VertexArray> GetVertexArray() = 0;
			//virtual std::shared_ptr<Shader> GetShader() = 0;
		virtual LightType GetType() = 0;
		virtual void Init() = 0;
		static Light* Create(
			EntityManager * entityManager, Entity::Id id,
			LightType type,
			const glm::vec3& position = { 2.2f,1.0f,2.0f },
			const glm::vec3& direction = { -0.2f, -1.0f, -0.3f },
			const float cutOffAngle = glm::cos(glm::radians(20.0f)),
			const float outterCutOffAngle = glm::cos(glm::radians(30.0f)),
			Props props = Props()
		);
		inline void SetProps(const Props& props) {
			m_LightProp.ambient = props.ambient;
			m_LightProp.diffuse = props.diffuse;
			m_LightProp.specular = props.specular;

		}
	protected:
		Props m_LightProp;

	};

}