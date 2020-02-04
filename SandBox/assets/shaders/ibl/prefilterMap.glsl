#type vertex
#version 430 core

layout (location = 0) in vec3 aPos;


uniform mat4 u_ProjectionView;
uniform mat4 u_Model;

uniform mat4 u_CubeMapProjectionView;
out vec3 WorldPos;

void main(){

	WorldPos = aPos;
	//这里注意：渲染Skybox时不需要u_Model,LightProbes时需要
	gl_Position = u_ProjectionView*u_Model*vec4(WorldPos,1.0);
	//gl_Position = u_CubeMapProjectionView*u_Model*vec4(WorldPos,1.0);

}

#type fragment
#version 430 core

const float PI = 3.14159;
in vec3 WorldPos;
out vec4 FragColor;

uniform float u_roughness;
uniform samplerCube u_EnvironmentMap;
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
//http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
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
float NoemalDistribution_TrowbridgeReitz_GGX(vec3 N,vec3 H,float roughness){
	float roughness2 = roughness*roughness;
	float NHDOT = max(abs(dot(N,H)),0.0);
	float tmp= (NHDOT*NHDOT)*(roughness2-1.0)+1.0;
	float NDF = roughness2/(PI*tmp*tmp);
	return NDF;
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

void main(){

	vec3 N = normalize(WorldPos);
	//make the simplyfying assumption that view = reflection = normal direction
	vec3 R=N;
	vec3 V=R;

	//蒙特卡洛采样
	const uint SAMPLE_COUNT=1024u;
	vec3 prefilterColor = vec3(0.0);
	float totalWeight = 0.0;

	for(uint i=0u;i<SAMPLE_COUNT;i++){
		
		vec2 Xi = HammersleyNoBitOps(i,SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(Xi,N,u_roughness);
		//计算反射光线方向
		vec3 L = normalize(2.0* dot(V,H)*H-V);

		float NdotL = max(dot(N,L),0.0);

		if(NdotL>0.0){
			float D = NoemalDistribution_TrowbridgeReitz_GGX(N,H,u_roughness);
			float NdotH = max(dot(N,H),0.0);
			float HdotV = max(dot(N,V),0.0);
			float pdf = D*NdotH/(4.0*HdotV)+0.0001;

			float resolution = 512.0;// resolution of source cubemap (per face)
			float  saTexel = 4.0*PI/(6.0*resolution*resolution);
			float saSample= 1.0/(float(SAMPLE_COUNT)*pdf +0.0001);

			float mipLevel = (u_roughness == 0.0)? 0.0:0.5*log2(saSample / saTexel); 

			prefilterColor += textureLod(u_EnvironmentMap,L,mipLevel).rgb*NdotL;
			totalWeight+= NdotL;

		}

	}
	prefilterColor = prefilterColor/totalWeight;
	FragColor = vec4(prefilterColor,1.0);//vec4(1.0,0.0,0.0,1.0);//

}