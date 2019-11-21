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
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!

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
	sampler2D depth;

	float shininess;
	bool isBlinnLight;
	int  isTextureSample;//判断是否使用texture,或者只有color


}u_Material;


uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
float ShadowCalculation(vec4 fragPosLightSpace,vec3 normal,vec3 lightDir)
{
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;//透视除法，变换到[-1.0,1.0]
   projCoords = projCoords * 0.5 + 0.5;//变换到[0,1]坐标

   float closestDepth = texture(u_Material.depth,projCoords.xy).r;//从shadowMap 取最小的z值

   float currentDepth = projCoords.z;
       float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.010);
//
////   float bias = 0.005;
//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.5;
// float shadow = 0.0;
//    vec2 texelSize = 1.0 / textureSize(u_Materal.depth, 0);
//    for(int x = -1; x <= 1; ++x)
//    {
//        for(int y = -1; y <= 1; ++y)
//        {
//            float pcfDepth = texture(u_Materal.depth, projCoords.xy + vec2(x, y) * texelSize).r; 
//            shadow += (currentDepth - bias )> pcfDepth  ? 1.0 : 0.0;        
//        }    
//    }
//    shadow /= 9.0;
//
//    //比较当前深度和最近采样点深度
    float shadow = (currentDepth-bias) > closestDepth? 1.0 : 0.5;
//    //超出深度图区域的修正
//    if (projCoords.z > 1.0)
//        shadow = 0.0;
   return shadow;
}



void main()
{           
 vec3 color =texture(u_Material.diffuse, v_TexCoords).rgb;
 vec3 normal = normalize(v_Normal);
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient = 0.5* color;
    // Diffuse
 vec3 lightDir = normalize(u_LightPos - v_FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(u_CameraViewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0),u_Material.shininess);
    vec3 specular = spec * lightColor;    
    // 计算阴影
  
	float shadow = ShadowCalculation(v_FragPosLightSpace,normal,lightDir);       
  //shadow = min(shadow, 0.75);
  vec3 lighting =  ambient +( (1.0 - shadow) * (diffuse + specular)) * color;  ;// *(1-shadow )* color;//==1.0?vec3(1.0,0.0,0.0):vec3(0.0,0.0,1.0);//(ambient + (1.0 - 0.5) * (diffuse + specular)) * color;   // * (diffuse + specular)
   FragColor = vec4(lighting, 1.0);
}