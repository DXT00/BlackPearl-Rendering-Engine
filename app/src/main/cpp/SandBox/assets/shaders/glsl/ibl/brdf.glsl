#type vertex
#version 430 core
layout(location =0) in vec3 aPos;
layout(location =1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 v_TexCoord;

void main(){
	v_TexCoord = aTexCoords;
	gl_Position = vec4(aPos,1.0);

}
#type fragment
#version 430 core

in vec2 v_TexCoord;
out vec3 FragColor;
const float PI = 3.14159;
float VanDerCorpus(uint n, uint base)
{
    float invBase = 1.0 / float(base);
    float denom   = 1.0;
    float result  = 0.0;

    for(uint i = 0u; i < 32u; ++i)
    {
        if(n > 0u)
        {
            denom   = mod(float(n), 2.0);
            result += denom * invBase;
            invBase = invBase / 2.0;
            n       = uint(float(n) / 2.0);
        }
    }

    return result;
}
// ----------------------------------------------------------------------------
vec2 HammersleyNoBitOps(uint i, uint N)
{
    return vec2(float(i)/float(N), VanDerCorpus(i, 2u));
}

float RadicalInverse_VdC(uint bits){
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i,uint N){
	return vec2(float(i)/float(N),RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi,vec3 N,float roughness){
	float a = roughness*roughness;
	float phi = 2.0*PI*Xi.x;
	float cosTheta = sqrt((1.0-Xi.y)/(1.0+(a*a-1.0)*Xi.y));
	float sinTheta = sqrt(1.0-cosTheta*cosTheta);

	//sphere coordinate to cartesian coordinate
	vec3 H;
	H.x = sinTheta*cos(phi);
	H.y = sinTheta*sin(phi);
	H.z = cosTheta;

	//转换到fragment 的世界坐标系
	vec3 up = abs(N.z)<0.999?vec3(0.0,0.0,1.0):vec3(1.0,0.0,0.0);
	vec3 tangent = normalize(cross(up,N));
	vec3 bitangent = cross(N,tangent);

	vec3 sampleVec = tangent*H.x+bitangent*H.y+N*H.z;
	return normalize(sampleVec);
}
float Geometry_SchlickGGX(float NdotV,float roughness){
	//use k_IBL
	float k = (roughness*roughness)/2.0;
	return NdotV/(NdotV*(1.0-roughness)+roughness);
}

float GeometrySmith(vec3 N,vec3 V,vec3 L,float roughness){
	float NdotV = max(dot(N,V),0.0);
	float NdotL = max(dot(N,L),0.0);
	float ggx1 = Geometry_SchlickGGX(NdotV,roughness);
	float ggx2 = Geometry_SchlickGGX(NdotL,roughness);

	return ggx1*ggx2;
}

vec2 InterateBRDF(float NdotV,float roughness){
	
	vec3 V;
	V.x = sqrt(1.0-NdotV*NdotV);
	V.y = 0.0;
	V.z = NdotV;

	float A = 0.0;
	float B = 0.0;

	vec3 N = vec3(0.0,0.0,1.0);

	const uint SAMPLE_COUNT = 1024u;
	for(uint i=0u;i<SAMPLE_COUNT;++i){
		
		vec2 Xi= HammersleyNoBitOps(i,SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(Xi,N,roughness);

		vec3 L = normalize(2.0 * dot(V, H) * H - V);
		float NdotL = max(L.z,0.0);
		float NdotH = max(H.z,0.0);
		float VdotH = max(dot(V,H),0.0);

		if(NdotL>0.0){
			float G = GeometrySmith(N,V,L,roughness);
			float G_Vis = (G*VdotH)/(NdotH* NdotV);
			float Fc = pow(1.0-VdotH,5.0);
			A +=(1.0-Fc)*G_Vis;
			B += Fc*G_Vis;
		}
	}
	A/=float(SAMPLE_COUNT);
	B/=float(SAMPLE_COUNT);

	return vec2(A,B);



}
void main(){

	vec2 integratedBRDF = InterateBRDF(v_TexCoord.x,v_TexCoord.y);
	FragColor = vec3(integratedBRDF.x,integratedBRDF.y,0.0);
	//FragColor = vec2(integratedBRDF.x,integratedBRDF.y);
}


