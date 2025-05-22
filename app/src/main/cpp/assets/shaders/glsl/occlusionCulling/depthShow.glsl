#type vertex
#version 450 core

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
#version 450 core

out vec4 FragColor;
in vec2 TexCoords;


uniform sampler2D u_DepthMap;
void main(){

	
	vec3 color = texture(u_DepthMap, TexCoords).rgb;

	FragColor = vec4(color, 1.0);

//FragColor = vec4(vec3(1.0 - texture(u_Material.diffuse, TexCoords)), 1.0);
}