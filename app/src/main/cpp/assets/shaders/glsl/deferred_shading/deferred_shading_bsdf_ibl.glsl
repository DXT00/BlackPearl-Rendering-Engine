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


#include <assets/shaders/glsl/common/CommonViewStruct.glsl>
#include <assets/shaders/glsl/common/CommonDeferredStruct.glsl>

void main()
{
    gl_Position = g_View.matProjectionView * g_Transform.matModel * vec4(aPos,1.0);

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

} ;


out vec4 FragColor;
in vec2 v_TexCoord;

layout (location = 0) out vec4 gSceneColor; //use for emissive color and fog
layout (location = 1) out vec4 gGbufferA;  //encode normal.xy + Encode IndirectIrradiance + reserve
layout (location = 2) out vec4 gGbufferB; // Metallic + Specular + Roughness + ShadingModelID / 255.0
layout (location = 3) out vec4 gGbufferC; 


void main(){
//	SurfaceGeometry geom;
//      geom.position = v_FragPos;
//      geom.normal = normalize(v_Normal);
      //TODO::
//      geom.viewDir = normalize(-vPosition); // Assuming eye is at (0,0,0)
//      geom.tangent = normalize(vTangent);
//      geom.bitangent = normalize(cross(geom.normal, geom.tangent));


    GBufferData GBuffer = DecodeGbuffer(gGbufferA, gGbufferB, gGbufferC);


#if COOK
    MaterialSample mat = GetMaterialFromGBuffer(GBuffer);
#elif (Disney)
    DisneyMaterialSample mat = GetMaterialFromGBuffer(GBuffer);
#endif

   for(uint nLight = 0; nLight < g_ForwardLight.numLights; nLight++)
   {
       LightConstants light = g_ForwardLight.lights[i];
       FragColor += ShadeSurface(light, geom, mat);

   }
   half IndirectIrradiance = GBuffer.IndirectIrradiance;
    
    //direct light

    //ibl
}
