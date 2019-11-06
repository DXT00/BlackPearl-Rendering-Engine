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

		std::shared_ptr<Shader> lightShader;
		lightShader.reset(DBG_NEW Shader("assets/shaders/PointLight.glsl"));		
		std::shared_ptr<Material> lightMaterial;
		std::shared_ptr<MaterialColor> lightColor(DBG_NEW MaterialColor(MaterialColor::Type::DiffuseColor, this->GetLightProps().diffuse));
		std::vector<std::shared_ptr<MaterialColor>>materialColors;
		materialColors.push_back(lightColor);

		lightShader->Bind();
		lightShader->SetUniformVec3f("u_Material.diffuseColor", lightColor->Get());


		lightMaterial.reset(DBG_NEW Material(lightShader, std::vector<std::shared_ptr<Texture>>(), materialColors));
		VertexBufferLayout layout = {
			{ElementDataType::Float3,"aPos",false}
		};
		Mesh mesh(lightVertices, std::vector<unsigned int>(), lightMaterial, layout);//这里没有indices!

		m_Meshes.push_back(mesh);

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