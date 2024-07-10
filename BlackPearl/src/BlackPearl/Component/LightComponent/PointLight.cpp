#include "pch.h"
#include "PointLight.h"




namespace BlackPearl {


	void PointLight::Init()
	{

		//data
		std::vector<float> lightVertices = {
			-0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			//Front face
			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			//Left face
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			//Right face
			 0.5f,  0.5f,  0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 //Bottom face
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,
			//Top face
			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f,  0.5f,
		};

		
		std::shared_ptr<Material> lightMaterial;
		std::shared_ptr<Material::TextureMaps> texture(DBG_NEW Material::TextureMaps());

		lightMaterial.reset(DBG_NEW Material("assets/shaders/glsl/PointLight.glsl", texture, m_LightProp.diffuse, m_LightProp.diffuse, m_LightProp.diffuse, m_LightProp.diffuse));
		VertexBufferLayout layout = {
			{ElementDataType::Float3,"aPos",false,0}
		};
		m_Mesh = std::make_shared<Mesh>(lightVertices, std::vector<unsigned int>(), lightMaterial, layout);//这里没有indices!

		m_ShadowMap.reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, m_ShadowMapPointLightWidth, m_ShadowMapPointLightWidth, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT));
		//m_Meshes.push_back(mesh);

		//std::shared_ptr<VertexBuffer> pointLightVertexBuffer;
		//pointLightVertexBuffer.reset(DBG_NEW VertexBuffer(lightVertices));
		//m_Shader.reset(DBG_NEW Shader("assets/shaders/PointLight.glsl"));
		//m_VertexArray.reset(DBG_NEW VertexArray());
		//pointLightVertexBuffer->SetBufferLayout({
		//	{ElementDataType::Float3,"aPos",false}
		//	});
		//m_VertexArray->AddVertexBuffer(pointLightVertexBuffer);








	}
	void PointLight::UpdateAttenuation(unsigned int maxdistance)
	{
		switch (maxdistance)
				{
				case 7:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.7f, m_Attenuation.quadratic = 1.8f;
					break;
				case 13:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.35f, m_Attenuation.quadratic = 0.44f;
					break;
				case 20:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.22f, m_Attenuation.quadratic = 0.20f;
					break;
				case 32:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.14f, m_Attenuation.quadratic = 0.07f;
					break;
				case 50:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.09f, m_Attenuation.quadratic = 0.032f;
					break;
				case 65:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.07f, m_Attenuation.quadratic = 0.017f;
					break;
				case 100:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.045f, m_Attenuation.quadratic = 0.0075f;
					break;
				case 160:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.0027f, m_Attenuation.quadratic = 0.0028f;
					break;
				case 200:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.022f, m_Attenuation.quadratic = 0.0019f;
					break;
				case 325:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.014f, m_Attenuation.quadratic = 0.0007f;
					break;
				case 600:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.007f, m_Attenuation.quadratic = 0.0002f;
					break;
				case 3250:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.0014f, m_Attenuation.quadratic = 0.000007f;
					break;
				default:
					m_Attenuation.constant = 1.0f, m_Attenuation.linear = 0.022f, m_Attenuation.quadratic = 0.0019f;

					GE_CORE_WARN("Undefined maxDistance! Failed to construct Attenuation! Attenuation=200")
						break;
				}
	}
}