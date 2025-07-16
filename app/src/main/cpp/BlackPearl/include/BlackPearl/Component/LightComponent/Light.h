#pragma once
#include"BlackPearl/Renderer/VertexArray.h"
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/Renderer/Shadow/ShadowMap.h"
using namespace BlackPearl::math;

#include "hlsl/core/light_cb.h"
#include <glm/glm.hpp>
namespace BlackPearl {

	enum class LightType {
		DirectionLight = 0,
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
			/* angularSize（角大小）是一个关键概念，用于描述 物体或像素在视线方向上的张角（即从观察者视角看，物体占据的角度范围）。它的单位通常是 弧度（radians）。以下是具体解释：
			1. angularSize 的定义
			物理意义：表示某个物体（如太阳、像素）在观察者眼中的 视觉大小。
			例如：太阳的角大小约为 0.53°（≈0.0092 弧度），月亮类似。
			*/
			float angularSize = 0.53f;
			Props() : ambient(math::float3(1.0f)), diffuse(math::float3(1.0f)), specular(math::float3(0.0f)), emission(math::float3(0.0f)),intensity(0.5f), area(1.0f), shadowBias(0.08) {}
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
			m_LightProp.angularSize = props.angularSize;

		}
	

	public:        
		std::shared_ptr<IShadowMap> shadowMap = nullptr;
		int shadowChannel = -1;
        virtual void FillLightConstants(LightConstants& lightConstants);

	protected:

		Props m_LightProp;
		Props m_LightLastProp;

	};

}