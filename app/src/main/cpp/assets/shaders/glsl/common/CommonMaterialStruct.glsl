#ifndef BP_COMMON_MATERIAL_STRUCT_H
#define BP_COMMON_MATERIAL_STRUCT_H

#include <material_cb.h>

// Material Textures Sampler
layout(binding = 0) uniform sampler2D t_BaseOrDiffuse;    // MATERIAL_DIFFUSE_SLOT -> 0
layout(binding = 1) uniform sampler2D t_MetalRoughOrSpecular;
layout(binding = 2) uniform sampler2D t_Normal;
layout(binding = 3) uniform sampler2D t_Emissive;
layout(binding = 4) uniform sampler2D t_Occlusion;
layout(binding = 5) uniform sampler2D t_Transmission;


#if COOK
layout(std140, binding = 7) uniform MaterialUBO  { // MATERIAL_CB_SLOT -> binding=7
   MaterialConstants g_Mat;
};
#elif (Disney)
layout(std140, binding = 7) uniform MaterialUBO { // MATERIAL_CB_SLOT -> binding=7
  DisneyMaterialConstant g_Mat;
};
#endif


struct MaterialTextureSample
{
    float4 albedo;
    float4 metalRoughOrSpecular;
    float4 normal;
    float4 emissive;
    float4 occlusion;
    float4 transmission;
};


// 4. 默认材质值函数
MaterialTextureSample DefaultMaterialTextures() {
    MaterialTextureSample s;
    s.albedo = vec4(1.0);
    s.metalRoughOrSpecular = vec4(0.0);
    s.emissive = vec4(0.0);
    s.normal = vec4(0.5, 0.5, 1.0, 1.0); // 默认法线 (0,0,1)
    s.occlusion = vec4(1.0);
    s.transmission = vec4(0.0);
    return s;
}


// 5. 转换 SampleMaterialTexturesAuto 函数
MaterialTextureSample SampleMaterialTexturesAuto(vec2 texCoord) {
    MaterialTextureSample values = DefaultMaterialTextures();

   // if ((g_Mat.flags & MaterialFlags_UseBaseOrDiffuseTexture) != 0) { // MaterialFlags_UseBaseOrDiffuseTexture
        values.albedo = texture(t_BaseOrDiffuse, texCoord);
  //  }

    if ((g_Mat.flags & MaterialFlags_UseMetalRoughOrSpecularTexture) != 0) { // MaterialFlags_UseMetalRoughOrSpecularTexture
        values.metalRoughOrSpecular = texture(t_MetalRoughOrSpecular, texCoord);
    }

    if ((g_Mat.flags & MaterialFlags_UseEmissiveTexture) != 0) { // MaterialFlags_UseEmissiveTexture
        values.emissive = texture(t_Emissive, texCoord);
    }

    if ((g_Mat.flags & MaterialFlags_UseNormalTexture) != 0) { // MaterialFlags_UseNormalTexture
        values.normal = texture(t_Normal, texCoord);
    }

    if ((g_Mat.flags & MaterialFlags_UseOcclusionTexture) != 0) { // MaterialFlags_UseOcclusionTexture
        values.occlusion = texture(t_Occlusion, texCoord);
    }

    if ((g_Mat.flags & MaterialFlags_UseTransmissionTexture) != 0) { // MaterialFlags_UseTransmissionTexture
        values.transmission = texture(t_Transmission, texCoord);
    }

    return values;
}

// 6. 转换 SampleMaterialTexturesLevel 函数
MaterialTextureSample SampleMaterialTexturesLevel(vec2 texCoord, float lod) {
    MaterialTextureSample values = DefaultMaterialTextures();

    if ((g_Mat.flags & MaterialFlags_UseBaseOrDiffuseTexture) != 0) {
        values.albedo = textureLod(t_BaseOrDiffuse, texCoord, lod);
    }

    if ((g_Mat.flags & MaterialFlags_UseMetalRoughOrSpecularTexture) != 0) {
        values.metalRoughOrSpecular = textureLod(t_MetalRoughOrSpecular, texCoord, lod);
    }

    if ((g_Mat.flags & MaterialFlags_UseEmissiveTexture) != 0) {
        values.emissive = textureLod(t_Emissive, texCoord, lod);
    }

    if ((g_Mat.flags & MaterialFlags_UseNormalTexture) != 0) {
        values.normal = textureLod(t_Normal, texCoord, lod);
    }

    if ((g_Mat.flags & MaterialFlags_UseOcclusionTexture) != 0) {
        values.occlusion = textureLod(t_Occlusion, texCoord, lod);
    }

    if ((g_Mat.flags & MaterialFlags_UseTransmissionTexture) != 0) {
        values.transmission = textureLod(t_Transmission, texCoord, lod);
    }

    return values;
}

// 7. 转换 SampleMaterialTexturesGrad 函数
MaterialTextureSample SampleMaterialTexturesGrad(vec2 texCoord, vec2 ddx, vec2 ddy) {
    MaterialTextureSample values = DefaultMaterialTextures();

    if ((g_Mat.flags & MaterialFlags_UseBaseOrDiffuseTexture) != 0) {
        values.albedo = textureGrad(t_BaseOrDiffuse, texCoord, ddx, ddy);
    }

    if ((g_Mat.flags & MaterialFlags_UseMetalRoughOrSpecularTexture) != 0) {
        values.metalRoughOrSpecular = textureGrad(t_MetalRoughOrSpecular, texCoord, ddx, ddy);
    }

    if ((g_Mat.flags & MaterialFlags_UseEmissiveTexture) != 0) {
        values.emissive = textureGrad(t_Emissive, texCoord, ddx, ddy);
    }

    if ((g_Mat.flags & MaterialFlags_UseNormalTexture) != 0) {
        values.normal = textureGrad(t_Normal, texCoord, ddx, ddy);
    }

    if ((g_Mat.flags & MaterialFlags_UseOcclusionTexture) != 0) {
        values.occlusion = textureGrad(t_Occlusion, texCoord, ddx, ddy);
    }

    if ((g_Mat.flags & MaterialFlags_UseTransmissionTexture) != 0) {
        values.transmission = textureGrad(t_Transmission, texCoord, ddx, ddy);
    }

    return values;
}

MaterialSample CreateCookBSDFMaterial(in SurfaceGeometry geom, in MaterialConstants consts, vec2 texcoord)
{

	MaterialSample result;
    MaterialTextureSample textures = SampleMaterialTexturesAuto(texcoord);
    /*
      SurfaceGeometry geom;
      geom.position = vPosition;
      geom.normal = normalize(vNormal);
      geom.viewDir = normalize(-vPosition); // Assuming eye is at (0,0,0)
      geom.tangent = normalize(vTangent);
      geom.bitangent = normalize(cross(geom.normal, geom.tangent));
    
    */

    vec3 nn = normalize(geom.normal);
    vec3 nt = normalize(geom.tangent);
    mat3x3 tbn = mat3x3(nt, cross(nn, nt), nn);

	// normal map
#if USE_NORMAL_MAP
    // tbn basis
    result.shadingNormal = tbn * textures.normal * 2.0 - 1.0);
#else
    result.shadingNormal = nn;
#endif

    // albedo/specular base
#if USE_ALBEDO_MAP
    result.albedo = textures.albedo.xyz;
#else
    result.albedo = consts.albedo.xyz;
#endif

    // roughness
#if USE_ROUGHNESS_MAP
    result.roughness = textures.metalRoughOrSpecular.y;
    result.metallic  =  textures.metalRoughOrSpecular.z;
#else
    result.roughness = consts.roughness;
    result.metallic  = consts.metallic;
#endif

#if USE_EMISSIVE_MAP
    result.emissive = textures.emissive.xyz;
#else
    result.emissive = consts.emissive;
#endif

#if USE_TRANSMISSION_MAP
    result.transmission = textures.transmission.xyz;
#else
    result.transmission = consts.transmission;
#endif

#if USE_AO_MAP
   result.ao = textures.occlusion.y;
#else
   result.ao = consts.ao;
#endif
    result.specular = consts.specular;
    result.ior = consts.ior;
    result.flags = consts.flags;
    result.materialID = consts.materialID;
    result.shadingModelID = consts.shadingModelID;

    result.domain = consts.domain;
    result.opacity = consts.opacity;
    result.alphaThreshold = consts.alphaThreshold;

    return result;
}


DisneyMaterialSample CreateDisneyBSDFMaterial(SurfaceGeometry geom, DisneyMaterialConstant consts, vec2 texcoord){
	
    DisneyMaterialSample result;
    MaterialTextureSample textures = SampleMaterialTexturesAuto(texcoord);
    /*
      SurfaceGeometry geom;
      geom.position = vPosition;
      geom.normal = normalize(vNormal);
      geom.viewDir = normalize(-vPosition); // Assuming eye is at (0,0,0)
      geom.tangent = normalize(vTangent);
      geom.bitangent = normalize(cross(geom.normal, geom.tangent));
    
    */

    vec3 nn = normalize(geom.normal);
    vec3 nt = normalize(geom.tangent);
    mat3x3 tbn = mat3x3(nt, cross(nn, nt), nn);

	// normal map
#if USE_NORMAL_MAP
    // tbn basis
    result.shadingNormal = tbn * textures.normal * 2.0 - 1.0);
#else
   result.shadingNormal = nn;
#endif

    // albedo/specular base
#if USE_ALBEDO_MAP
    result.baseColor = textures.albedo.xyz;
#else
    result.baseColor = consts.baseColor.xyz;
#endif

    // roughness
#if USE_ROUGHNESS_MAP
    result.roughness = textures.metalRoughOrSpecular.y;
    result.metallic =  textures.metalRoughOrSpecular.z;
#else
    result.roughness = consts.roughness;
    result.metallic = consts.metallic;
#endif

#if USE_EMISSIVE_MAP
    result.emissive = textures.emissive.xyz;
#else
    result.emissive = consts.emissive;
#endif

//#if USE_TRANSMISSION_MAP
//    result.transmission = textures.transmission.xyz;
//#else
//    result.transmission = consts.transmission;
//#endif

//#if USE_AO_MAP
//   result.ao = textures.occlusion.y;
//#else
//   result.ao = consts.ao;
//#endif
    result.flags = consts.flags;
    result.materialID = consts.materialID;
    
    result.subsurface = consts.subsurface;
    result.subsurfaceRadius = consts.subsurfaceRadius;
    result.subsurfaceColor = consts.subsurfaceColor;
    result.specular = consts.specular;
    result.specularTint = consts.specularTint;
    result.anisotropic = consts.anisotropic;
    result.anisotropicRotation = consts.anisotropicRotation;
    result.sheen = consts.sheen;
    result.sheenTint = consts.sheenTint;
    result.clearcoat = consts.clearcoat;
    result.clearcoatRoughness = consts.clearcoatRoughness;
    result.ior = consts.ior;
    result.transmission = consts.transmission;
    result.transmissionRoughness = consts.transmissionRoughness;
    result.alpha = consts.alpha;


    return result;


}
#endif //BP_COMMON_MATERIAL_STRUCT_H