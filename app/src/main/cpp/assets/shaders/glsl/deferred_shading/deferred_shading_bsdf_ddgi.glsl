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

#include <common/CommonMath.glsl>
#include <common/CommonViewStruct.glsl>
#include <common/CommonDeferredStruct.glsl>
#include <common/CommonTransform.glsl>
#include <bsdf/BSDF.glsl>
#include <gBuffer/gBuffer.glsl>


layout(binding = 5) uniform sampler2D uIndirectLight;



// only diffuse
vec3 CalculateDDGI(vec2 uv, vec3 albedo, float metallic, float roughness){

	    vec3 indirectShading = vec3(0);
	    vec3 specular = vec3(0);

		indirectShading  = texture(uIndirectLight, uv).rgb;
		vec3 diffuseBRDF = (albedo - albedo * metallic) / PI;
		indirectShading = diffuseBRDF * indirectShading;

//		vec4 reflection = textureLod(uReflection, fragTexCoord, 0.0f);
//		vec3 specularIrradiance = reflection.rgb;//* reflection.a;
//		vec2 specularBRDF = texture(uPreintegratedFG, vec2(material.normalDotView, material.roughness)).rg;
//		specular = specularIrradiance * (F0 * specularBRDF.x + specularBRDF.y);//todo...
//
//		float roughnessSq = material.roughness * material.roughness;
//		float specularOcclusion = getSpecularOcclusion(material.normalDotView, roughnessSq, material.ao);
//		specular *= specularOcclusion;
	
	return indirectShading + specular;
}







void main(){
    vec2 uv = v_TexCoord;
#if USE_GLES_PLS
    uv = vec2(uv.x, 1.0-uv.y);
#endif
    GBufferData GBuffer = DecodeGBuffer(uv);

    float2 pixelPos = uv * g_View.viewportSize; //v_TexCoord range [0,1]

    //float3 worldPos = ScreenSpaceToWorldPosition(pixelPos, GBuffer.Depth);
    float3 worldPos = worldPositionFromDepth(uv, GBuffer.Depth, inverse(g_View.matProjectionView));

      SurfaceGeometry geom;
      geom.position = worldPos;
      geom.normal = normalize(GBuffer.WorldNormal);
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

    vec3 DDGI = CalculateDDGI(uv, mat.albedo, mat.metallic, mat.roughness);



#if USE_GLES_PLS
       pls.t_gSceneColor = vec4( mat.emissive + DDGI,1.0);
 #else
       FragColor =  vec4(mat.emissive + DDGI,1.0);//
#endif



}
