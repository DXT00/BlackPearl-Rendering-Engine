
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



// Geometry structure, in world coordinate
struct SurfaceGeometry {
    vec3 position;
    vec3 normal;
    vec3 viewDir;
    vec3 tangent;
    vec3 bitangent;
};



