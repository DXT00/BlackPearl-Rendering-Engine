#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

/* draw quad */
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
out vec2 v_TexCoords;


void main()
{
	v_TexCoords = aTexCoords;

	gl_Position =vec4(aPos,1.0);
 //u_ProjectionView* u_Model * 
}

#type fragment
#version 430 core

in vec2 v_TexCoords;
//uniform sampler2D gPosition;
//uniform sampler2D gNormal;
//uniform sampler2D gDiffuse_Roughness;
//uniform sampler2D gSpecular_Mentallic;
uniform sampler2D gAmbientGI_AO;

out vec4 FragColor;
uniform vec2 gScreenSize;

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}

void main(){
	vec2 texCoords = CalcTexCoord();
	vec3 outColor = texture(gAmbientGI_AO,texCoords).rgb;
	FragColor = vec4(outColor,1.0);
}