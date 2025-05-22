#type vertex

#version 450 core

#ifdef GL_ES
precision mediump float;  // 必须声明精度（ES 要求）
#endif

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aPrePos;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aNormal;

#if USE_TBN
layout(location = 4) in vec3 aTangent;
#endif

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;
#if USE_TBN
out vec3 v_Tangent;
#endif

#include <assets/shaders/glsl/common/CommonViewStruct.glsl>
#include <assets/shaders/glsl/common/CommonTransformStruct.glsl>


void main()
{
    gl_Position = g_View.matProjectionView * g_Transform.matModel * vec4(aPos,1.0);

    v_TexCoord = aTexCoords;
    v_FragPos = vec3(g_Transform.matModel* vec4(aPos,1.0));
    v_Normal =  mat3(g_Transform.matInvModel)* aNormal;
#if USE_TBN
	v_Tangent =  mat3(g_Transform.matInvModel)*aTangent;
#endif
}


#type fragment
#version 450 core


#include <assets/shaders/glsl/bsdf/BSDF.glsl>

#include <forward_cb.h>

#include <assets/shaders/glsl/common/CommonViewStruct.glsl>

#include <assets/shaders/glsl/common/CommonForwardStruct.glsl>



out vec4 FragColor;
in vec2 v_TexCoord;
in vec3 v_Normal;		
in vec3 v_FragPos;	
#if USE_TBN
in vec3	v_Tangent;
#endif
void main(){


  
	  SurfaceGeometry geom;
      geom.position = v_FragPos;
      geom.normal = normalize(v_Normal);
      geom.viewDir = normalize(g_View.cameraPos - v_FragPos); // Assuming eye is at (0,0,0)
#if USE_TBN
      geom.tangent = normalize(v_Tangent);
      geom.bitangent = normalize(cross(geom.normal, geom.tangent));
#else
      getTBN(v_FragPos, v_TexCoord, v_Normal, geom.tangent, geom.bitangent);
#endif

#if COOK
    MaterialSample mat = CreateCookBSDFMaterial(geom, g_Mat, v_TexCoord);
#elif (Disney)
    DisneyMaterialSample mat = CreateDisneyBSDFMaterial(geom, g_Mat, v_TexCoord);
#endif

   for(uint nLight = 0; nLight < g_ForwardLight.numLights; nLight++)
   {
       LightConstants light = g_ForwardLight.lights[i];
       FragColor += ShadeSurface(light, geom, mat);
   }
    

}
