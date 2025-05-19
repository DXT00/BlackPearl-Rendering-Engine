#type vertex

#version 450 core

#ifdef GL_ES
precision mediump float;  // 必须声明精度（ES 要求）
#endif

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aPrePos;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aNormal;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;


layout(std140, binding = 0) uniform ForwardShadingViewConstants {
    vec2 viewportOrigin;
    vec2 viewportSize;
    mat4 matProjectionView;
    mat4 matView;
    mat4 matProjection;
    vec3 cameraPos;
    vec3 cameraRot;
} g_ForwardView;


layout(std140, binding = 1) uniform TransformConstants {
    mat4 matModel;
    mat4 matInvModel;

} g_Transform;


void main()
{
    gl_Position = g_ForwardView.matProjectionView * g_Transform.matModel * vec4(aPos,1.0);

    v_TexCoord = aTexCoords;
    v_FragPos = vec3(g_Transform.matModel* vec4(aPos,1.0));
    v_Normal =  mat3(g_Transform.matInvModel)* aNormal;

}


#type fragment
#version 450 core


#include <assets/shaders/pbr/BSDF.glsl>

#include <forward_cb.h>


layout(std140, binding = 8) uniform ForwardShadingUBO {
    ForwardShadingLightConstants g_ForwardLight;

} g_Transform;


out vec4 FragColor;
in vec2 v_TexCoord;
in vec3 v_Normal;		
in vec3 v_FragPos;	

void main(){
	SurfaceGeometry geom;
      geom.position = v_FragPos;
      geom.normal = normalize(vNormal);
      //TODO::
//      geom.viewDir = normalize(-vPosition); // Assuming eye is at (0,0,0)
//      geom.tangent = normalize(vTangent);
//      geom.bitangent = normalize(cross(geom.normal, geom.tangent));

#ifdef COOK
    MaterialConstant matConst = CreateCookBSDFMaterial(geom, g_Mat, TexCoords);
#elif defined (Disney)
    DisneyMaterialConstant matConst = CreateDisneyBSDFMaterial(geom, g_Mat, TexCoords);
#endif

   for(uint nLight = 0; nLight < g_ForwardLight.numLights; nLight++)
   {
       LightConstants light = g_ForwardLight.lights[i];
       FragColor += ShadeSurface(light, geom, matConst);

   }
    

}
