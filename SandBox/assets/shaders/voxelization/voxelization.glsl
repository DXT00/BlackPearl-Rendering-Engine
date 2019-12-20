// Author:	Fredrik Pr鋘tare <prantare@gmail.com>
// Date:	11/26/2016
//#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!


out vec3 worldPositionGeom;
out vec3 normalGeom;
out vec2 v_TexCoord;

void main(){
	worldPositionGeom = vec3(u_Model * vec4(aPos,1.0));
	v_TexCoord = vec2(aTexCoords.x,aTexCoords.y);

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
in vec2 v_TexCoord[];

out vec3 worldPositionFrag;
out vec3 normalFrag;
out vec2 g_TexCoord;

void main(){
	vec3 p1 = worldPositionGeom[1] - worldPositionGeom[0];
	vec3 p2 = worldPositionGeom[2] - worldPositionGeom[0];
	vec3 p = abs(cross(p1, p2)); 
	for(uint i = 0; i < 3; ++i){
		worldPositionFrag = worldPositionGeom[i];
		normalFrag = normalGeom[i];
		g_TexCoord =  v_TexCoord[i];

		if(p.z > p.x && p.z > p.y){
			gl_Position = vec4(worldPositionFrag.x, worldPositionFrag.y, 0, 1);
		} else if (p.x > p.y && p.x > p.z){
			gl_Position = vec4(worldPositionFrag.y, worldPositionFrag.z, 0, 1);
		} else {
			gl_Position = vec4(worldPositionFrag.x, worldPositionFrag.z, 0, 1);
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
#define MAX_LIGHTS 1

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

uniform PointLight u_PointLights[MAX_LIGHTS];
uniform int u_PointLightNums;
uniform vec3 u_CameraViewPos;
layout(rgba8, binding = 0) uniform image3D texture3D;
uniform vec3 u_CubeSize;
in vec3 worldPositionFrag;//=v_FragPos
in vec3 normalFrag;
in vec2 g_TexCoord;


vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir){
	vec3 fragColor;

	float distance = length(light.position-worldPositionFrag);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	//ambient
	vec3 ambient =  light.ambient *  (u_Material.ambientColor * (1-u_Material.isTextureSample)+ texture(u_Material.diffuse,g_TexCoord).rgb * u_Material.isTextureSample);//light.ambient * (u_Material.ambientColor );
	//diffuse
	vec3 lightDir = normalize(light.position-worldPositionFrag);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff *( u_Material.diffuseColor *(1-u_Material.isTextureSample)
					+ texture(u_Material.diffuse,g_TexCoord).rgb*u_Material.isTextureSample);//light.diffuse * diff * ( u_Material.diffuseColor );
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

bool isInsideCube(const vec3 p, float e) { return abs(p.x) < u_CubeSize.x + e && abs(p.y) < u_CubeSize.y + e && abs(p.z) < u_CubeSize.z + e; }
out vec4 color;
void main(){
	vec3 viewDir = normalize(u_CameraViewPos-worldPositionFrag);

	vec3 color = vec3(0.0f);
	if(!isInsideCube(worldPositionFrag, 0)) return;

	// Calculate diffuse lighting fragment contribution.
	int maxLights = min(u_PointLightNums, MAX_LIGHTS);
	for(int i = 0; i < maxLights; ++i) color += CalcPointLight(u_PointLights[i], normalFrag,viewDir);

	// Output lighting to 3D texture.
	vec3 voxel = scaleAndBias(worldPositionFrag);
	ivec3 dim = imageSize(texture3D);// retrieve the dimensions of an image
	float alpha = pow(1 - 0, 4); // For soft shadows to work better with transparent materials.//TODO::透明度
	vec4 res = alpha * vec4(vec3(color), 1);
    imageStore(texture3D, ivec3(dim * voxel), res);//write a single texel into an image; dim * voxel：64.0*([0,1.0])

}