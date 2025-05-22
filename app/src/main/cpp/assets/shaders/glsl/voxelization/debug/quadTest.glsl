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
const float offset = 1.0 / 300.0; 
uniform sampler2D u_CubeFace;

void main(){


FragColor = vec4(texture(u_CubeFace, TexCoords).rgb, 1.0);

}
#pragma once
