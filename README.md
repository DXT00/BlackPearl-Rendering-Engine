### 添加PointLight shadow map

* 以PointLight为视角，用CubeMap获取深度贴图,通过几何着色器把一个frangment映射到CubeMap的六个面,计算光源到六个fragment的深度，存入CubeMap

Shader Code:
CubeMapDepthShader.glsl

```
#type vertex
#version 330 core
layout (location =0) in vec3 aPos;

uniform mat4 u_Model;
void main(){
	gl_Position = u_Model * vec4(aPos,1.0);
}

#type geometry
#version 330 core
layout (triangles) in;
layout (triangle_strip,max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

void main(){

	for(int face = 0;face<6;++face){
		gl_Layer = face;
		for(int i=0;i<3;++i){
			FragPos = gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face]*FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}

//计算每个Fragment到光源的深度
#type fragment
#version 330 core
in vec4 FragPos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;

void main(){
    // get distance between fragment and light source
	float lightDistance = length(FragPos.xyz-u_LightPos);
	
	//map to [0:1] range by dividing by farPlane;
	lightDistance = lightDistance/u_FarPlane;

	gl_FragDepth = lightDistance;
	
}

```

### 创建Shadow

* 和 Direct Light ShadowMap 一样，计算PointLight与fragment的距离，并和CubeMap对比。
CubeMapShadowMapping.glsl
```

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
	samplerCube cube;

	float shininess;
	bool isBlinnLight;
	int  isTextureSample;//判断是否使用texture,或者只有color


}u_Material;

struct PointLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	float constant;
	float linear;
	float quadratic;

};
uniform int u_PointLightNums;

uniform PointLight u_PointLights[100];
uniform vec3 u_LightPos;
uniform vec3 u_CameraViewPos;
uniform float u_FarPlane ;
uniform samplerCube u_CubeMap;
float ShadowCalculation(vec3 fragPos,vec3 lightPos);
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir);
void main(){
	vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	vec3 outColor ;//=vec3(0.2,0.3,0.9);
	//outColor = CalcParallelLight(u_ParallelLight,v_Normal,viewDir);
	//outColor += CalcSpotLight(u_SpotLight, v_Normal,viewDir);

	for(int i=0;i<u_PointLightNums;i++){

		outColor += CalcPointLight(u_PointLights[i], v_Normal,viewDir);

	}

	FragColor = vec4(outColor,1.0);


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
	vec3 diffuse = light.diffuse * diff * ( u_Material.diffuseColor *(1-u_Material.isTextureSample)
					+ texture(u_Material.diffuse,v_TexCoords).rgb*u_Material.isTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
	

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

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	float shadow = ShadowCalculation(v_FragPos,light.position);       

	fragColor = ambient+ (1.0 - shadow) *(diffuse + specular);// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);

	return fragColor;
}

float ShadowCalculation(vec3 fragPos,vec3 lightPos){

		vec3 fragToLight = fragPos.xyz - lightPos; 

	 float closestDepth = texture(u_Material.cube, fragToLight).r;
	// It is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth = closestDepth*u_FarPlane;
    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // Now test for shadows
  
	float shadow = 0.0;
	float bias = 0.05; 
	float samples = 4.0;
	float offset = 0.1;
	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
	    for(float y = -offset; y < offset; y += offset / (samples * 0.5))
	    {
	        for(float z = -offset; z < offset; z += offset / (samples * 0.5))
	        {
	            float closestDepth = texture(u_Material.cube, fragToLight + vec3(x, y, z)).r; 
	            closestDepth *= u_FarPlane;   // Undo mapping [0;1]
	            if(currentDepth - bias > closestDepth)
	                shadow += 1.0;
	        }
	    }
	}
	shadow /= (samples * samples * samples);
	return shadow;
}

```
![Alt text](/results/ImageNew1.PNG)