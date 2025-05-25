#pragma once
#include "Light.h"
#include "hlsl/core/light_cb.h"
namespace BlackPearl {

	class DirectionLight :public Light
	{
	public:
		DirectionLight(Props props = Props())
			:Light(),m_Direction(math::float3( -0.2f, -1.0f, -0.3f )) {
			SetProps(props);

			Init();
		}
		virtual ~DirectionLight() = default;
		virtual void Init() override;

		inline void SetDirection(const math::float3& direction) { m_Direction = direction; }
		inline math::float3 GetDirection() const { return m_Direction; }
		void UpdateMesh(Props props) {
			SetProps(props);
			/*		std::shared_ptr<Material> lightMaterial = m_Mesh.GetMaterial();
					lightMaterial->SetMaterialColor({ props.ambient,props.diffuse,props.specular ,props.emission});*/


		}
		virtual inline LightType GetType() override { return LightType::DirectionLight; }

        virtual void FillLightConstants(LightConstants& lightConstants) override;

		//	virtual std::shared_ptr<VertexArray> GetVertexArray()override { return m_VertexArray; };
		//	virtual std::shared_ptr<Shader> GetShader() override { return m_Shader; };
	private:
		math::float3 m_Direction;

		//	std::shared_ptr<VertexArray> m_VertexArray;
		//	std::shared_ptr<Shader> m_Shader;
	};

}