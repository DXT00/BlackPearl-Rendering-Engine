

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


// 1. 替换 HLSL 的 cbuffer 为 GLSL 的 uniform buffer
layout(std140, binding = 7) uniform MaterialConstants { // MATERIAL_CB_SLOT -> binding=7
    int flags; // 假设 MaterialConstants 包含 flags 字段
    // 其他材质常量...
} g_Material;

// 2. 替换 HLSL 的 Texture2D 和 SamplerState 为 GLSL 的组合采样器
layout(binding = 0) uniform sampler2D t_BaseOrDiffuse;    // MATERIAL_DIFFUSE_SLOT -> 0
layout(binding = 1) uniform sampler2D t_MetalRoughOrSpecular;
layout(binding = 2) uniform sampler2D t_Normal;
layout(binding = 3) uniform sampler2D t_Emissive;
layout(binding = 4) uniform sampler2D t_Occlusion;
layout(binding = 5) uniform sampler2D t_Transmission;

// 3. 定义材质采样结构体
struct MaterialTextureSample {
    vec4 baseOrDiffuse;
    vec4 metalRoughOrSpecular;
    vec4 emissive;
    vec4 normal;
    vec4 occlusion;
    vec4 transmission;
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

    if ((g_Material.flags & 0x1) != 0) { // MaterialFlags_UseBaseOrDiffuseTexture
        values.baseOrDiffuse = texture(t_BaseOrDiffuse, texCoord);
    }

    if ((g_Material.flags & 0x2) != 0) { // MaterialFlags_UseMetalRoughOrSpecularTexture
        values.metalRoughOrSpecular = texture(t_MetalRoughOrSpecular, texCoord);
    }

    if ((g_Material.flags & 0x4) != 0) { // MaterialFlags_UseEmissiveTexture
        values.emissive = texture(t_Emissive, texCoord);
    }

    if ((g_Material.flags & 0x8) != 0) { // MaterialFlags_UseNormalTexture
        values.normal = texture(t_Normal, texCoord);
    }

    if ((g_Material.flags & 0x10) != 0) { // MaterialFlags_UseOcclusionTexture
        values.occlusion = texture(t_Occlusion, texCoord);
    }

    if ((g_Material.flags & 0x20) != 0) { // MaterialFlags_UseTransmissionTexture
        values.transmission = texture(t_Transmission, texCoord);
    }

    return values;
}

// 6. 转换 SampleMaterialTexturesLevel 函数
MaterialTextureSample SampleMaterialTexturesLevel(vec2 texCoord, float lod) {
    MaterialTextureSample values = DefaultMaterialTextures();

    if ((g_Material.flags & 0x1) != 0) {
        values.baseOrDiffuse = textureLod(t_BaseOrDiffuse, texCoord, lod);
    }

    if ((g_Material.flags & 0x2) != 0) {
        values.metalRoughOrSpecular = textureLod(t_MetalRoughOrSpecular, texCoord, lod);
    }

    if ((g_Material.flags & 0x4) != 0) {
        values.emissive = textureLod(t_Emissive, texCoord, lod);
    }

    if ((g_Material.flags & 0x8) != 0) {
        values.normal = textureLod(t_Normal, texCoord, lod);
    }

    if ((g_Material.flags & 0x10) != 0) {
        values.occlusion = textureLod(t_Occlusion, texCoord, lod);
    }

    if ((g_Material.flags & 0x20) != 0) {
        values.transmission = textureLod(t_Transmission, texCoord, lod);
    }

    return values;
}

// 7. 转换 SampleMaterialTexturesGrad 函数
MaterialTextureSample SampleMaterialTexturesGrad(vec2 texCoord, vec2 ddx, vec2 ddy) {
    MaterialTextureSample values = DefaultMaterialTextures();

    if ((g_Material.flags & 0x1) != 0) {
        values.baseOrDiffuse = textureGrad(t_BaseOrDiffuse, texCoord, ddx, ddy);
    }

    if ((g_Material.flags & 0x2) != 0) {
        values.metalRoughOrSpecular = textureGrad(t_MetalRoughOrSpecular, texCoord, ddx, ddy);
    }

    if ((g_Material.flags & 0x4) != 0) {
        values.emissive = textureGrad(t_Emissive, texCoord, ddx, ddy);
    }

    if ((g_Material.flags & 0x8) != 0) {
        values.normal = textureGrad(t_Normal, texCoord, ddx, ddy);
    }

    if ((g_Material.flags & 0x10) != 0) {
        values.occlusion = textureGrad(t_Occlusion, texCoord, ddx, ddy);
    }

    if ((g_Material.flags & 0x20) != 0) {
        values.transmission = textureGrad(t_Transmission, texCoord, ddx, ddy);
    }

    return values;
}