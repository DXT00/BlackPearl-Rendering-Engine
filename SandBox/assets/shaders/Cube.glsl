#pragma once

#type vertex
#version 330 core
layout(location = 0) in vec3 aPos;

out vec2 TexCoord;
		
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
	gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);
}



#type fragment
#version 330 core
out vec4 FragColor;

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

void main(){
	FragColor = vec4(u_Material.diffuseColor,1.0);
}
