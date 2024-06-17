#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 5) in ivec4 aJointIndices;/*�뵱ǰfragment��ص�3��Joint  {1,2,4}*/
layout(location = 6) in ivec4 aJointIndices1;/*�뵱ǰfragment��ص�3��Joint  {1,2,4}*/
layout(location = 7) in ivec4 aJointIndices2;/*�뵱ǰfragment��ص�3��Joint  {1,2,4}*/

layout(location = 8) in vec4 aWeights;/*ÿ��Joint��Ȩ��*/
layout(location = 9) in vec4 aWeights1;/*ÿ��Joint��Ȩ��*/
layout(location = 10) in vec4 aWeights2;/*ÿ��Joint��Ȩ��*/

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

const int MAX_JOINTS = 70;//max joints allowed in a skeleton
const int MAX_WEIGHT = 12;//max number of joints that can affect a vertex

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;
uniform mat4 u_JointModel[MAX_JOINTS];
void main(){
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalLocalNormal = vec4(0.0);

	mat4 model = mat4(0.0);
	for(int i=0;i<4;i++){
		if(aWeights[i] == 0.0) continue;
		model+=aWeights[i]*u_JointModel[aJointIndices[i]];

	}
	for(int i=0;i<4;i++){	
		if(aWeights1[i] == 0.0) continue;
		model+=aWeights1[i]*u_JointModel[aJointIndices1[i]];
	}
	for(int i=0;i<4;i++){
		if(aWeights2[i] == 0.0) continue;
		model+=aWeights2[i]*u_JointModel[aJointIndices2[i]];
	}

	vec4 newPos = model*vec4(aPos,1.0);

	gl_Position = u_ProjectionView*u_Model*vec4(newPos.xyz,1.0);
	v_FragPos = vec3(u_Model*newPos);//vec3(u_Model*vec4(newPos.xyz,1.0));

	vec4 newNormal = model*vec4(aNormal,0.0);
	v_Normal =  mat3(u_TranInverseModel)*newNormal.xyz;//vec3(u_Model*newNormal);
	v_TexCoord = aTexCoords;

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


uniform Settings u_Settings;
uniform Material u_Material;
const vec2 lightBias = vec2(0.7, 0.6);//just indicates the balance between diffuse and ambient lighting

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;
out vec4 FragColor;


uniform bool u_HasSpotLight;
uniform bool u_HasParallelLight;
uniform int u_PointLightNums;

//uniform Material u_Material;
uniform ParallelLight u_ParallelLight;
uniform PointLight u_PointLights[100];
uniform SpotLight u_SpotLight;

uniform vec3 u_CameraViewPos;
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir);
vec3 CalcParallelLight(ParallelLight light,vec3 normal,vec3 viewDir);
vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 viewDir);

void main()
{    

	vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	vec3 outColor =vec3(0.0,0.0,0.0);
	if(u_HasParallelLight)
		outColor += CalcParallelLight(u_ParallelLight,v_Normal,viewDir);
	if(u_HasSpotLight)
		outColor += CalcSpotLight(u_SpotLight, v_Normal,viewDir);

	for(int i=0;i<u_PointLightNums;i++){
		outColor += CalcPointLight(u_PointLights[i], v_Normal,viewDir);
	}

	FragColor = vec4(outColor,1.0);
}


vec3 CalcParallelLight(ParallelLight light,vec3 normal,vec3 viewDir){
	vec3 fragColor;
	//ambient
	vec3 ambient = light.ambient *  (u_Material.ambientColor * (1-u_Settings.isAmbientTextureSample)+ texture(u_Material.diffuse,v_TexCoord).rgb * u_Settings.isAmbientTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;//u_LightColor * u_Material.ambient
	
	//diffuse
	vec3 lightDir = normalize(-light.direction);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff *( u_Material.diffuseColor *(1-u_Settings.isDiffuseTextureSample)
					+ texture(u_Material.diffuse,v_TexCoord).rgb*u_Settings.isDiffuseTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
	
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
	vec3 ambient = light.ambient * (u_Material.ambientColor * (1-u_Settings.isAmbientTextureSample)+ texture(u_Material.diffuse,v_TexCoord).rgb * u_Settings.isAmbientTextureSample);
//	vec3 ambient = light.ambient * u_Material.ambientColor 
//					   *texture(u_Material.diffuse,v_TexCoord).rgb;
	
	//diffuse
	vec3 lightDir = normalize(light.position-v_FragPos);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * ( u_Material.diffuseColor *(1-u_Settings.isDiffuseTextureSample)
					+ texture(u_Material.diffuse,v_TexCoord).rgb*u_Settings.isDiffuseTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
//	vec3 diffuse = light.diffuse * diff *  u_Material.diffuseColor *texture(u_Material.diffuse,v_TexCoord).rgb;

//specular
	vec3 specular;
	float spec;
	if(u_Material.isBlinnLight){

		vec3 halfwayDir = normalize(lightDir+viewDir);
		spec = pow(max(dot(norm,halfwayDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  *  (u_Material.specularColor*(1-u_Settings.isSpecularTextureSample)+texture(u_Material.specular,v_TexCoord).rgb*u_Settings.isSpecularTextureSample);
	}
	else{

		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  * (u_Material.specularColor*(1-u_Settings.isSpecularTextureSample)+texture(u_Material.specular,v_TexCoord).rgb*u_Settings.isSpecularTextureSample);//texture(u_Material.specular,v_TexCoord).rgb;
	}
	vec3 emission =(u_Settings.isEmissionTextureSample==1)? texture(u_Material.emission, v_TexCoord).rgb:vec3(0);
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	emission *= attenuation;

	fragColor = diffuse + ambient + specular + emission;// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
	
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
	vec3 ambient = light.ambient * (u_Material.ambientColor * (1-u_Settings.isAmbientTextureSample)+ texture(u_Material.diffuse,v_TexCoord).rgb * u_Settings.isAmbientTextureSample);
	
	//diffuse
	
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * ( u_Material.diffuseColor *(1-u_Settings.isDiffuseTextureSample)
					+ texture(u_Material.diffuse,v_TexCoord).rgb*u_Settings.isDiffuseTextureSample);
	//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
	
	//specular
	vec3 reflectDir = normalize(reflect(-lightDir,norm));
	//vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	float spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
	vec3 specular =  light.specular * spec  * (u_Material.specularColor*(1-u_Settings.isSpecularTextureSample)+texture(u_Material.specular,v_TexCoord).rgb*u_Settings.isSpecularTextureSample);


	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	
	diffuse  *= intensity;
	specular *= intensity;
	//ambient *= intensity;
	fragColor = diffuse + ambient + specular;// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
		
		


		return fragColor;
}
