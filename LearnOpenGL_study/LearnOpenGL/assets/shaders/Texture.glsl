#type vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!
uniform mat4 u_ProjectionView;


void main()
{
	gl_Position = u_ProjectionView * u_Model * vec4(aPos,1.0);
	v_FragPos = vec3(u_Model* vec4(aPos,1.0));
	v_TexCoord = vec2(aTexCoord.x,aTexCoord.y);
	v_Normal =  mat3(u_TranInverseModel)* aNormal;//vec3(u_Model * vec4(aNormal,1.0));
}


#type fragment
#version 330 core


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
struct Material{
	//vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;

};

out vec4 FragColor;		
float ambientStrength = 0.1f;
float specularStrength = 1.0f;
//uniform float u_shininessStrength;//反光度因子，可以是2,4,8,16,..256,一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小

in vec2 v_TexCoord;
in vec3 v_Normal;		
in vec3 v_FragPos;		


uniform Material u_Material;
uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;
uniform float u_MixValue;

uniform ParallelLight u_ParallelLight;
uniform PointLight u_PointLight;
uniform SpotLight u_SpotLight;

uniform vec3 u_CameraViewPos;



vec4 CalcPointLight(PointLight light);
vec4 CalcParallelLight(ParallelLight light);
vec4 CalcSpotLight(SpotLight light);



//emission
vec3 emission = texture(u_Material.emission,v_TexCoord).rgb;

void main(){
	
	FragColor = CalcSpotLight(u_SpotLight);
}

vec4 CalcParallelLight(ParallelLight light){
	vec4 fragColor;
	//ambient
	vec3 ambient = light.ambient * texture(u_Material.diffuse,v_TexCoord).rgb;//u_LightColor * u_Material.ambient
	
	//diffuse
	vec3 lightDir = normalize(-light.direction);
	vec3 norm = normalize(v_Normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
	
	//specular
	vec3 reflectDir = normalize(reflect(-lightDir,norm));
	vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	float spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
	vec3 specular =  light.specular * spec  * texture(u_Material.specular,v_TexCoord).rgb;


	fragColor = vec4((diffuse + ambient + specular),1.0);// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
	
	return fragColor;
}

vec4 CalcPointLight(PointLight light){
	vec4 fragColor;

	float distance = length(light.position-v_FragPos);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	//ambient
	vec3 ambient = light.ambient *texture(u_Material.diffuse,v_TexCoord).rgb;//u_LightColor * u_Material.ambient
	
	//diffuse
	vec3 lightDir = normalize(light.position-v_FragPos);
	vec3 norm = normalize(v_Normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
	
	//specular
	vec3 reflectDir = normalize(reflect(-lightDir,norm));
	vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	float spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
	vec3 specular =  light.specular * spec  * texture(u_Material.specular,v_TexCoord).rgb;


	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	fragColor = vec4((diffuse + ambient + specular),1.0);// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
	
	return fragColor;
}

vec4 CalcSpotLight(SpotLight light){
	
	vec3 lightDir = normalize(light.position - v_FragPos);
	float theta = dot(lightDir,normalize(-light.direction));
	vec4 fragColor;
	if(theta>light.cutOff){
		

		float distance = length(light.position-v_FragPos);
		float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
		//ambient
		vec3 ambient = light.ambient *  texture(u_Material.diffuse,v_TexCoord).rgb;//u_LightColor * u_Material.ambient
		
		//diffuse
		vec3 lightDir = normalize(light.position-v_FragPos);
		vec3 norm = normalize(v_Normal);
		float diff = max(dot(lightDir,norm),0.0f);
		vec3 diffuse = light.diffuse * diff * texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
		
		//specular
		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
		float spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
		vec3 specular = light.specular * spec  * texture(u_Material.specular,v_TexCoord).rgb;


		ambient  *= attenuation;
		diffuse  *= attenuation;
		specular *= attenuation;

		fragColor = vec4((diffuse + ambient + specular),1.0);// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
		
		

	}
	else{
	//TODO:l
		fragColor = vec4(light.ambient * texture(u_Material.diffuse,v_TexCoord).rgb,1.0);
	
	}
	
	return fragColor;
}