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

layout(std140, binding = 0) uniform ForwardShadingViewConstants {
    vec2      viewportOrigin;
    vec2      viewportSize;
    mat4 matProjectionView;
    mat4 matView;
    mat4 matProjection;
    vec3 cameraPos;
    vec3 cameraRot;
} g_ForwardView;


layout(std140, binding = 1) uniform TransformConstants {
    mat4 matModel;

} g_Transform;



uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
	TexCoords = aTexCoords;
    gl_Position = g_ForwardView.matProjectionView * g_Transform.matModel * vec4(aPos,1.0);
}


#type fragment
#version 450 core


#include <assets/shaders/pbr/BSDF.glsl>


out vec4 FragColor;
in vec2 TexCoords;


void main(){
    MaterialTextureSample texSamples = SampleMaterialTexturesAuto(TexCoords);
	FragColor = texSamples.baseOrDiffuse;

}
