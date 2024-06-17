#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->�����cpu�������ٴ�����!
uniform mat4 u_ProjectionView;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;
void main()
{
	gl_Position = u_ProjectionView * u_Model * vec4(aPos,1.0);
	v_FragPos = vec3(u_Model* vec4(aPos,1.0));
	v_TexCoord = vec2(aTexCoords.x,aTexCoords.y);
	v_Normal =  mat3(u_TranInverseModel)* aNormal;//vec3(u_Model * vec4(aNormal,1.0));

}

#type fragment
#version 430 core

const float PI=3.14159265359;
struct PointLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	float constant;
	float linear;
	float quadratic;

};
//uniform struct Material{
//	vec3 ambientColor;
//	vec3 diffuseColor;
//	vec3 specularColor;
//	vec3 emissionColor;
//	sampler2D diffuse; //or call it albedo
//	sampler2D specular;
//	sampler2D emission;
//	sampler2D normal;
//	sampler2D height;
//	sampler2D ao;
//	sampler2D roughness;
//	sampler2D mentallic;
//
//
//	float shininess;
//	bool isBlinnLight;
//	int  isTextureSample;//�ж��Ƿ�ʹ��texture,����ֻ��color
//
//}u_Material;
//
uniform Material u_Material;
in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoord;

out vec4 FragColor;
//Light
uniform PointLight u_PointLights[100];
uniform int u_PointLightNums;
//Camera
uniform vec3 u_CameraViewPos;
//Material parameters
uniform vec3 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_ao;
//IBL
uniform samplerCube u_IrradianceMap;
uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BrdfLUTMap;

int step = 100;
vec3 sum  = vec3(0.0);
float dw = 1.0/step;
float kd=0.1;
float ks=0.1;

float calculateAttenuation(PointLight light,vec3 fragPos){
	float distance = length(light.position-fragPos);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	return attenuation;
}

float NoemalDistribution_TrowbridgeReitz_GGX(vec3 N,vec3 H,float roughness){
	float roughness2 = roughness*roughness;
	float NHDOT = max(abs(dot(N,H)),0.0);
	float tmp= (NHDOT*NHDOT)*(roughness2-1.0)+1.0;
	float NDF = roughness2/(PI*tmp*tmp);
	return NDF;
}

float Geometry_SchlickGGX(float NdotV,float roughness){
	return NdotV/(NdotV*(1.0-roughness)+roughness);
}

float GeometrySmith(vec3 N,vec3 V,vec3 L,float roughness){
	float NdotV = max(dot(N,V),0.0);
	float NdotL = max(dot(N,L),0.0);
	float ggx1 = Geometry_SchlickGGX(NdotV,roughness);
	float ggx2 = Geometry_SchlickGGX(NdotL,roughness);

	return ggx1*ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0){
	return F0 + (1.0-F0)* pow(1.0 - cosTheta, 5.0);
}
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 BRDF(vec3 Kd,vec3 Ks,vec3 specular){
	
	vec3 fLambert = u_Material.diffuseColor/PI;//diffuseColor �൱�� albedo
	return Kd * fLambert+  specular;//specular ���Ѿ���Ks(Ks=F)�ˣ�����Ҫ�ٳ���Ks *
}
vec3 LightRadiance(vec3 fragPos,PointLight light){
	float attenuation = calculateAttenuation(light,fragPos);
	//float cosTheta = max(dot(N,wi),0.0);
	vec3 radiance = light.diffuse*attenuation;
	return radiance;
}


void main(){
	vec3 N = normalize(v_Normal);
	vec3 V = normalize(u_CameraViewPos-v_FragPos);
	vec3 R = reflect(-V,N);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0,u_Material.diffuseColor,u_metallic);

	//reflection equation
	vec3 Lo = vec3(0.0);
	for(int i=0;i<u_PointLightNums;i++){
		vec3 L = normalize(u_PointLights[i].position-v_FragPos);
		vec3 H = normalize(V+L);
		float attenuation = calculateAttenuation(u_PointLights[i],v_FragPos);
		vec3 radiance = u_PointLights[i].diffuse * attenuation;

		float NDF = NoemalDistribution_TrowbridgeReitz_GGX(N,H,u_roughness);
		float G = GeometrySmith(N, V,L,u_roughness);
		vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
		
		//CookTorrance
		vec3 nominator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
        vec3 specular     = nominator / denominator;
		
		// kS is equal to Fresnel
		vec3 Ks = F;
		// for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 Kd = vec3(1.0)-Ks;
		 // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
		Kd *= (1.0 - u_metallic);

		
		float NdotL = max(dot(N,L),0.0);
		Lo+= BRDF(Kd,Ks,specular)*LightRadiance(v_FragPos,u_PointLights[i])*NdotL;
	}

	//ambient lightings (we now use IBL as the ambient term)!
	vec3 F =  FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, u_roughness);
	vec3 Ks = F;
	vec3 Kd = vec3(1.0)-Ks;
	Kd *= (1.0 - u_metallic);
	vec3 environmentIrradiance = texture(u_IrradianceMap,N).rgb;
	vec3 diffuse = environmentIrradiance*u_Material.diffuseColor;

	//sample both the prefilter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part
	const float MAX_REFLECTION_LOD = 4.0;
	//sample MAX_REFLECTION_LOD level mipmap everytime !
	vec3 prefileredColor = textureLod(u_PrefilterMap,R,u_roughness*MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(u_BrdfLUTMap,vec2(max(dot(N,V),0.0),u_roughness)).rg;

	vec3 specular = prefileredColor * (F*brdf.x+brdf.y);
	vec3 ambient = (Kd*diffuse+specular) * u_ao;
    vec3 color =ambient + Lo;

	//HDR tonemapping
    color = color / (color + vec3(1.0));
	//gamma correction
    color = pow(color, vec3(1.0/2.2));  
    FragColor = vec4(color, 1.0);
}