

#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!
uniform bool u_ReverseNormals;

/*
	这里由于使用的是CubeMap,
	可以直接使用一个方向向量采样深度值
	所以，不需要把vertex坐标转换到光空间下了
*/

void main(){
	gl_Position = u_ProjectionView* u_Model * vec4(aPos, 1.0);
	v_FragPos = vec3(u_Model * vec4(aPos, 1.0));
	if(u_ReverseNormals)
		v_Normal = transpose(inverse(mat3(u_Model))) * (-1.0*aNormal);
	else
		v_Normal =  transpose(inverse(mat3(u_Model))) * aNormal;

	v_TexCoords = aTexCoords;
}


#type fragment
#version 330 core

out vec4 FragColor;


in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoords;

const float PI=3.14159265359;
//uniform struct Material{
//	vec3 ambientColor;
//	vec3 diffuseColor;
//	vec3 specularColor;
//	vec3 emissionColor;
//	sampler2D diffuse;
//	sampler2D specular;
//	sampler2D emission;
//	sampler2D normal;
//	sampler2D height;
//	sampler2D depth;
//	samplerCube cube;
//	sampler2D ao;
//	sampler2D roughness;
//	sampler2D mentallic;
//
//	float shininess;
//	bool isBlinnLight;
//	int  isTextureSample;//判断是否使用texture,或者只有color
//
//
//}u_Material;

struct PointLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	float constant;
	float linear;
	float quadratic;
	float intensity;

};
uniform int u_PointLightNums;
uniform Material u_Material;
uniform PointLight u_PointLights[20];
uniform samplerCube u_ShadowMap[20];
//uniform vec3 u_LightPos;
uniform vec3 u_CameraViewPos;
uniform float u_FarPlane ;
uniform int u_IsPBRObject;
//uniform samplerCube u_CubeMap;
float ShadowCalculation(vec3 fragPos,vec3 lightPos,samplerCube shadowMap);
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir);
vec3 getNormalFromMap()
{
    vec3 tangentNormal =  2.0* texture(u_Material.normal, v_TexCoords).xyz- vec3(1.0);

    vec3 Q1  = dFdx(v_FragPos);
    vec3 Q2  = dFdy(v_FragPos);
    vec2 st1 = dFdx(v_TexCoords);
    vec2 st2 = dFdy(v_TexCoords);

    vec3 N   = normalize(v_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
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
vec3 BRDF(vec3 Kd,vec3 Ks,vec3 specular,vec3 albedo){
	
	vec3 fLambert = albedo/PI;//diffuseColor 相当于 albedo
	return Kd * fLambert+  specular;//specular 中已经有Ks(Ks=F)了，不需要再乘以Ks *
}
vec3 LightRadiance(vec3 fragPos,PointLight light){
	float attenuation = calculateAttenuation(light,fragPos);
	//float cosTheta = max(dot(N,wi),0.0);
	vec3 radiance =light.intensity* light.diffuse*attenuation;
	return radiance;
}
vec3 CalcPBRPointLight(PointLight light,vec3 getNormalFromMap,vec3 albedo,float metallic,float roughness,vec3 fragPos){
	
	//tangent normal 
	vec3 N = getNormalFromMap;//getNormalFromMap(normal,normalMap,fragPos);
	vec3 V = normalize(u_CameraViewPos-fragPos);
	vec3 R = reflect(-V,N);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0,albedo,metallic);

	//reflection equation
	vec3 Lo = vec3(0.0);
	//for(int i=0;i<u_PointLightNums;i++){
		vec3 L = normalize(light.position-fragPos);
		vec3 H = normalize(V+L);
		float attenuation = calculateAttenuation(light,fragPos);
		vec3 radiance = light.diffuse * attenuation;

		float NDF = NoemalDistribution_TrowbridgeReitz_GGX(N,H,roughness);
		float G = GeometrySmith(N, V,L,roughness);
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
		Kd *= (1.0 - metallic);

		
		float NdotL = max(dot(N,L),0.0);
		Lo+= BRDF(Kd,Ks,specular,albedo)*LightRadiance(fragPos,light)*NdotL;
	//}
//
//	//HDR tonemapping
//	 Lo = Lo / (Lo + vec3(1.0));
//	//gamma correction
//    Lo = pow(Lo, vec3(1.0/2.2));  
	return Lo;
}



void main(){
	vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	vec3 outColor ;//=vec3(0.2,0.3,0.9);
	//outColor = CalcParallelLight(u_ParallelLight,v_Normal,viewDir);
	//outColor += CalcSpotLight(u_SpotLight, v_Normal,viewDir);
	vec3 albedo =pow(texture(u_Material.diffuse, v_TexCoords).rgb, vec3(2.2));//vec3(pow( texture(u_Material.diffuse, v_TexCoord).r, (2.2)));

	float mentallic = texture(u_Material.mentallic,v_TexCoords).r;
	float roughness  = texture(u_Material.roughness ,v_TexCoords).r;
	float ao        = texture(u_Material.ao, v_TexCoords).r;
	vec3 emission = texture(u_Material.emission,v_TexCoords).rgb;

	vec3 N = getNormalFromMap();
	for(int i=0;i<u_PointLightNums;i++){
	float shadow = ShadowCalculation(v_FragPos,u_PointLights[i].position,u_ShadowMap[i]); 

	if(u_IsPBRObject==0)
		outColor +=(1.0 - shadow) * CalcPointLight(u_PointLights[i],v_Normal,viewDir);// CalcPointLight(u_PointLight, normal,viewDir,material,fragPos);

	else
		outColor +=(1.0 - shadow) * CalcPBRPointLight(u_PointLights[i],N,albedo,mentallic, roughness, v_FragPos);// CalcPointLight(u_PointLight, normal,viewDir,material,fragPos);
		//outColor += CalcPointLight(u_PointLights[i], v_Normal,viewDir,u_ShadowMap[i]);

	}

	FragColor = vec4(emission* u_Material.isTextureSample+outColor,1.0);//vec4(1.0,0.0,0.0,1.0);//


}
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir){
	vec3 fragColor;

	float distance = length(light.position-v_FragPos);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	//ambient
	vec3 ambient = light.ambient*(  u_Material.ambientColor * (1-u_Material.isTextureSample)
					   + texture(u_Material.diffuse,v_TexCoords).rgb * u_Material.isTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;//u_LightColor * u_Material.ambient
	
	//diffuse
	vec3 lightDir = normalize(light.position-v_FragPos);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse =light.diffuse * diff * ( u_Material.diffuseColor *(1-u_Material.isTextureSample)
					+ texture(u_Material.diffuse,v_TexCoords).rgb*u_Material.isTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
	

//specular
	vec3 specular;
	float spec;
	if(u_Material.isBlinnLight){

		vec3 halfwayDir = normalize(lightDir+viewDir);
		spec = pow(max(dot(norm,halfwayDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  * ( u_Material.specularColor*(1-u_Material.isTextureSample)+texture(u_Material.specular,v_TexCoords).rgb*u_Material.isTextureSample);
	}
	else{

		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  * ( u_Material.specularColor*(1-u_Material.isTextureSample)+ texture(u_Material.specular,v_TexCoords).rgb*u_Material.isTextureSample);//texture(u_Material.specular,v_TexCoord).rgb;
	}

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	//float shadow = ShadowCalculation(v_FragPos,light.position,shadowMap);       


	fragColor = ambient+  light.intensity*(diffuse + specular);// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
		// ambient+ (1.0 - shadow) *(diffuse + specular);
		//fragColor = ambient+ (diffuse + specular);
	return fragColor;
}

float ShadowCalculation(vec3 fragPos,vec3 lightPos,samplerCube shadowMap){

		vec3 fragToLight = fragPos.xyz - lightPos; 

	 float closestDepth = texture(shadowMap, fragToLight).r;
	// It is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth = closestDepth*u_FarPlane;
    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // Now test for shadows
  
	float shadow = 0.0;
	float bias = 0.08; 
	float samples = 4.0;
	float offset = 0.1;
	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
	    for(float y = -offset; y < offset; y += offset / (samples * 0.5))
	    {
	        for(float z = -offset; z < offset; z += offset / (samples * 0.5))
	        {
	            float closestDepth = texture(shadowMap, fragToLight + vec3(x, y, z)).r; 
	            closestDepth *= u_FarPlane;   // Undo mapping [0;1]
	            if(currentDepth - bias > closestDepth)
	                shadow += 1.0;
	        }
	    }
	}
	shadow /= (samples * samples * samples);
	return shadow;

//vec3 fragToLight = fragPos.xyz - lightPos; 
//float SampledDistance = texture(u_CubeMap,fragToLight).r;
//
//    float Distance = length(fragToLight);
//
//    if (Distance < SampledDistance*u_FarPlane + bias)
//        return 0.3; // Inside the light
//    else
//        return 0.8; // Inside the shadow
}

