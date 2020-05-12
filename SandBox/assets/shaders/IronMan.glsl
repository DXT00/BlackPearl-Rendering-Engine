#type vertex
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!
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

	//v_Normal =  mat3(u_TranInverseModel)* aNormal;//vec3(u_Model * vec4(aNormal,1.0));
}


#type fragment
#version 450 core


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
//	
//	float shininess;
//	bool isBlinnLight;
//	int  isTextureSample;//判断是否使用texture,或者只有color
//
//}u_Material;
 uniform Material u_Material;
 uniform Settings u_Settings;

out vec4 FragColor;

in vec2 v_TexCoord;
in vec3 v_Normal;		
in vec3 v_FragPos;	

uniform bool u_HasSpotLight;
uniform bool u_HasParallelLight;
uniform int u_PointLightNums;

//uniform Material u_Material;
uniform ParallelLight u_ParallelLight;
uniform PointLight u_PointLights[10];
uniform SpotLight u_SpotLight;

uniform vec3 u_CameraViewPos;
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir);
vec3 CalcParallelLight(ParallelLight light,vec3 normal,vec3 viewDir);
vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 viewDir);

void main()
{    

	vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	vec3 outColor =vec3(0.0,0.0,0.0);
//	if(u_HasParallelLight)
//		outColor += CalcParallelLight(u_ParallelLight,v_Normal,viewDir);
//	if(u_HasSpotLight)
//		outColor += CalcSpotLight(u_SpotLight, v_Normal,viewDir);
	int uPointLightNum = u_PointLightNums;
	for(int i=0;i<uPointLightNum;i++){
		outColor += CalcPointLight(u_PointLights[i], v_Normal,viewDir);
	}

	FragColor = vec4(outColor,1.0);
}

int uDiffuseTextureSample = u_Settings.isDiffuseTextureSample;

vec3 CalcParallelLight(ParallelLight light,vec3 normal,vec3 viewDir){
	vec3 fragColor;
	//ambient

	vec3 ambient = light.ambient *  (u_Material.ambientColor);// * (1-u_Settings.isTextureSample)+ texture(u_Material.diffuse,v_TexCoord).rgb * u_Settings.isTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;//u_LightColor * u_Material.ambient
	
	//diffuse
	vec3 lightDir = normalize(-light.direction);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse;
	if(uDiffuseTextureSample==1)
		diffuse = light.diffuse * diff *texture(u_Material.diffuse,v_TexCoord).rgb;
	else
		diffuse = light.diffuse * diff *u_Material.diffuseColor;

//	= light.diffuse * diff *( u_Material.diffuseColor *(1-u_Settings.isTextureSample)
//					+ texture(u_Material.diffuse,v_TexCoord).rgb*u_Settings.isTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
	
	//specular
	vec3 reflectDir = normalize(reflect(-lightDir,norm));
	float spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
	vec3 specular =  light.specular * spec  *  u_Material.specularColor;//texture(u_Material.specular,v_TexCoord).rgb;


	fragColor = diffuse + ambient + specular;// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
	
	return fragColor;
}

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir){
	vec3 fragColor;
//	int textureSample = u_Settings.isTextureSample;
	float distance = length(light.position-v_FragPos);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	//ambient
	vec3 ambient ;
	if(uDiffuseTextureSample==1)
		ambient = light.ambient *texture(u_Material.diffuse,v_TexCoord).rgb;
	else
		ambient = light.ambient * u_Material.ambientColor;
	//vec3 ambient = light.ambient * (u_Material.ambientColor * (1-u_Settings.isTextureSample)+ texture(u_Material.diffuse,v_TexCoord).rgb * u_Settings.isTextureSample);
//	vec3 ambient = light.ambient * u_Material.ambientColor 
//					   *texture(u_Material.diffuse,v_TexCoord).rgb;
	
	//diffuse
	vec3 lightDir = normalize(light.position-v_FragPos);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);

	vec3 diffuse;
	

	if(uDiffuseTextureSample==1)
		diffuse = light.diffuse * diff *texture(u_Material.diffuse,v_TexCoord).rgb;
	else
		diffuse = light.diffuse * diff *u_Material.diffuseColor;

//	vec3 diffuse = light.diffuse * diff * ( u_Material.diffuseColor *(1-u_Settings.isTextureSample)
//					+ texture(u_Material.diffuse,v_TexCoord).rgb*u_Settings.isTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
//	vec3 diffuse = light.diffuse * diff *  u_Material.diffuseColor *texture(u_Material.diffuse,v_TexCoord).rgb;

//specular
	vec3 specular;
	float spec;
	bool isBlinnLight = u_Settings.isBlinnLight;
	if(isBlinnLight){

		vec3 halfwayDir = normalize(lightDir+viewDir);
		spec = pow(max(dot(norm,halfwayDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  *  u_Material.specularColor;
	}
	else{

		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  *  u_Material.specularColor;//texture(u_Material.specular,v_TexCoord).rgb;
	}
	int emissionSample = u_Settings.isEmissionTextureSample;
	 vec3 emission = (emissionSample==1)?texture(u_Material.emission, v_TexCoord).rgb:vec3(0);
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
	vec3 ambient = vec3(0.2f) * ( u_Material.ambientColor);//* (1-u_Settings.isTextureSample)+texture(u_Material.diffuse,v_TexCoord).rgb * u_Settings.isTextureSample);// texture(u_Material.diffuse,v_TexCoord).rgb;//u_LightColor * u_Material.ambient
	
	//diffuse
	
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse;
	if(uDiffuseTextureSample==0)
		diffuse = light.diffuse * diff *  (u_Material.diffuseColor);//* (1-u_Settings.isTextureSample)+texture(u_Material.diffuse,v_TexCoord).rgb*u_Settings.isTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
	else
		diffuse = light.diffuse * diff * texture(u_Material.diffuse,v_TexCoord).rgb;
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