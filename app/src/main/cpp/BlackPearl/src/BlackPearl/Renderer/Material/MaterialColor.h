#pragma once
#include <glm/glm.hpp>
#include "BlackPearl/Math/Math.h"
namespace BlackPearl {

	class MaterialColor
	{
	public:
		enum Type {
			AmbientColor,
			DiffuseColor,
			SpecularColor,
			emissiveColor

		};
		struct Color {
			math::float3 ambientColor = math::float3(0.0);
			math::float3 diffuseColor = math::float3(0.0);
			math::float3 specularColor = math::float3(0.0);
			math::float3 emissiveColor = math::float3(0.0);
		//	float intensity = 1.0f;
			
		};
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
			m_Color.emissiveColor = color.emissiveColor;
		}
		//float GetIntensity() const { return m_Color.intensity; }
		//void SetIntensity(float intensity) { m_Color.intensity = intensity; }
		void SetAmbientColor(const math::float3 color) {
			m_Color.ambientColor = color;
		}
		void SetDiffuseColor(const math::float3 color) {
			m_Color.diffuseColor = color;
		}	
		void SetSpecularColor(const math::float3 color) {
			m_Color.specularColor = color;
		}
		void SetEmissionColor(const math::float3 color) {
			m_Color.emissiveColor = color;
		}
	private:
		Type m_Type;

		Color m_Color;



	};

}