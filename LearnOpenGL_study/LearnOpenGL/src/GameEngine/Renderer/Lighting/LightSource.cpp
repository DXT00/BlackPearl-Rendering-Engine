#include "pch.h"
#include "LightSource.h"

void LightSource::Init()
{
	//data
	float lightVertices[] = {
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
	//Shader
	const std::string lightvVertexSrc = R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;
	
		out vec2 TexCoord;
				
		uniform mat4 u_Model;
		uniform mat4 u_ProjectionView;

		void main()
		{
			gl_Position =u_ProjectionView* u_Model * vec4(aPos,1.0);
		}

	)";

	const std::string lightFragmentSrc = R"(
		#version 330 core
		out vec4 FragColor;
		
		uniform vec3 u_BlubColor;
		uniform vec3 u_LightColor;		
		
		void main(){
			FragColor = vec4(u_LightColor * u_BlubColor,1.0);
		}
	)";

	std::shared_ptr<VertexBuffer> lightVertexBuffer;
	lightVertexBuffer.reset(new VertexBuffer(lightVertices,sizeof(lightVertices)));
	m_Shader.reset(new Shader(lightvVertexSrc, lightFragmentSrc));

	m_VertexArray.reset(new VertexArray());
	lightVertexBuffer->SetBufferLayout({
		{ElementDataType::Float3,"aPos",false}
		});
	m_VertexArray->AddVertexBuffer(lightVertexBuffer);

	m_Shader->Bind();
	m_Shader->SetUniformVec3f("u_BlubColor", this->GetBlubColor());
	m_Shader->SetUniformVec3f("u_LightColor", this->GetLightColor());


}
