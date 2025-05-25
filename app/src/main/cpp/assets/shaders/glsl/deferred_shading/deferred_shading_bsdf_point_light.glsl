#type vertex

#version 450 core

#ifdef GL_ES
precision mediump float;  // 必须声明精度（ES 要求）
#endif

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;


#include <assets/shaders/glsl/common/CommonViewStruct.glsl>

#include <assets/shaders/glsl/common/CommonTransformStruct.glsl>

void main()
{
  	v_TexCoord = aTexCoords;

	gl_Position = vec4(aPos,1.0);

}


#type fragment
#version 450 core


#include <assets/shaders/glsl/pbr/BSDF.glsl>
#include <assets/shaders/glsl/gBuffer.glsl>

out vec4 FragColor;
in vec2 v_TexCoord;

#include <assets/shaders/glsl/common/CommonViewStruct.glsl>
#include <assets/shaders/glsl/common/CommonDeferredStruct.glsl>
#include <assets/shaders/glsl/common/CommonTransform.glsl>
void main(){



    GbufferData GBuffer = DecodeGbuffer(t_gGbufferA, t_gGbufferB, t_gGbufferC);

    float2 pixelPos = v_TexCoord * g_View.viewportSize; //v_TexCoord range [0,1]
    
    float3 worldPos = ScreenSpaceToWorldPosition(pixelPos, Gbuffer.Depth);

   
    
    


      SurfaceGeometry geom;
      geom.position = worldPos;
      geom.normal = Gbuffer.WorldNormal;
      geom.viewDir = normalize(g_View.cameraPos - worldPos); // Assuming eye is at (0,0,0)
#if USE_TBN
//      todo:: GBuffer.WorldTangent = half3(0); //TODO:: get Aniso flag
//      geom.tangent = normalize(v_Tangent);
//      geom.bitangent = normalize(cross(geom.normal, geom.tangent));
      getTBN(geom.normal, v_TexCoord, geom.normal, geom.tangent, geom.bitangent);

#else
      getTBN(worldPos, v_TexCoord, geom.normal, geom.tangent, geom.bitangent);
#endif


#if COOK
    MaterialSample mat = GetMaterialFromGBuffer(GBuffer);
#elif (Disney)
    DisneyMaterialSample mat = GetMaterialFromGBuffer(GBuffer);
#endif
    mat.emissive = t_gSceneColor.xyz;

   for(uint nLight = 0; nLight < g_DeferredLight.numLights; nLight++)
   {
       LightConstants light = g_DeferredLight.lights[i];
       FragColor += ShadeSurface(light, geom, mat);
   }
   half IndirectIrradiance = GBuffer.IndirectIrradiance;
    
    //direct light

    //ibl
}
