#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <initializer_list>
namespace BlackPearl {

	class MaterialColor
	{
	public:
		enum Type {
			AmbientColor,
			DiffuseColor,
			SpecularColor,
			EmissionColor

		};
		struct Color {
            glm::vec3 ambientColor = glm::vec3(0.0);
            glm::vec3 diffuseColor = glm::vec3(0.0);
            glm::vec3 specularColor = glm::vec3(0.0);
            glm::vec3 emissionColor = glm::vec3(0.0);

            //	float intensity = 1.0f;
            Color(std::initializer_list<glm::vec3> color) {
                std::vector<glm::vec3> c = color;

                ambientColor = c[0];
                diffuseColor = c[1];
                specularColor = c[2];
                emissionColor = c[3];
            }
            Color(){}
        };
		//
        MaterialColor(){}
		MaterialColor(Type type, Color & color)
			:m_Type(type), m_Color(color) {}


		~MaterialColor() = default;
		inline Type GetType() const { return m_Type; }
		inline Color Get() const { return m_Color; }
		void SetColor(const Color& color) {
			m_Color.ambientColor  = color.ambientColor;
			m_Color.diffuseColor  = color.diffuseColor;
			m_Color.specularColor = color.specularColor;
			m_Color.emissionColor = color.emissionColor;
		

		}
		//float GetIntensity() const { return m_Color.intensity; }
		//void SetIntensity(float intensity) { m_Color.intensity = intensity; }
		void SetAmbientColor(const glm::vec3 color) {
			m_Color.ambientColor = color;
		}
		void SetDiffuseColor(const glm::vec3 color) {
			m_Color.diffuseColor = color;
		}	
		void SetSpecularColor(const glm::vec3 color) {
			m_Color.specularColor = color;
		}
		void SetEmissionColor(const glm::vec3 color) {
			m_Color.emissionColor = color;
		}
	private:
		Type m_Type;

		Color m_Color;



	};

}