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

    GBufferData GBuffer = DecodeGBuffer(v_TexCoord);

    float2 pixelPos = v_TexCoord * g_View.viewportSize; //v_TexCoord range [0,1]
    
    float3 worldPos = ScreenSpaceToWorldPosition(pixelPos, GBuffer.Depth);

      SurfaceGeometry geom;
      geom.position = worldPos;
      geom.normal = GBuffer.WorldNormal;
      geom.viewDir = normalize(g_View.cameraPos - worldPos); // Assuming eye is at (0,0,0)

      getTBN(worldPos, v_TexCoord, geom.normal, geom.tangent, geom.bitangent);




    MaterialSample mat = GetMaterialFromGBuffer(GBuffer);


    mat.emissive = pls.t_gSceneColor.rgb;

    vec4 sceneColor = vec4(0.0);
   //for(uint nLight = 0u; nLight < uint(g_DeferredLight.numLights); nLight++)
   {
       LightConstants light = g_DeferredLight.lights[0];
       sceneColor += ShadeSurface(light, geom, mat);

   }
   half IndirectIrradiance = GBuffer.IndirectIrradiance;


    pls.t_gSceneColor = sceneColor;

}
