
#ifndef BP_COMMON_H
#define BP_COMMON_H


#ifdef GL_ES
	precision mediump float;  // 必须声明精度（ES 要求）

	#define half mediump float	
	#define half2 mediump vec2	
	#define half3  mediump vec3	
	#define half4  mediump vec4	
	#define half3x3 mediump mat3	
	#define half3x4 mediump mat3x4	 
	#define half4x4 mediump mat4	

    #define float2 vec2
	#define float3 vec3  //默认16bit,中等精度
	#define float4 vec4
    #define float3x3 mat3
    #define float4x4 mat4

    #define int2 ivec2
	#define int3 ivec3
	#define int4 ivec4

    
    #define uint2 uvec2
	#define uint3 uvec3
	#define uint4 uvec4

    #define bool2 bvec2
	#define bool3 bvec3
	#define bool4 bvec4
#else //PC opengl/vuljan

	#define half  float	
	#define half2  vec2	
	#define half3   vec3	
	#define half4   vec4	
	#define half3x3  mat3	
	#define half3x4  mat3x4	 
	#define half4x4  mat4	

    #define float2 vec2
	#define float3 vec3
	#define float4 vec4
    #define float3x3 mat3
	#define float4x4 mat4


    #define int2 ivec2
	#define int3 ivec3
	#define int4 ivec4

    #define uint2 uvec2
	#define uint3 uvec3
	#define uint4 uvec4

    #define bool2 bvec2
	#define bool3 bvec3
	#define bool4 bvec4

#endif




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

#endif //BP_COMMON_H