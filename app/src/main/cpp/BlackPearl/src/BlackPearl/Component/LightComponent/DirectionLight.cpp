#include "pch.h"
#include "Component/LightComponent/DirectionLight.h"
#include <glm/gtc/matrix_transform.hpp>
#include "BlackPearl/Renderer/Renderer.h"

namespace BlackPearl {

	void DirectionLight::Init()
	{
		//ƽ�й�ֻ�ṩ�����򲢲���ҪShader��Ⱦ��Ҳ����Ҫ��������


		//data
		//float lightVertices[] = {
		//	-0.5f, -0.5f, -0.5f,
		//	 0.5f, -0.5f, -0.5f,
		//	 0.5f,  0.5f, -0.5f,
		//	 0.5f,  0.5f, -0.5f,
		//	-0.5f,  0.5f, -0.5f,
		//	-0.5f, -0.5f, -0.5f,

		//	-0.5f, -0.5f,  0.5f,
		//	 0.5f, -0.5f,  0.5f,
		//	 0.5f,  0.5f,  0.5f,
		//	 0.5f,  0.5f,  0.5f,
		//	-0.5f,  0.5f,  0.5f,
		//	-0.5f, -0.5f,  0.5f,

		//	-0.5f,  0.5f,  0.5f,
		//	-0.5f,  0.5f, -0.5f,
		//	-0.5f, -0.5f, -0.5f,
		//	-0.5f, -0.5f, -0.5f,
		//	-0.5f, -0.5f,  0.5f,
		//	-0.5f,  0.5f,  0.5f,

		//	 0.5f,  0.5f,  0.5f,
		//	 0.5f,  0.5f, -0.5f,
		//	 0.5f, -0.5f, -0.5f,
		//	 0.5f, -0.5f, -0.5f,
		//	 0.5f, -0.5f,  0.5f,
		//	 0.5f,  0.5f,  0.5f,

		//	-0.5f, -0.5f, -0.5f,
		//	 0.5f, -0.5f, -0.5f,
		//	 0.5f, -0.5f,  0.5f,
		//	 0.5f, -0.5f,  0.5f,
		//	-0.5f, -0.5f,  0.5f,
		//	-0.5f, -0.5f, -0.5f,

		//	-0.5f,  0.5f, -0.5f,
		//	 0.5f,  0.5f, -0.5f,
		//	 0.5f,  0.5f,  0.5f,
		//	 0.5f,  0.5f,  0.5f,
		//	-0.5f,  0.5f,  0.5f,
		//	-0.5f,  0.5f, -0.5f,
		//};


		//std::shared_ptr<VertexBuffer> lightVertexBuffer;
		//lightVertexBuffer.reset(new VertexBuffer(lightVertices, sizeof(lightVertices)));
		//m_Shader.reset(new Shader("assets/shaders/glsl/Light.glsl"));
		//m_VertexArray.reset(new VertexArray());
		//lightVertexBuffer->SetBufferLayout({
		//	{ElementDataType::Float3,"aPos",false}
		//	});
		//m_VertexArray->AddVertexBuffer(lightVertexBuffer);

		//m_Shader->Bind();
		//m_Shader->SetUniformVec3f("u_LightColor", this->GetLightProps().color);

		//

		//m_LightProp.ambient = { 0.2f, 0.2f, 0.2f };
		//m_LightProp.diffuse = { 0.5f, 0.5f, 0.5f };
		//m_LightProp.specular = { 1.0f, 1.0f,1.0f };

	}

    void DirectionLight::FillLightConstants(LightConstants& lightConstants) {

        lightConstants = DefaultLightConstants();
        lightConstants.direction = m_Direction;
        lightConstants.lightType = LightType_Directional;
        lightConstants.color = float3(1.0, 1.0, 0.0);// m_LightProp.diffuse;
        lightConstants.intensity = 3.0f;
       
    }
}