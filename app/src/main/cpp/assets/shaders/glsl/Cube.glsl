#type vertex

#version 450 core

#ifdef GL_ES
precision mediump float;  // 必须声明精度（ES 要求）

#endif


layout(location = Slot_aPos) in vec3 aPos;
layout(location = Slot_aPrePos) in vec3 aPrePos;
layout(location = Slot_aNormal) in vec3 aNormal;
layout(location = Slot_aTexCoords) in vec2 aTexCoords;

out vec2 TexCoords;

#include <assets/shaders/glsl/common/CommonViewStruct.glsl>
#include <assets/shaders/glsl/common/CommonTransformStruct.glsl>




//uniform mat4 u_Model;
//uniform mat4 u_ProjectionView;

void main()
{
	TexCoords = aTexCoords;
	//gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);
    gl_Position = g_View.matProjectionView * g_Transform.matModel * vec4(aPos,1.0); //gl_Position-->Clip space
    //ndc Space = Clip Space/ Clip.w
}


#type fragment
#version 450 core



out vec4 FragColor;
in vec2 TexCoords;

#include <assets/shaders/glsl/common/CommonForwardStruct.glsl>

void main(){
    MaterialTextureSample texSamples = SampleMaterialTexturesAuto(TexCoords);
	FragColor = texSamples.albedo;//vec4(1.0,1.0,0.0,1.0);// vec4(u_Material.diffuseColor,0.2);
}
