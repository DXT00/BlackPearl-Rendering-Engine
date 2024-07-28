#pragma once
#include "Light.h"
#include <glm/glm.hpp>
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/RHI/RHITexture.h"
namespace BlackPearl {

	class PointLight :public Light
	{
	public:
		struct Attenuation {
			unsigned int maxDistance;
			float constant;
			float linear;
			float quadratic;
			Attenuation() :maxDistance(50), constant(1.0f), linear(0.09f), quadratic(0.032) {}
			Attenuation(unsigned int maxDistance) :maxDistance(maxDistance) {
				switch (maxDistance)
				{
				case 7:
					constant = 1.0f, linear = 0.7f, quadratic = 1.8f;
					break;
				case 13:
					constant = 1.0f, linear = 0.35f, quadratic = 0.44f;
					break;
				case 20:
					constant = 1.0f, linear = 0.22f, quadratic = 0.20f;
					break;
				case 32:
					constant = 1.0f, linear = 0.14f, quadratic = 0.07f;
					break;
				case 50:
					constant = 1.0f, linear = 0.09f, quadratic = 0.032f;
					break;
				case 65:
					constant = 1.0f, linear = 0.07f, quadratic = 0.017f;
					break;
				case 100:
					constant = 1.0f, linear = 0.045f, quadratic = 0.0075f;
					break;
				case 160:
					constant = 1.0f, linear = 0.0027f, quadratic = 0.0028f;
					break;
				case 200:
					constant = 1.0f, linear = 0.022f, quadratic = 0.0019f;
					break;
				case 325:
					constant = 1.0f, linear = 0.014f, quadratic = 0.0007f;
					break;
				case 600:
					constant = 1.0f, linear = 0.007f, quadratic = 0.0002f;
					break;
				case 3250:
					constant = 1.0f, linear = 0.0014f, quadratic = 0.000007f;
					break;
				default:
					GE_CORE_ERROR("Undefined maxDistance! Failed to construct Attenuation!")
						break;
				}

			}
			//Ĭ�Ͼ���50 constant(1.0f),linear(0.09f),quadratic(0.032){}
			//Ĭ�Ͼ���3250constant(1.0f),linear(0.0014f),quadratic(0.000007)
			//���https://learnopengl-cn.github.io/02%20Lighting/05%20Light%20casters/
		};
		PointLight(Props props = Props())
			:Light() {
			SetProps(props);
			Init();
		}
		virtual ~PointLight() = default;
		virtual void Init() override;

		inline void SetDiffuse(math::float3 diffuse) {
			m_LightProp.diffuse = diffuse;
			m_Mesh->SetMaterialColor({ m_LightProp.ambient,m_LightProp.diffuse,m_LightProp.specular ,m_LightProp.emission });

		}
		inline void SetAmbient(math::float3 ambient) {
			m_LightProp.ambient = ambient;
			m_Mesh->SetMaterialColor({ m_LightProp.ambient,m_LightProp.diffuse,m_LightProp.specular ,m_LightProp.emission });

		}
		inline void SetSpecular(math::float3 specular) {
			m_LightProp.specular = specular;
			m_Mesh->SetMaterialColor({ m_LightProp.ambient,m_LightProp.diffuse,m_LightProp.specular ,m_LightProp.emission });

		}
		inline void SetEmission(math::float3 emission) {
			m_LightProp.emission = emission;
			m_Mesh->SetMaterialColor({ m_LightProp.ambient,m_LightProp.diffuse,m_LightProp.specular ,m_LightProp.emission });

		}

		void UpdateMesh(Props props) {
			SetProps(props);
			m_Mesh->SetMaterialColor({ props.ambient,props.diffuse,props.specular ,props.emission });
			/*		std::shared_ptr<Material> lightMaterial = m_Mesh.GetMaterial();
					lightMaterial->SetMaterialColor({ props.ambient,props.diffuse,props.specular ,props.emission});*/


		}
		inline void SetAttenuation(const Attenuation& attenuation) { m_Attenuation = attenuation;}
		inline void SetAttenuation(unsigned int maxdistance) { m_Attenuation.maxDistance = maxdistance; UpdateAttenuation(maxdistance); }
		void UpdateAttenuation(unsigned int maxdistance);
		inline Attenuation GetAttenuation() const { return m_Attenuation; }
		virtual inline LightType GetType() override { return LightType::PointLight; }

		TextureHandle GetShadowMap()const { return m_ShadowMap; }
		unsigned int GetShadowMapWidth() const{ return m_ShadowMapPointLightWidth; }
		unsigned int GetShadowMapHeight() const { return m_ShadowMapPointLightHeight; }

		inline std::shared_ptr<Mesh> GetMeshes()const { return m_Mesh; }

	private:
		std::shared_ptr<Mesh> m_Mesh;
		Attenuation m_Attenuation;
		/* ÿ��PointLight����һ���ɼ�����Χ��ȵ�ShadowMap */
		TextureHandle m_ShadowMap;
		unsigned int m_ShadowMapPointLightWidth = 1024;
		unsigned int m_ShadowMapPointLightHeight = 1024;
	};

}