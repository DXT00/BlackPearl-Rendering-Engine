#type vertex
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
out vec2 v_TexCoords;


void main()
{
	//v_TexCoords = aTexCoords;

	//gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);
	gl_Position =vec4(aPos,1.0);

}

#type fragment
#version 450 core


#define DIST_FACTOR 1.1f /* Distance is multiplied by this when calculating attenuation. */
#define CONSTANT 1
#define LINEAR 0 /* Looks meh when using gamma correction. */
#define QUADRATIC 1
struct GBuffer{
	vec3 fragPos;
	vec3 normal;
	vec3 getNormalFromMap;
	vec3 diffuseColor;
	vec3 specularColor;
	int isPBRObject;
	float roughness;
	float metallic ;
	float ao;

}gBuffer;


struct ParallelLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
	float intensity;
};
uniform ParallelLight u_ParallelLight;

uniform vec3 u_CameraViewPos;
uniform vec3 u_CubeSize; //m_CubeObj的大小，控制体素化范围
uniform sampler3D texture3D;
uniform float u_VoxelDim;
const float VOXEL_SIZE=1.0/u_VoxelDim; /* Size of a voxel. 128x128x128 => 1/128 = 0.0078125. */

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse_Roughness;
uniform sampler2D gSpecular_Mentallic;
uniform sampler2D gAmbientGI_AO;
uniform sampler2D gNormalMap;

uniform Settings u_Settings;
uniform vec2 u_ScreenSize;
bool uDirectLight = u_Settings.directLight;
bool uShadow = u_Settings.shadows;
bool uBlinLight = u_Settings.isBlinnLight;


out vec4 FragColor;

vec3 scaleAndBias(vec3 p){
	return 0.5f * p + vec3(0.5f); 
}

// Returns true if the point p is inside the unity cube. 
//bool isInsideCube(const vec3 p, float e) { return abs(p.x) < u_CubeSize.x + e && abs(p.y) < u_CubeSize.y + e && abs(p.z) < u_CubeSize.z + e; }


bool isInsideCube(const vec3 p, float e) {
	return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; 
}
vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / u_ScreenSize;
}


float attenuate(float dist){ 
	dist *= DIST_FACTOR;
	return 1.0f / (CONSTANT + LINEAR * dist + QUADRATIC * dist * dist); 
}



/*************************************************************************************************/
float traceShadowCone(vec3 from,vec3 lightDir,vec3 normal){
	from = from-u_CameraViewPos;
	from = from/u_CubeSize;

		//targetDistance = targetDistance/u_CubeSize.x;

	from += normal * 0.005; // Removes artifacts but makes self shadowing for dense meshes meh.

	float acc = 0;

	float dist = 3 * VOXEL_SIZE;
	// I'm using a pretty big margin here since I use an emissive light ball with a pretty big radius in my demo scenes.
	//const float STOP = length(direction) - 16 * VOXEL_SIZE;
	vec3 direction = normalize(lightDir);
	while(dist < 1 && acc < 1){	
		vec3 c = from + dist * direction;
		if(!isInsideCube(c, 0)) return 1.0;
		c = scaleAndBias(c);
		float l = pow(dist, 2); // Experimenting with inverse square falloff for shadows.

		vec4 color1 = textureLod(texture3D, c,( 1 + 0.75 * l));
		vec4 color2 = textureLod(texture3D, c, (2.5 * l));

		if(color1.a>=0&&color2.a>=0){
			float s1 = 0.062 * textureLod(texture3D, c,( 1 + 0.75 * l)).a;
			float s2 = 0.135 * textureLod(texture3D, c, (4.5 * l)).a;
			//		float s1 = 0.062 * textureLod(texture3D, c, 1 + 0.75 * l).a;
			//		float s2 = 0.135 * textureLod(texture3D, c, 4.5 * l).a;
			float s = s1+ s2;
		
			acc += (1 - acc) * s;

		}
		
		dist += 0.9 * VOXEL_SIZE * (1 + 0.05 * l);//0.9 * VOXEL_SIZE * (1 + 0.05 * l);
	}
	return 1 - pow(smoothstep(0, 1, acc * 1.4), 1.0 / 1.4);
}	




vec3 CalcParallelLight(ParallelLight light,vec3 normal,vec3 viewDir,vec3 diffuseColor,vec3 specularColor,float shininess){
	vec3 fragColor;	
	//diffuse
	vec3 lightDir = normalize(-light.direction);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse;
	diffuse = light.diffuse * diff *diffuseColor;


	//specular
	vec3 reflectDir = normalize(reflect(-lightDir,norm));
	float spec = pow(max(dot(reflectDir,viewDir),0.0),shininess);
	vec3 specular =  light.specular * spec  * specularColor;//texture(u_Material.specular,v_TexCoord).rgb;


	fragColor = diffuse +  specular;// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
	
	return fragColor;
}



void main(){
	FragColor = vec4(0,0,0,0);//a必须是0，和 ambient pass Blend之后，ambientGI pass会用a来判断是否滤波

	vec2 texCoords = CalcTexCoord();

	gBuffer.fragPos = texture(gPosition,texCoords).rgb;
	gBuffer.normal = texture(gNormal,texCoords).xyz;
	gBuffer.getNormalFromMap = texture(gNormalMap,texCoords).xyz;
	gBuffer.diffuseColor = texture(gDiffuse_Roughness,texCoords).rgb;
	gBuffer.specularColor = texture(gSpecular_Mentallic,texCoords).rgb;
	gBuffer.isPBRObject = int(texture(gPosition,texCoords).a/256.0);
	//gBuffer.isSkyBox = (int)texture(gNormal,texCoords).a;
	gBuffer.roughness = texture(gDiffuse_Roughness,texCoords).a;
	gBuffer.metallic = texture(gSpecular_Mentallic,texCoords).a;
	gBuffer.ao = texture(gAmbientGI_AO,texCoords).a;
	const vec3 viewDirection = normalize(gBuffer.fragPos-u_CameraViewPos);

	float shadowBlend = 1.0;
//	if(uShadow)
		
	
	
	
	vec3 direct = vec3(0.0);
	if(uDirectLight){
		shadowBlend = traceShadowCone(gBuffer.fragPos,u_ParallelLight.direction,gBuffer.normal);
		direct = shadowBlend*u_ParallelLight.intensity * CalcParallelLight(u_ParallelLight,gBuffer.normal, viewDirection,gBuffer.diffuseColor,gBuffer.specularColor, gBuffer.metallic);

	}
	FragColor.rgb = direct;

}
