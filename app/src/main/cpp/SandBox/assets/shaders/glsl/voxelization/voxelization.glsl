  #type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!


out vec3 worldPositionGeom;
out vec3 normalGeom;

void main(){
	worldPositionGeom = vec3(u_Model * vec4(aPos,1.0));

	normalGeom = mat3(transpose(inverse(u_Model)))*aNormal;
	normalGeom=normalize(normalGeom);
	gl_Position = u_ProjectionView * vec4(worldPositionGeom, 1.0);
}

#type geometry
#version 430 core

layout(triangles) in;
layout(triangle_strip,max_vertices = 3) out;

in vec3 worldPositionGeom[];
in vec3 normalGeom[];

out vec3 worldPositionFrag;
out vec3 normalFrag;
uniform vec3 u_CameraViewPos;
uniform vec3 u_CubeSize;
out vec3 normal_worldPositionFrag;

/*几何着色器后会进入光栅化阶段，因此需要把坐标转换的 [-1,1]的裁剪平面！！*/

void main(){
	vec3 p1 = worldPositionGeom[1] - worldPositionGeom[0];
	vec3 p2 = worldPositionGeom[2] - worldPositionGeom[0];
	vec3 p = abs(cross(p1, p2)); 
	for(uint i = 0; i < 3; ++i){
		worldPositionFrag = worldPositionGeom[i];//
		normal_worldPositionFrag=(worldPositionGeom[i]-u_CameraViewPos)/u_CubeSize;
		normalFrag = normalGeom[i];
		if(p.z > p.x && p.z > p.y){
			gl_Position = vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.y, 0, 1);
		} else if (p.x > p.y && p.x > p.z){
			gl_Position = vec4(normal_worldPositionFrag.y, normal_worldPositionFrag.z, 0, 1);
		} else {
			gl_Position = vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.z, 0, 1);
		}
		EmitVertex();
	}
    EndPrimitive();
}

// Lit (diffuse) fragment voxelization shader.
// Author:	Fredrik Pr鋘tare <prantare@gmail.com> 
// Date:	11/26/2016
#type fragment
#version 430 core

// Lighting settings.
#define POINT_LIGHT_INTENSITY 1
#define MAX_LIGHTS 5

// Lighting attenuation factors.
#define DIST_FACTOR 1.1f /* Distance is multiplied by this when calculating attenuation. */
#define CONSTANT 1
#define LINEAR 0
#define QUADRATIC 1

// Returns an attenuation factor given a distance.
float attenuate(float dist){ dist *= DIST_FACTOR; return 1.0f / (CONSTANT + LINEAR * dist + QUADRATIC * dist * dist); }

struct PointLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
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
	int  isTextureSample;//判断是否使用texture,或者只有color

}u_Material;

uniform PointLight u_PointLights[5];
uniform int u_PointLightNums;
uniform vec3 u_CameraViewPos;
uniform vec3 u_CubeSize;




layout(rgba8, binding = 0) uniform image3D texture3D;

in vec3 worldPositionFrag;//=v_FragPos
in vec3 normalFrag;

//vec3 calculatePointLight(const PointLight light){
//	const vec3 direction = normalize(light.position - worldPositionFrag);
//	const float distanceToLight = distance(light.position, worldPositionFrag);
//	const float attenuation = attenuate(distanceToLight);
//	const float d = max(dot(normalize(normalFrag), direction), 0.0f);
//	return d * POINT_LIGHT_INTENSITY * attenuation * light.color;
//};
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir){
	vec3 fragColor;

	float distance = length(light.position-worldPositionFrag);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	//ambient
	vec3 ambient = light.ambient * (u_Material.ambientColor );
//	vec3 ambient = light.ambient * u_Material.ambientColor 
//					   *texture(u_Material.diffuse,v_TexCoord).rgb;
	
	//diffuse
	vec3 lightDir = normalize(light.position-worldPositionFrag);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * ( u_Material.diffuseColor );//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
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
	// vec3 emission = texture(u_Material.emission, v_TexCoord).rgb;
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	//emission *= attenuation;

	fragColor = diffuse + ambient + specular;// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
	
	return fragColor;
}

vec3 scaleAndBias(vec3 p) { return 0.5f * p + vec3(0.5f); }

bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }
//bool isInsideCube(const vec3 p, float e) { return abs(p.x) < u_CubeSize.x + e && abs(p.y) < u_CubeSize.y + e && abs(p.z) < u_CubeSize.z + e; }
//bool isInsideCube(const vec3 p, float e) 
//{
//	//u_CameraViewPos+u_CameraRight*u_CameraRight;
//	return	p.x < u_CameraViewPos.x + u_CubeSize.x && p.x > u_CameraViewPos.x - u_CubeSize.x &&
//			p.y < u_CameraViewPos.y + u_CubeSize.y && p.y > u_CameraViewPos.y - u_CubeSize.y &&
//			 p.z < u_CameraViewPos.z + u_CubeSize.z && p.z > u_CameraViewPos.z - u_CubeSize.z ;
//
//}

void main(){
	vec3 viewDir = normalize(u_CameraViewPos-worldPositionFrag);

	vec3 color = vec3(0.0);
	//vec3 worldPositionFrag_=u_CubeSize*worldPositionFrag;

	vec3 normalWorldPositionFrag =worldPositionFrag-u_CameraViewPos;// vec3(worldPositionFrag.x-u_CubePos.x,worldPositionFrag.y-u_CubePos.y,worldPositionFrag.z-u_CubePos.z);
	normalWorldPositionFrag = normalWorldPositionFrag/u_CubeSize;
		if(!isInsideCube(normalWorldPositionFrag, 0)) return;

	//vec3 normalWorldPositionFrag =worldPositionFrag;
	// Calculate diffuse lighting fragment contribution.
	for(int i = 0; i < u_PointLightNums; ++i) 
	{
	color += CalcPointLight(u_PointLights[i], normalFrag,viewDir);

	}
//	vec3 spec = material.specularReflectivity * material.specularColor;
//	vec3 diff = material.diffuseReflectivity * material.diffuseColor;
//	color = (diff + spec) * color + clamp(material.emissivity, 0, 1) * material.diffuseColor;

	// Output lighting to 3D texture.
	vec3 voxel = scaleAndBias(normalWorldPositionFrag);
	ivec3 dim = imageSize(texture3D);// retrieve the dimensions of an image
	float alpha = pow(1 - 0, 4); // For soft shadows to work better with transparent materials.
	vec4 res = alpha * vec4(vec3(color), 1);
    imageStore(texture3D, ivec3(dim * voxel), res);//write a single texel into an image;
}