#type vertex

#version 450 core

#ifdef GL_ES
precision mediump float;  // 必须声明精度（ES 要求）
#endif

layout(location = Slot_aPos) in vec3 aPos;
layout(location = Slot_aNormal) in vec3 aNormal;
layout(location = Slot_aTexCoords) in vec2 aTexCoords;

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



#if !USE_GLES_PLS
out vec4 FragColor;
#endif

in vec2 v_TexCoord;

#include <assets/shaders/glsl/common/CommonViewStruct.glsl>
#include <assets/shaders/glsl/common/CommonDeferredStruct.glsl>
#include <assets/shaders/glsl/common/CommonTransform.glsl>
#include <assets/shaders/glsl/bsdf/BSDF.glsl>
#include <assets/shaders/glsl/gBuffer/gBuffer.glsl>

void main(){
    vec2 uv = v_TexCoord;
#if USE_GLES_PLS
    uv = vec2(uv.x, 1.0-uv.y);
#endif
    GBufferData GBuffer = DecodeGBuffer(uv);

    float2 pixelPos = uv * g_View.viewportSize; //v_TexCoord range [0,1]

    float3 worldPos = ScreenSpaceToWorldPosition(pixelPos, GBuffer.Depth);

      SurfaceGeometry geom;
      geom.position = worldPos;
      geom.normal = GBuffer.WorldNormal;
      geom.viewDir = normalize(g_View.cameraPos - worldPos); // Assuming eye is at (0,0,0)
#if USE_TBN
//      todo:: GBuffer.WorldTangent = half3(0); //TODO:: get Aniso flag
//      geom.tangent = normalize(v_Tangent);
//      geom.bitangent = normalize(cross(geom.normal, geom.tangent));
      getTBN(geom.normal, uv, geom.normal, geom.tangent, geom.bitangent);

#else
      getTBN(worldPos, uv, geom.normal, geom.tangent, geom.bitangent);
#endif


#if COOK
    MaterialSample mat = GetMaterialFromGBuffer(GBuffer);
#elif (Disney)
    DisneyMaterialSample mat = GetMaterialFromGBuffer(GBuffer);
#endif

#if USE_GLES_PLS
    mat.emissive = pls.t_gSceneColor.rgb;
#else
    mat.emissive = texture(t_gSceneColor,uv).rgb;

#endif
    vec4 sceneColor = vec4(0.0);
   //for(uint nLight = 0u; nLight < uint(g_DeferredLight.numLights); nLight++)
   {
       LightConstants light = g_DeferredLight.light;
#if USE_GLES_PLS
       sceneColor = ShadeSurface(light, geom, mat);
#else
       FragColor = ShadeSurface(light, geom, mat);
#endif
   }
   half IndirectIrradiance = GBuffer.IndirectIrradiance;
#if USE_GLES_PLS
//    pls.t_gGbufferA = vec4(0.0);
//    pls.t_gGbufferB = vec4(0.0);
//    pls.t_gGbufferC = vec4(0.0);

    pls.t_gSceneColor =sceneColor;
#endif



    //direct light

//    //ibl
//    if(v_TexCoord.x <0.5 && v_TexCoord.y < 0.5){
//     FragColor = vec4(1,0,0,1);
//    }else{
//        FragColor = texture(t_gGbufferA,v_TexCoord);//vec4(texture(t_gGbufferC,v_TexCoord).xyz,1.0);
//
//    }
}
