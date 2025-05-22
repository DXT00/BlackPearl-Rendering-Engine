
#ifdef GL_ES
	precision mediump float;  // 必须声明精度（ES 要求）

	#define half mediump float	
	#define half2 mediump vec2	
	#define half3  mediump vec3	
	#define half4  mediump vec4	
	#define half3x3 mediump mat3	
	#define half3x4 mediump mat3x4	 
	#define half4x4 mediump mat4	

	#define float3 vec3  //默认16bit,中等精度
	#define float4 vec4
	#define float4x4 mat4
#else //PC opengl/vuljan

	#define half  float	
	#define half2  vec2	
	#define half3   vec3	
	#define half4   vec4	
	#define half3x3  mat3	
	#define half3x4  mat3x4	 
	#define half4x4  mat4	

	#define float3 vec3
	#define float4 vec4
	#define float4x4 mat4
#endif



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


static const int ShadingModel_Unlit = 0;
static const int ShadingModel_DefaultLit = 1;
static const int ShadingModel_Disney = 2;



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



void getTBN(in vec3 fragPos,in vec2 texCoord, in vec3 N, out vec3 T, out vec3 B)
{
    //vec3 tangentNormal =  2.0* texture(u_Material.normal, texCoord).xyz- vec3(1.0);
	//vec3 tangentNormal =  2.0* normal- vec3(1.0);

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(texCoord);
    vec2 st2 = dFdy(texCoord);

    N   = normalize(N);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    //mat3 TBN = mat3(T, B, N);

}

float Luminance( float3 linearColor )
{
	return dot( linearColor, float3( 0.3, 0.59, 0.11 ) );
}


// Geometry structure, in world coordinate
struct SurfaceGeometry {
    vec3 position;
    vec3 normal;
    vec3 viewDir;
    vec3 tangent;
    vec3 bitangent;
};



