#type vertex

#version 450 core

#ifdef GL_ES
precision mediump float;  // 必须声明精度（ES 要求）
#endif

layout(location = Slot_aPos) in vec3 aPos;
layout(location = Slot_aPrePos) in vec3 aPrePos;
layout(location = Slot_aTexCoords) in vec2 aTexCoords;
layout(location = Slot_aNormal) in vec3 aNormal;

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

#ifdef DEFERRED_SHADING_PASS
#undef DEFERRED_SHADING_PASS
#endif

#define DEFERRED_SHADING_PASS 1



out vec4 FragColor;
in vec2 v_TexCoord;

#include <assets/shaders/glsl/common/CommonViewStruct.glsl>
#include <assets/shaders/glsl/common/CommonDeferredStruct.glsl>
#include <assets/shaders/glsl/common/CommonTransform.glsl>
#include <assets/shaders/glsl/bsdf/BSDF.glsl>
#include <assets/shaders/glsl/gBuffer/gBuffer.glsl>

void main(){

    GBufferData GBuffer = DecodeGBuffer(v_TexCoord);

    float2 pixelPos = v_TexCoord * g_View.viewportSize; //v_TexCoord range [0,1]
    
    float3 worldPos = ScreenSpaceToWorldPosition(pixelPos, GBuffer.Depth);

      SurfaceGeometry geom;
      geom.position = worldPos;
      geom.normal = GBuffer.WorldNormal;
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
    mat.emissive = texture(t_gSceneColor,v_TexCoord).rgb;

   for(uint nLight = 0; nLight < g_DeferredLight.numLights; nLight++)
   {
       LightConstants light = g_DeferredLight.lights[nLight];
       FragColor += ShadeSurface(light, geom, mat);
   }
   half IndirectIrradiance = GBuffer.IndirectIrradiance;
    
    //direct light

//    //ibl
//    if(v_TexCoord.x <0.5 && v_TexCoord.y < 0.5){
//     FragColor = vec4(1,0,0,1);
//    }else{
//        FragColor = texture(t_gGbufferA,v_TexCoord);//vec4(texture(t_gGbufferC,v_TexCoord).xyz,1.0);
//
//    }
}
