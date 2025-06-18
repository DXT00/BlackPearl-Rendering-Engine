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



vec3 SHDiffuse(const int probeIndex,const vec3 normal){
	float x = normal.x;
	float y = normal.y;
	float z = normal.z;
    LightProbeConstants probe = g_DeferredLight.lightProbes[probeIndex];

	vec3 result = (
		probe.SHCoeffs[0] +
		
		probe.SHCoeffs[1] * x +
		probe.SHCoeffs[2] * y +
		probe.SHCoeffs[3] * z +
		
		probe.SHCoeffs[4] * z * x +
		probe.SHCoeffs[5] * y * z +
		probe.SHCoeffs[6] * y * x +
		probe.SHCoeffs[7] * (3.0 * z * z - 1.0) +
		probe.SHCoeffs[8] * (x*x - y*y)
  );

  return max(result, vec3(0.0));
}
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 CalculateAmbientGI(vec3 worldPos, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, float ao){

//	vec3 albedo = pow(texture(u_Material.diffuse,v_TexCoord).rgb,vec3(2.2));
//	float metallic = texture(u_Material.mentallic, v_TexCoord).r;
//    float roughness = texture(u_Material.roughness, v_TexCoord).r;
//    float ao = texture(u_Material.ao, v_TexCoord).r;
	//vec3 normal = texture(u_Material.normal,v_TexCoord).xyz;
	//normal = normalize(normal);
//	vec3 N = getNormalFromMap(v_FragPos,v_TexCoord);
//
//	vec3 V = normalize(u_CameraViewPos-v_FragPos);
	vec3 R = reflect(-V,N);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0,albedo,metallic);

	//ambient lightings (we now use IBL as the ambient term)!
	vec3 F =  FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 Ks = F;
	vec3 Kd = vec3(1.0)-Ks;
	Kd = Kd * (1.0 - metallic);
	vec3 environmentIrradiance = vec3(0.0);//= vec3(1.0,1.0,1.0);
	uint kProbe = g_DeferredLight.numLightProbes;
    float totalWeight = 0;
    for(uint i=0u; i< kProbe; i++){
        float d = length(g_DeferredLight.lightProbes[i].pos - worldPos);
        totalWeight += 1.0/d*d;
    }

	for(int i=0;i< kProbe;i++){
        float d = length(g_DeferredLight.lightProbes[i].pos - worldPos);
        float w = 1.0/d*d;
        w = w/totalWeight;
        w = max(0.001,w);
		environmentIrradiance += w * SHDiffuse(i,N);// u_ProbeWeight[i]*texture(u_IrradianceMap[i],N).rgb;

	}
	vec3 diffuse = environmentIrradiance*albedo;
//
//	//sample both the prefilter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part
//	const float MAX_REFLECTION_LOD = 4.0;//1.0;
//	//sample MAX_REFLECTION_LOD level mipmap everytime !
//	/*specular Map只取最近的一个*/
//	vec3 prefileredColor = textureLod(u_PrefilterMap,R,roughness*MAX_REFLECTION_LOD).rgb;//= vec3(1.0,1.0,1.0);
//
//
////	for(int i=0;i<u_Kprobes;i++){
////		prefileredColor+= u_ProbeWeight[i]*textureLod(u_PrefilterMap[i],R,roughness*MAX_REFLECTION_LOD).rgb;
////	//	prefileredColor*= textureLod(u_PrefilterMap[i],R,roughness*MAX_REFLECTION_LOD).rgb;
////
////	}
//	vec2 brdf = texture(u_BrdfLUTMap,vec2(max(dot(N,V),0.0),roughness)).rg;
//
//	vec3 specular = prefileredColor * (F*brdf.x+brdf.y);
//	vec3 ambient =  (Kd*diffuse+specular) * ao;
//
////	 ambient = ambient / (ambient + vec3(1.0));
////	//gamma correction
////    ambient = pow(ambient, vec3(1.0/2.2));  
//	return ambient;


	diffuse = diffuse / (diffuse + vec3(1.0));
	diffuse = pow(diffuse, vec3(1.0/2.2)); 
    return diffuse;

}






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


    vec3 IBL = CalculateAmbientGI(worldPos, geom.normal, geom.viewDir, mat.albedo, mat.metallic, mat.roughness, mat.ao);




#if USE_GLES_PLS
       pls.t_gSceneColor = vec4( mat.emissive + IBL,1.0);
 #else
       FragColor =  vec4( mat.emissive + IBL,1.0);
#endif
   
   //half IndirectIrradiance = GBuffer.IndirectIrradiance;
    //direct light

//    //ibl
//    if(v_TexCoord.x <0.5 && v_TexCoord.y < 0.5){
//     FragColor = vec4(1,0,0,1);
//    }else{
//        FragColor = texture(t_gGbufferA,v_TexCoord);//vec4(texture(t_gGbufferC,v_TexCoord).xyz,1.0);
//
//    }



}
