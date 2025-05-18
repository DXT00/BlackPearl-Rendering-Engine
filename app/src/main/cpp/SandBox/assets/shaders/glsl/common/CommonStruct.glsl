
#ifdef GL_ES
precision mediump float;  // 必须声明精度（ES 要求）
#endif

#define float3 vec3
#define float4 vec4
#define float4x4 mat4


const int MaterialFlags_UseSpecularGlossModel            = 0x00000001;
const int MaterialFlags_DoubleSided                      = 0x00000002;
const int MaterialFlags_UseMetalRoughOrSpecularTexture   = 0x00000004;
const int MaterialFlags_UseBaseOrDiffuseTexture          = 0x00000008;
const int MaterialFlags_UseEmissiveTexture               = 0x00000010;
const int MaterialFlags_UseNormalTexture                 = 0x00000020;
const int MaterialFlags_UseOcclusionTexture              = 0x00000040;
const int MaterialFlags_UseTransmissionTexture           = 0x00000080;
const int MaterialFlags_ThinSurface                      = 0x00000100;
const int MaterialFlags_PSDExclude                       = 0x00000200;
const int MaterialFlags_UseHeightMapTexture              = 0x00000400;
const int MaterialFlags_UseCubeMapTexture                = 0x00000800;
const int MaterialFlags_UseDepthTexture                  = 0x00001000;
const int MaterialFlags_UseSpecularTexture               = 0x00002000;
const int MaterialFlags_UseRoughnessTexture              = 0x00004000;
const int MaterialFlags_UseMetalTexture                  = 0x00008000;
const int MaterialFlags_UseOpacityTexture                = 0x00010000;

struct Material{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 emissionColor;
	float roughnessValue;
	float mentallicValue;
	float aoValue;
	sampler2D diffuse; //or call it albedo
	sampler2D specular;
	sampler2D emission;
	sampler2D normal;
	sampler2D height;
	sampler2D depth;
	sampler2D ao;
	sampler2D roughness;
	sampler2D mentallic;	
	samplerCube cube;
	float shininess;
	float specularDiffusion;
	float diffuseReflectivity;
	float specularReflectivity;
	float transparency;
	bool  isBlinnLight;
	float emissivity;
//	int   isTextureSample;
//	int   isDiffuseTextureSample;
//	int   isSpecularTextureSample;
//	int   isMetallicTextureSample;
	float refractiveIndex;
};


struct Settings{
	bool  isBlinnLight;
	int   isAmbientTextureSample;
	int   isDiffuseTextureSample;
	int   isSpecularTextureSample;
	int   isHeightTextureSample;
	int   isEmissionTextureSample;
	int   isPBRTextureSample;//normalMap,aoMap,metallicMap
	bool  directLight;
	bool  indirectDiffuseLight;
	bool  indirectSpecularLight;
	bool  shadows;
	float GICoeffs;
	float SSRGICoeffs;
	bool  hdr;
	bool  guassian_horiziotal;
	bool  guassian_vertical;
	bool  guassian_mipmap;
	bool  showBlurArea;
};

struct Props {
    //Enable texture
    float shininess;
    float refractIndex;
    bool  isBinnLight;
    int  isPBRTextureSample;//�Ƿ�ʹ������-->���� ao,normal,metalllic,roughness
    int  isDiffuseTextureSample;
    int  isSpecularTextureSample;
    int  isHeightTextureSample;
    int  isEmissionTextureSample;
    int isRefractMaterial;
    int isDoubleSided;
};






// 4. 默认材质值函数
MaterialTextureSample DefaultMaterialTextures() {
    MaterialTextureSample s;
    s.baseOrDiffuse = vec4(1.0);
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

   // if ((g_Material.flags & MaterialFlags_UseBaseOrDiffuseTexture) != 0) { // MaterialFlags_UseBaseOrDiffuseTexture
        values.baseOrDiffuse = texture(t_BaseOrDiffuse, texCoord);
  //  }

    if ((g_Material.flags & MaterialFlags_UseMetalRoughOrSpecularTexture) != 0) { // MaterialFlags_UseMetalRoughOrSpecularTexture
        values.metalRoughOrSpecular = texture(t_MetalRoughOrSpecular, texCoord);
    }

    if ((g_Material.flags & MaterialFlags_UseEmissiveTexture) != 0) { // MaterialFlags_UseEmissiveTexture
        values.emissive = texture(t_Emissive, texCoord);
    }

    if ((g_Material.flags & MaterialFlags_UseNormalTexture) != 0) { // MaterialFlags_UseNormalTexture
        values.normal = texture(t_Normal, texCoord);
    }

    if ((g_Material.flags & MaterialFlags_UseOcclusionTexture) != 0) { // MaterialFlags_UseOcclusionTexture
        values.occlusion = texture(t_Occlusion, texCoord);
    }

    if ((g_Material.flags & MaterialFlags_UseTransmissionTexture) != 0) { // MaterialFlags_UseTransmissionTexture
        values.transmission = texture(t_Transmission, texCoord);
    }

    return values;
}

// 6. 转换 SampleMaterialTexturesLevel 函数
MaterialTextureSample SampleMaterialTexturesLevel(vec2 texCoord, float lod) {
    MaterialTextureSample values = DefaultMaterialTextures();

    if ((g_Material.flags & MaterialFlags_UseBaseOrDiffuseTexture) != 0) {
        values.baseOrDiffuse = textureLod(t_BaseOrDiffuse, texCoord, lod);
    }

    if ((g_Material.flags & MaterialFlags_UseMetalRoughOrSpecularTexture) != 0) {
        values.metalRoughOrSpecular = textureLod(t_MetalRoughOrSpecular, texCoord, lod);
    }

    if ((g_Material.flags & MaterialFlags_UseEmissiveTexture) != 0) {
        values.emissive = textureLod(t_Emissive, texCoord, lod);
    }

    if ((g_Material.flags & MaterialFlags_UseNormalTexture) != 0) {
        values.normal = textureLod(t_Normal, texCoord, lod);
    }

    if ((g_Material.flags & MaterialFlags_UseOcclusionTexture) != 0) {
        values.occlusion = textureLod(t_Occlusion, texCoord, lod);
    }

    if ((g_Material.flags & MaterialFlags_UseTransmissionTexture) != 0) {
        values.transmission = textureLod(t_Transmission, texCoord, lod);
    }

    return values;
}

// 7. 转换 SampleMaterialTexturesGrad 函数
MaterialTextureSample SampleMaterialTexturesGrad(vec2 texCoord, vec2 ddx, vec2 ddy) {
    MaterialTextureSample values = DefaultMaterialTextures();

    if ((g_Material.flags & MaterialFlags_UseBaseOrDiffuseTexture) != 0) {
        values.baseOrDiffuse = textureGrad(t_BaseOrDiffuse, texCoord, ddx, ddy);
    }

    if ((g_Material.flags & MaterialFlags_UseMetalRoughOrSpecularTexture) != 0) {
        values.metalRoughOrSpecular = textureGrad(t_MetalRoughOrSpecular, texCoord, ddx, ddy);
    }

    if ((g_Material.flags & MaterialFlags_UseEmissiveTexture) != 0) {
        values.emissive = textureGrad(t_Emissive, texCoord, ddx, ddy);
    }

    if ((g_Material.flags & MaterialFlags_UseNormalTexture) != 0) {
        values.normal = textureGrad(t_Normal, texCoord, ddx, ddy);
    }

    if ((g_Material.flags & MaterialFlags_UseOcclusionTexture) != 0) {
        values.occlusion = textureGrad(t_Occlusion, texCoord, ddx, ddy);
    }

    if ((g_Material.flags & MaterialFlags_UseTransmissionTexture) != 0) {
        values.transmission = textureGrad(t_Transmission, texCoord, ddx, ddy);
    }

    return values;
}

struct MaterialSample
{
    float3 shadingNormal;
    float3 geometryNormal;
    float3 diffuseAlbedo; // BRDF input Cdiff
    float3 specularF0; // BRDF input F0
    float3 emissiveColor;
    float opacity;
    float occlusion;
    float roughness;
    float3 baseColor; // native in metal-rough, derived in spec-gloss
    float metalness; // native in metal-rough, derived in spec-gloss
    float transmission;
    float diffuseTransmission;
    bool hasMetalRoughParams; // indicates that 'baseColor' and 'metalness' are valid
    float ior;
    float shadowNoLFadeout;
};

MaterialSample DefaultMaterialSample()
{
    MaterialSample result;
    result.shadingNormal = float3(0);
    result.geometryNormal = float3(0);
    result.diffuseAlbedo = float3(0);
    result.specularF0 = float3(0);
    result.emissiveColor = float3(0);
    result.opacity = 1.0;
    result.occlusion = 1.0;
    result.roughness = 0.0;
    result.baseColor = float3(0);
    result.metalness = 0.0;
    result.transmission = 0.0;
    result.diffuseTransmission = 0.0;
    result.hasMetalRoughParams = false;
    result.ior = 1.5;
    return result;
}
const float c_DielectricSpecular = 0.04;


MaterialSample EvaluateSceneMaterial(float3 normal, float4 tangent, MaterialConstants material, MaterialTextureSample textures)
{
    MaterialSample result = DefaultMaterialSample();
    result.geometryNormal = normalize(normal);
    result.shadingNormal = result.geometryNormal;
    
    if (material.flags & MaterialFlags_UseSpecularGlossModel)
    {
        float3 diffuseColor = material.baseOrDiffuseColor.rgb * textures.baseOrDiffuse.rgb;
        float3 specularColor = material.specularColor.rgb * textures.metalRoughOrSpecular.rgb;
        result.roughness = 1.0 - textures.metalRoughOrSpecular.a * (1.0 - material.roughness);

#if ENABLE_METAL_ROUGH_RECONSTRUCTION
        ConvertSpecularGlossToMetalRough(diffuseColor, specularColor, result.baseColor, result.metalness);
        result.hasMetalRoughParams = true;
#endif

        // Compute the BRDF inputs for the specular-gloss model
        // https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_materials_pbrSpecularGlossiness/README.md#specular---glossiness
        result.diffuseAlbedo = diffuseColor * (1.0 - max(specularColor.r, max(specularColor.g, specularColor.b)));
        result.specularF0 = specularColor;
    }
    else
    {
        result.baseColor = material.baseOrDiffuseColor.rgb * textures.baseOrDiffuse.rgb;
        result.roughness = material.roughness * textures.metalRoughOrSpecular.g;
        result.metalness = material.metalness * textures.metalRoughOrSpecular.b;
        result.hasMetalRoughParams = true;

        // Compute the BRDF inputs for the metal-rough model
        // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#metal-brdf-and-dielectric-brdf
        result.diffuseAlbedo = lerp(result.baseColor * (1.0 - c_DielectricSpecular), 0.0, result.metalness);
        result.specularF0 = lerp(c_DielectricSpecular, result.baseColor.rgb, result.metalness);
    }
    
    result.occlusion = 1.0;
    if (material.flags & MaterialFlags_UseOcclusionTexture)
    {
        result.occlusion = textures.occlusion.r;
    }

    result.occlusion = lerp(1.0, result.occlusion, material.occlusionStrength);
    
    result.opacity = material.opacity;
    if (material.flags & MaterialFlags_UseBaseOrDiffuseTexture)
        result.opacity *= textures.baseOrDiffuse.a;
    result.opacity = saturate(result.opacity);

    result.transmission = material.transmissionFactor;
    result.diffuseTransmission = material.diffuseTransmissionFactor;
    if (material.flags & MaterialFlags_UseTransmissionTexture)
    {
        result.transmission *= textures.transmission.r;
        result.diffuseTransmission *= textures.transmission.r;
    }
    
    result.emissiveColor = material.emissiveColor;
    if (material.flags & MaterialFlags_UseEmissiveTexture)
        result.emissiveColor *= textures.emissive.rgb;

    if (material.flags & MaterialFlags_UseNormalTexture)
        ApplyNormalMap(result, tangent, textures.normal, material.normalTextureScale);

    result.ior = material.ior;
    
    result.shadowNoLFadeout = material.shadowNoLFadeout;

    return result;
}
