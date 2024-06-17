#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;



out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;
out vec4 v_FragPosLightSpace;


uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->�����cpu�������ٴ�����!

uniform mat4 u_LightProjectionViewMatrix;

void main()
{
    gl_Position = u_ProjectionView* u_Model * vec4(aPos, 1.0);
    v_FragPos = vec3(u_Model * vec4(aPos, 1.0));
    v_Normal =  transpose(inverse(mat3(u_Model)))*aNormal;// mat3(u_TranInverseModel) * aNormal;
	v_TexCoords = aTexCoords;
    v_FragPosLightSpace = u_LightProjectionViewMatrix * u_Model*vec4(aPos,1.0);
}

#type fragment
#version 330 core

out vec4 FragColor;


in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoords;
in vec4 v_FragPosLightSpace;


uniform vec3 u_LightPos;
uniform vec3 u_CameraViewPos;

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
//
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
//
//	float shininess;
//	bool isBlinnLight;
//	int  isTextureSample;//�ж��Ƿ�ʹ��texture,����ֻ��color
//
//
//}u_Material;
uniform Material u_Material;

uniform int u_PointLightNums;

uniform ParallelLight u_ParallelLight;
uniform PointLight u_PointLights[100];
uniform SpotLight u_SpotLight;


float ShadowCalculation(vec4 fragPosLightSpace,vec3 normal,vec3 lightDir);



void main()
{           

	vec3 color =texture(u_Material.diffuse, v_TexCoords).rgb;
	vec3 normal = normalize(v_Normal);
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient =   0.5* lightColor*( u_Material.ambientColor * (1-u_Material.isTextureSample) + texture(u_Material.diffuse,v_TexCoords).rgb * u_Material.isTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;//u_LightColor * u_Material.ambient
	
    // Diffuse
	vec3 lightDir = normalize(u_LightPos - v_FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse =  lightColor * diff * ( u_Material.diffuseColor *(1-u_Material.isTextureSample) + texture(u_Material.diffuse,v_TexCoords).rgb*u_Material.isTextureSample);
    // Specular
    vec3 viewDir = normalize(u_CameraViewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0),u_Material.shininess);
    vec3 specular = spec * lightColor * u_Material.specularColor;    
    // ������Ӱ
  
	float shadow = ShadowCalculation(v_FragPosLightSpace,normal,lightDir);       
  //shadow = min(shadow, 0.75);
  vec3 lighting =  ambient +( (1.0 - shadow) * (diffuse + specular)) * color;  // *(1-shadow )* color;//==1.0?vec3(1.0,0.0,0.0):vec3(0.0,0.0,1.0);//(ambient + (1.0 - 0.5) * (diffuse + specular)) * color;   // * (diffuse + specular)
   FragColor = vec4(lighting, 1.0);
}




float ShadowCalculation(vec4 fragPosLightSpace,vec3 normal,vec3 lightDir)
{
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;//͸�ӳ������任��[-1.0,1.0]
   projCoords = projCoords * 0.5 + 0.5;//�任��[0,1]����

   float closestDepth = texture(u_Material.depth,projCoords.xy).r;//��shadowMap ȡ��С��zֵ

   float currentDepth = projCoords.z;
   float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

   ///////////////////////////////////////////////////////////////////
   ////////////////////�������������ܶ������ͼ������Ȼ��ѽ��ƽ������///////////////////
   float shadow = 0.0;
   vec2 texelSize = 1.0/textureSize(u_Material.depth,0);
   for(int x = -1;x<=1;x++){
	for(int y= -1;y<=1;y++){
		float pcfDepth = texture(u_Material.depth,projCoords.xy+vec2(x,y)*texelSize).r;
		shadow +=currentDepth-bias>pcfDepth?1.0:0.0;
	}
   }
   shadow = shadow/9.0;
   ///////////////////////////////////////////////////////////////////////////////////


//    //�Ƚϵ�ǰ��Ⱥ�������������
 //   float shadow = (currentDepth-bias) > closestDepth? 1.0 : 0.1;

   return shadow;
}
