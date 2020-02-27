#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
out vec2 v_TexCoords;


void main()
{
	v_TexCoords = aTexCoords;

	gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);

}


#type fragment
#version 430 core

in vec2 v_TexCoords;

const float PI=3.14159265359;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse_Roughness;
uniform sampler2D gSpecular_Mentallic;
uniform sampler2D gAmbientGI_AO;
uniform sampler2D gNormalMap;

uniform vec2 gScreenSize;

struct PointLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	float constant;
	float linear;
	float quadratic;

};
struct gBufferMaterial{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 emissionColor;

	float shininess;
	bool isBlinnLight;
	int  isTextureSample;//判断是否使用texture,或者只有color

};

//uniform Material u_Material;
//TODO::法线贴图
//vec3 getNormalFromMap(vec3 normal,vec3 normalMap,vec3 fragPos)
//{
//    //vec3 tangentNormal =  2.0* texture(u_Material.normal, v_TexCoord).xyz- vec3(1.0);
//	vec3 tangentNormal =  2.0* normalMap- vec3(1.0);
//
//    vec3 Q1  = dFdx(fragPos);
//    vec3 Q2  = dFdy(fragPos);
//    vec2 st1 = dFdx(texCoord);
//    vec2 st2 = dFdy(texCoord);
//
//    vec3 N   = normalize(normal);
//    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
//    vec3 B  = -normalize(cross(N, T));
//    mat3 TBN = mat3(T, B, N);
//
//    return normalize(TBN * tangentNormal);
//}

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
	vec3 radiance = light.diffuse*attenuation;
	return radiance;
}
uniform PointLight u_PointLight;
uniform vec3 u_CameraViewPos;

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir,gBufferMaterial material,vec3 fragPos);

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

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}


out vec4 FragColor;
uniform samplerCube u_ShadowMap;
uniform float u_FarPlane ;

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


void main(){
	gBufferMaterial material;

	vec2 texCoords = CalcTexCoord();
	vec3 fragPos = texture(gPosition,texCoords).rgb;
	vec3 normal = texture(gNormal,texCoords).xyz;
	vec3 getNormalFromMap = texture(gNormalMap,texCoords).xyz;

	material.diffuseColor = texture(gDiffuse_Roughness,texCoords).rgb;
	material.isBlinnLight = false;
	material.specularColor = texture(gSpecular_Mentallic,texCoords).rgb;



	vec3 albedo = material.diffuseColor;
	float roughness = texture(gDiffuse_Roughness,texCoords).a;
	float metallic = texture(gSpecular_Mentallic,texCoords).a;
	float isPBRObject = texture(gAmbientGI_AO,texCoords).a;

	

	vec3 viewDir = normalize(u_CameraViewPos- fragPos);
	vec3 outColor =vec3(0.0,0.0,0.0);

	float shadow = ShadowCalculation(fragPos,u_PointLight.position,u_ShadowMap); 

	if(isPBRObject==0.0)
		outColor =(1.0 - shadow) * CalcPointLight(u_PointLight,normal,viewDir, material,fragPos);// CalcPointLight(u_PointLight, normal,viewDir,material,fragPos);

	else
		outColor =(1.0 - shadow) * CalcPBRPointLight(u_PointLight,getNormalFromMap,albedo,metallic, roughness, fragPos);// CalcPointLight(u_PointLight, normal,viewDir,material,fragPos);

	FragColor =vec4(outColor,1.0);//vec4(1.0,0.0,0.0,1.0); //


}


//vec3 CalcParallelLight(ParallelLight light,vec3 normal,vec3 viewDir,gBufferMaterial material){
//	vec3 fragColor;
//	//ambient
//	vec3 ambient = light.ambient *  (material.ambientColor * (1-material.isTextureSample)+ texture(material.diffuse,v_TexCoords).rgb * material.isTextureSample);//texture(material.diffuse,v_TexCoords).rgb;//u_LightColor * material.ambient
//	
//	//diffuse
//	vec3 lightDir = normalize(-light.direction);
//	vec3 norm = normalize(normal);
//	float diff = max(dot(lightDir,norm),0.0f);
//	vec3 diffuse = light.diffuse * diff *( material.diffuseColor *(1-material.isTextureSample)
//					+ texture(material.diffuse,v_TexCoords).rgb*material.isTextureSample);//texture(material.diffuse,v_TexCoords).rgb;// material.diffuse);u_LightColor
//	
//	//specular
//	vec3 reflectDir = normalize(reflect(-lightDir,norm));
//	float spec = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);
//	vec3 specular =  light.specular * spec  *  material.specularColor;//texture(material.specular,v_TexCoords).rgb;
//
//
//	fragColor = diffuse + ambient + specular;// * mix(texture(u_Texture1, v_TexCoords), texture(u_Texture2, vec2(1.0 - v_TexCoords.x, v_TexCoords.y)), u_MixValue);
//	
//	return fragColor;
//}

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir,gBufferMaterial material,vec3 fragPos){
	vec3 fragColor;

	float distance = length(light.position-fragPos);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	/*ambient*/
//	vec3 ambient = light.ambient * (material.ambientColor * (1-material.isTextureSample)+ texture(material.diffuse,v_TexCoords).rgb * material.isTextureSample);
	
	/*diffuse*/
	vec3 lightDir = normalize(light.position-fragPos);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * (material.diffuseColor);

	/*specular*/
	vec3 specular;
	float spec;
	if(material.isBlinnLight){

		vec3 halfwayDir = normalize(lightDir+viewDir);
		spec = pow(max(dot(norm,halfwayDir),0.0),material.shininess);
		specular =  light.specular * spec  *  material.specularColor;
	}
	else{

		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);
		specular =  light.specular * spec  *  material.specularColor;//texture(material.specular,v_TexCoords).rgb;
	}
//	 vec3 emission = texture(material.emission, v_TexCoords).rgb;


	//ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	//emission *= attenuation;

	fragColor = diffuse + specular;//+ ambient +emission;// * mix(texture(u_Texture1, v_TexCoords), texture(u_Texture2, vec2(1.0 - v_TexCoords.x, v_TexCoords.y)), u_MixValue);
	
	return fragColor;
}

//vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 viewDir,Material material,vec3 fragPos){
//	vec3 fragColor;
//
//	vec3 lightDir = normalize(light.position - fragPos);
//	float theta = dot(lightDir,normalize(-light.direction));
//	float intensity = (theta-light.outerCutOff)/(light.cutOff-light.outerCutOff);
//	intensity =clamp(intensity,0.0,1.0);
//	
//
//		
//
//	float distance = length(light.position-fragPos);
//	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
//	//ambient
//	vec3 ambient = vec3(0.2f) * ( material.ambientColor* (1-material.isTextureSample)+texture(material.diffuse,v_TexCoords).rgb * material.isTextureSample);// texture(material.diffuse,v_TexCoords).rgb;//u_LightColor * material.ambient
//	
//	//diffuse
//	
//	vec3 norm = normalize(normal);
//	float diff = max(dot(lightDir,norm),0.0f);
//	vec3 diffuse = light.diffuse * diff *  (material.diffuseColor* (1-material.isTextureSample)+texture(material.diffuse,v_TexCoords).rgb*material.isTextureSample);//texture(material.diffuse,v_TexCoords).rgb;// material.diffuse);u_LightColor
//	
//	//specular
//	vec3 reflectDir = normalize(reflect(-lightDir,norm));
//	//vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
//	float spec = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);
//	vec3 specular = light.specular * spec  *  material.specularColor;//texture(material.specular,v_TexCoords).rgb;
//
//
//	ambient  *= attenuation;
//	diffuse  *= attenuation;
//	specular *= attenuation;
//
//	
//	diffuse  *= intensity;
//	specular *= intensity;
//	//ambient *= intensity;
//	fragColor = diffuse + ambient + specular;// * mix(texture(u_Texture1, v_TexCoords), texture(u_Texture2, vec2(1.0 - v_TexCoords.x, v_TexCoords.y)), u_MixValue);
//		
//		
//
//
//		return fragColor;
//}