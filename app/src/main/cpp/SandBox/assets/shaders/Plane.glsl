
#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;
uniform mat4 u_ProjectionView;

void main()
{
	v_TexCoord = aTexCoords;
	//v_Normal   = aNormal;
	v_Normal =  mat3(u_TranInverseModel)* aNormal;//vec3(u_Model * vec4(aNormal,1.0));
	v_FragPos = vec3(u_Model* vec4(aPos,1.0));


	gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);
}



#type fragment
#version 430 core

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


uniform struct Material{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 emissionColor;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	sampler2D normal;
	sampler2D height;

	float shininess;
	bool isBlinnLight;
	int  isTextureSample;//�ж��Ƿ�ʹ��texture,����ֻ��color
}u_Material;

out vec4 FragColor;

//uniform sampler2D texture1;
in vec2 v_TexCoord;
in vec3 v_Normal;		
in vec3 v_FragPos;	




uniform int u_PointLightNums;

//uniform Material u_Material;
uniform ParallelLight u_ParallelLight;
uniform PointLight u_PointLights[100];
uniform SpotLight u_SpotLight;

uniform vec3 u_CameraViewPos;
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir);
vec3 CalcParallelLight(ParallelLight light,vec3 normal,vec3 viewDir);
vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 viewDir);
void main(){

	//FragColor = texture(u_Material.diffuse,v_TexCoord);
		vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	vec3 outColor ;//=vec3(0.2,0.3,0.9);
	//outColor = CalcParallelLight(u_ParallelLight,v_Normal,viewDir);
	outColor += CalcSpotLight(u_SpotLight, v_Normal,viewDir);

	for(int i=0;i<u_PointLightNums;i++){

	outColor += CalcPointLight(u_PointLights[i], v_Normal,viewDir);

	}

	FragColor = vec4(outColor,1.0);
}



vec3 CalcParallelLight(ParallelLight light,vec3 normal,vec3 viewDir){
	vec3 fragColor;
	//ambient
	vec3 ambient = light.ambient *  u_Material.ambientColor;//texture(u_Material.diffuse,v_TexCoord).rgb;//u_LightColor * u_Material.ambient
	
	//diffuse
	vec3 lightDir = normalize(-light.direction);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * u_Material.diffuseColor;//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
	
	//specular
	vec3 reflectDir = normalize(reflect(-lightDir,norm));
	float spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
	vec3 specular =  light.specular * spec  *  u_Material.specularColor;//texture(u_Material.specular,v_TexCoord).rgb;


	fragColor = diffuse + ambient + specular;// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
	
	return fragColor;
}

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir){
	vec3 fragColor;

	float distance = length(light.position-v_FragPos);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	//ambient
	vec3 ambient = light.ambient * u_Material.ambientColor * (1-u_Material.isTextureSample)
					   + texture(u_Material.diffuse,v_TexCoord).rgb * u_Material.isTextureSample;
//	vec3 ambient = light.ambient * u_Material.ambientColor 
//					   *texture(u_Material.diffuse,v_TexCoord).rgb;
	
	//diffuse
	vec3 lightDir = normalize(light.position-v_FragPos);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff *  u_Material.diffuseColor *(1-u_Material.isTextureSample)
					+ texture(u_Material.diffuse,v_TexCoord).rgb*u_Material.isTextureSample;//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
//	vec3 diffuse = light.diffuse * diff *  u_Material.diffuseColor *texture(u_Material.diffuse,v_TexCoord).rgb;

//specular
	vec3 specular;
	float spec;
	if(u_Material.isBlinnLight){

		vec3 halfwayDir = normalize(lightDir+viewDir);
		spec = pow(max(dot(norm,halfwayDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  *  u_Material.specularColor;
	}
	else{

		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  *  u_Material.specularColor;//texture(u_Material.specular,v_TexCoord).rgb;
	}
	 vec3 emission = texture(u_Material.emission, v_TexCoord).rgb;
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	emission *= attenuation;

	fragColor = diffuse + ambient + specular+emission;// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
	
	return fragColor;
}

vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 viewDir){
	vec3 fragColor;

	vec3 lightDir = normalize(light.position - v_FragPos);
	float theta = dot(lightDir,normalize(-light.direction));
	float intensity = (theta-light.outerCutOff)/(light.cutOff-light.outerCutOff);
	intensity =clamp(intensity,0.0,1.0);
	

		

	float distance = length(light.position-v_FragPos);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	//ambient
	vec3 ambient = vec3(0.2f) * ( u_Material.ambientColor* (1-u_Material.isTextureSample)+texture(u_Material.diffuse,v_TexCoord).rgb * u_Material.isTextureSample);// texture(u_Material.diffuse,v_TexCoord).rgb;//u_LightColor * u_Material.ambient
	
	//diffuse
	
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff *  (u_Material.diffuseColor* (1-u_Material.isTextureSample)+texture(u_Material.diffuse,v_TexCoord).rgb*u_Material.isTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
	
	//specular
	vec3 reflectDir = normalize(reflect(-lightDir,norm));
	//vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	float spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
	vec3 specular = light.specular * spec  *  u_Material.specularColor;//texture(u_Material.specular,v_TexCoord).rgb;


	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	
	diffuse  *= intensity;
	specular *= intensity;
	//ambient *= intensity;
	fragColor = diffuse + ambient + specular;// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
		
		


		return fragColor;
}