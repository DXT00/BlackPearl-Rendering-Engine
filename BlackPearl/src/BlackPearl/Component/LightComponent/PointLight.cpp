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

		lightMaterial.reset(DBG_NEW Material("assets/shaders/PointLight.glsl", texture, {}, this->GetLightProps().diffuse, {}, {}));
		VertexBufferLayout layout = {
			{ElementDataType::Float3,"aPos",false}
		};
		m_Mesh =Mesh(lightVertices, std::vector<unsigned int>(), lightMaterial, layout);//这里没有indices!

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
}