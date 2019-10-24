#include "pch.h"
#include "PointLight.h"




namespace BlackPearl {


	void PointLight::Init()
	{

		//data
		std::vector<float> lightVertices = {
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};


		std::shared_ptr<VertexBuffer> pointLightVertexBuffer;
		pointLightVertexBuffer.reset(DBG_NEW VertexBuffer(lightVertices));
		m_Shader.reset(DBG_NEW Shader("assets/shaders/PointLight.glsl"));
		m_VertexArray.reset(DBG_NEW VertexArray());
		pointLightVertexBuffer->SetBufferLayout({
			{ElementDataType::Float3,"aPos",false}
			});
		m_VertexArray->AddVertexBuffer(pointLightVertexBuffer);



		m_Shader->Bind();
		m_Shader->SetUniformVec3f("u_LightColor", this->GetLightProps().diffuse);




	}
}