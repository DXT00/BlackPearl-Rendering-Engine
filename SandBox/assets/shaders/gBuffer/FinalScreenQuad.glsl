#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;


void main()
{
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos,1.0);
}

#type fragment
#version 430 core

out vec4 FragColor;
in vec2 TexCoords;

struct Material{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 emissionColor;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	sampler2D normal;
	sampler2D height;

	float shininess;

};
uniform Material u_Material;
uniform sampler2D u_FinalScreenTexture;

const float offset = 1.0 / 300.0; 
void main(){

	vec3 color =texture(u_FinalScreenTexture, TexCoords).rgb;


	//HDR tonemapping
	 color = color / (color + vec3(1.0));
	//gamma correction
    color = pow(color, vec3(1.0/2.2));  

	FragColor = vec4(color, 1.0);

}
