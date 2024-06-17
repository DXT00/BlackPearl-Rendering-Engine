#pragma once
#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_ProjectionView;

out vec3 v_FragPos;

void  main(){
	
	v_FragPos = aPos;
	//ע��:Camera ��View Matrix��ƽ���������ٶ���պ�����Ч����պ���ֻ����ת����
	vec4 pos = u_Projection*mat4(mat3(u_View))* vec4(aPos,1.0);
	//vec4 pos = u_ProjectionView* vec4(aPos,1.0);

	gl_Position = pos.xyww;
	//����gl_Position = (x/w,y/w,w/w) ,zһֱ=1����պ��������������ͷ��

}

#type fragment
#version 430 core
//uniform struct Material{
//	vec3 ambientColor;
//	vec3 diffuseColor;
//	vec3 specularColor;
//	vec3 emissionColor;
//	sampler2D diffuse;
//	sampler2D specular;
//	sampler2D emission;
//	sampler2D normal;
//	sampler2D height;
//	samplerCube cube;
//	float shininess;
//
//}u_Material;
uniform Material u_Material;
uniform samplerCube cubeMap;
in vec3 v_FragPos;
out vec4 FragColor;

void main(){
	
	vec3 color = texture(cubeMap,v_FragPos).rgb;
	//HDR toneMap and gamma correct
	color = color/(color+vec3(1.0));
	color = pow(color,vec3(1.0/2.2));

	FragColor = vec4(color,1.0);

}