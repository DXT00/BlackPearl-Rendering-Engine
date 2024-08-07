#pragma once
#include"BlackPearl/Renderer/VertexArray.h"
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/Renderer/Shadow/ShadowMap.h"
using namespace BlackPearl::math;

#include "hlsl/core/light_cb.h"
#include <glm/glm.hpp>
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

			math::float3 ambient;
			math::float3 diffuse;
			math::float3 specular;
			math::float3 emission;
			float intensity;
			float area;
			float shadowBias;
			Props() : ambient({ 0.0f,0.0f,0.0f }), diffuse({ 1.0f,1.0f,1.0f }), specular({ 0.0f,0.0f,0.0f }), emission({0.0f,0.0f,0.0f}),intensity(1.0f), area(1.0f), shadowBias(0.08) {}
			Props(math::float3 ambient, math::float3 diffuse, math::float3 specular, math::float3 emission,float intensity, float area = 1.0, float shadowBias = 0.08 )
				: ambient(ambient), diffuse(diffuse), specular(specular),emission(emission),intensity(intensity), area(area), shadowBias(shadowBias){}
			bool operator==(Props& rhs) const {
				return (ambient == rhs.ambient &&
					diffuse == rhs.diffuse &&
					specular == rhs.specular &&
					emission == rhs.emission);
			}
			
		};
		//position{2.2f,1.0f,2.0f}
		Light()
			:Component(Type::Light) {};
		virtual ~Light() = default;

		Props GetLightProps() const { return  m_LightProp; }
		Props GetLightLastProps() const { return  m_LightLastProp; }

		virtual LightType GetType() = 0;
		virtual void Init() = 0;
		static Light* Create(
			LightType type,
			const math::float3& position = { 2.2f,1.0f,2.0f },
			const math::float3& direction = { -0.2f, -1.0f, -0.3f },
			const float cutOffAngle = glm::cos(glm::radians(20.0f)),
			const float outterCutOffAngle = glm::cos(glm::radians(30.0f)),
			Props props = Props()
		);
		inline void SetProps(const Props& props) {
			m_LightLastProp = m_LightProp;
			m_LightProp.ambient = props.ambient;
			m_LightProp.diffuse = props.diffuse;
			m_LightProp.specular = props.specular;
			m_LightProp.emission = props.emission;
			m_LightProp.intensity = props.intensity;
			m_LightProp.area = props.area;
			m_LightProp.shadowBias = props.shadowBias;
		}
	

		void Light::FillLightConstants(LightConstants& lightConstants) const;
	public:        
		std::shared_ptr<IShadowMap> shadowMap = nullptr;
		int shadowChannel = -1;

	protected:
		Props m_LightProp;
		Props m_LightLastProp;

	};

}