#type vertex

#version 450 core

#ifdef GL_ES
precision mediump float;  // 必须声明精度（ES 要求）

#endif

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aPrePos;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aNormal;

out vec2 TexCoords;

#include <assets/shaders/glsl/common/CommonViewStruct.glsl>
#include <assets/shaders/glsl/common/CommonTransformStruct.glsl>



uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
	TexCoords = aTexCoords;
	//gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);
    gl_Position = g_View.matProjectionView * g_Transform.matModel * vec4(aPos,1.0); //gl_Position-->Clip space
    //ndc Space = Clip Space/ Clip.w
}


#type fragment
#version 450 core


#include <assets/shaders/glsl/pbr/DisneyBSDF.glsl>


out vec4 FragColor;
in vec2 TexCoords;



void main(){
    MaterialTextureSample texSamples = SampleMaterialTexturesAuto(TexCoords);
	FragColor = texSamples.albedo;//vec4(1.0,1.0,0.0,1.0);// vec4(u_Material.diffuseColor,0.2);
//	FragColor =  vec4(1.0,0.0,0.0,1.0);//texture(u_Material.diffuse,TexCoords);//*vec4(u_Material.diffuseColor,1.0);
//	float depth = LinearizeDepth(gl_FragCoord.z)/far;
//	FragColor=vec4(vec3(depth),1.0);
}
