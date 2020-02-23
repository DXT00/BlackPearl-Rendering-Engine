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
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gSpecular;
uniform vec2 gScreenSize;



//uniform sampler2D texture_diffuse1;

struct PointLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	float constant;
	float linear;
	float quadratic;

};
struct ParallelLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};
struct SpotLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 direction;
	vec3 position;
	float cutOff;
	float outerCutOff;

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

uniform bool u_HasSpotLight;
uniform bool u_HasParallelLight;
uniform int u_PointLightNums;

//uniform Material u_Material;
uniform ParallelLight u_ParallelLight;
uniform PointLight u_PointLights[100];
uniform PointLight u_PointLight;
uniform SpotLight u_SpotLight;
uniform vec3 u_CameraViewPos;

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir,gBufferMaterial material,vec3 fragPos);
//vec3 CalcParallelLight(ParallelLight light,vec3 normal,vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 viewDir);
vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}

out vec4 FragColor;
void main(){
	gBufferMaterial material;

	vec2 texCoords = CalcTexCoord();
	vec3 v_FragPos = texture(gPosition,texCoords).rgb;
	vec3 v_Normal = texture(gNormal,texCoords).rgb;
	material.diffuseColor = texture(gAlbedoSpec,texCoords).rgb;
	
	

	material.isBlinnLight = false;
	material.specularColor = texture(gSpecular,texCoords).rgb;

	vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	vec3 outColor =vec3(0.0,0.0,0.0);

	outColor = CalcPointLight(u_PointLight, v_Normal,viewDir,material,v_FragPos);

	FragColor =vec4(outColor,1.0);//vec4(1.0,0.0,0.0,1.0); //



	//	if(u_HasParallelLight)
//		outColor += CalcParallelLight(u_ParallelLight,v_Normal,viewDir);
//	if(u_HasSpotLight)
//		outColor += CalcSpotLight(u_SpotLight, v_Normal,viewDir);
//
//	for(int i=0;i<u_PointLightNums;i++){
//		outColor += CalcPointLight(u_PointLights[i], v_Normal,viewDir,material,v_FragPos);
//	}

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