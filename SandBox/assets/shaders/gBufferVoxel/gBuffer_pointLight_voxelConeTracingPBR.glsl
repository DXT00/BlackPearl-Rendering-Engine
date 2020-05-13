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
	v_TexCoords = aTexCoords;

	gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);

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

struct PointLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	float intensity;

};

uniform PointLight u_PointLight;

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


/************************************ PBR function ************************************************/
const float PI=3.14159265359;
float calculateAttenuation(PointLight light,vec3 fragPos){
	float distance = length(light.position-fragPos);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	return attenuation;
}
float NoemalDistribution_TrowbridgeReitz_GGX(vec3 N,vec3 H,float roughness){
	float roughness2 = roughness*roughness;
	float NHDOT = max(abs(dot(N,H)),0.0);
	float tmp= (NHDOT*NHDOT)*(roughness2-1.0)+1.0;
	float NDF = roughness2/(PI*tmp*tmp);
	return NDF;
}
float Geometry_SchlickGGX(float NdotV,float roughness){
	return NdotV/(NdotV*(1.0-roughness)+roughness);
}

float GeometrySmith(vec3 N,vec3 V,vec3 L,float roughness){
	float NdotV = max(dot(N,V),0.0);
	float NdotL = max(dot(N,L),0.0);
	float ggx1 = Geometry_SchlickGGX(NdotV,roughness);
	float ggx2 = Geometry_SchlickGGX(NdotL,roughness);

	return ggx1*ggx2;
}
vec3 FresnelSchlick(float cosTheta, vec3 F0){
	return F0 + (1.0-F0)* pow(1.0 - cosTheta, 5.0);
}
vec3 BRDF(vec3 Kd,vec3 Ks,vec3 specular,vec3 albedo){
	vec3 fLambert = albedo/PI;//diffuseColor 相当于 albedo
	return Kd * fLambert+  specular;//specular 中已经有Ks(Ks=F)了，不需要再乘以Ks *
}
vec3 LightRadiance(vec3 fragPos,PointLight light){
	float attenuation = calculateAttenuation(light,fragPos);
	vec3 radiance = light.diffuse*attenuation;
	return radiance;
}
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 CalcPBRPointLight(PointLight light,vec3 getNormalFromMap,vec3 albedo,float metallic,float roughness,vec3 fragPos){
	
	//tangent normal 
	vec3 N = getNormalFromMap;//getNormalFromMap(normal,normalMap,fragPos);
	vec3 V = normalize(u_CameraViewPos-fragPos);
	vec3 R = reflect(-V,N);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0,albedo,metallic);

	//reflection equation
	vec3 Lo = vec3(0.0);
	vec3 L = normalize(light.position-fragPos);
	vec3 H = normalize(V+L);
	float attenuation = calculateAttenuation(light,fragPos);

	float NDF = NoemalDistribution_TrowbridgeReitz_GGX(N,H,roughness);
	float G = GeometrySmith(N, V,L,roughness);
	vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
	
	//CookTorrance
	vec3 nominator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
    vec3 specular     = nominator / denominator;
	
	vec3 Ks = F;

	vec3 Kd = vec3(1.0)-Ks;
	
	Kd *= (1.0 - metallic);

	
	float NdotL = max(dot(N,L),0.0);
	Lo+= BRDF(Kd,Ks,specular,albedo)*LightRadiance(fragPos,light)*NdotL;

	return Lo;
}

/*************************************************************************************************/
float traceShadowCone(vec3 from, vec3 lightPos,vec3 normal){
	from = from-u_CameraViewPos;
	lightPos = lightPos-u_CameraViewPos;
	
	vec3 fragPosToLight = lightPos-from;
	vec3 direction = fragPosToLight/u_CubeSize;
	from = from/u_CubeSize;
	//targetDistance = targetDistance/u_CubeSize.x;

	from += normal * 0.005; // Removes artifacts but makes self shadowing for dense meshes meh.

	float acc = 0;

	float dist = 3 * VOXEL_SIZE;
	// I'm using a pretty big margin here since I use an emissive light ball with a pretty big radius in my demo scenes.
	const float STOP = length(direction) - 16 * VOXEL_SIZE;
	direction = normalize(direction);
	while(dist < STOP && acc < 1){	
		vec3 c = from + dist * direction;
		if(!isInsideCube(c, 0)) break;
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




vec3 calculateDirectLight(float shadow,vec3 fragPos,vec3 normal,const PointLight light, const vec3 viewDir,vec3 diffuseColor,vec3 specularColor){
	vec3 direct = vec3(0.0);

	vec3 lightDir = light.position - fragPos;
//	//MY 
//	vec3 ligtdir = lightDirection/u_CubeSize;
//	float distToLight = length(ligtdir);

	const float distanceToLight = length(lightDir);
	lightDir = lightDir / distanceToLight;
	float attenuation = attenuate(distanceToLight) ;//1.0f/(light.constant+light.linear * distanceToLight+light.quadratic*distanceToLight*distanceToLight);

	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * min(diff,shadow) * diffuseColor;

	/*specular*/
	vec3 specular;
	float spec;
	float shininess = 64.0;
	if(uBlinLight){

		vec3 halfwayDir = normalize(lightDir+viewDir);
		spec = pow(max(dot(norm,halfwayDir),0.0),shininess);
		specular =  light.specular * min(spec,shadow)  *  specularColor;
	}
	else{

		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),shininess);
		specular =  light.specular * min(spec,shadow) * specularColor;//texture(material.specular,v_TexCoords).rgb;
	}
	// vec3 emission = texture(u_Material.emission, v_TexCoord).rgb;


	//ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	//emission *= attenuation;

	direct = diffuse + specular;//+ ambient +emission;// * mix(texture(u_Texture1, v_TexCoords), texture(u_Texture2, vec2(1.0 - v_TexCoords.x, v_TexCoords.y)), u_MixValue);
	



	return direct;
	
};


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
	if(uShadow)
		shadowBlend = traceShadowCone(gBuffer.fragPos,u_PointLight.position,gBuffer.normal);
	
	
	
	vec3 direct = vec3(0.0);
	if(uDirectLight){
		if(gBuffer.isPBRObject == 0){
		
			//direct += u_PointLights[i].intensity * calculateDirectLight_orign(u_PointLights[i], viewDirection);
			//(vec3 fragPos,vec3 normal,const PointLight light, const vec3 viewDir,vec3 diffuseColor,vec3 specularColor)
			direct = u_PointLight.intensity * calculateDirectLight(shadowBlend,gBuffer.fragPos,gBuffer.normal,u_PointLight, viewDirection,gBuffer.diffuseColor,gBuffer.specularColor);
			//direct = vec3(1,0,0);
		}
		else{
	

			vec3  albedo	= pow(gBuffer.diffuseColor, vec3(2.2));
			float metallic  = gBuffer.metallic;
			float roughness = gBuffer.roughness;
			float ao        = gBuffer.ao;
			vec3  normalMap = gBuffer.getNormalFromMap;
			vec3  fragPos   = gBuffer.fragPos;
		
			direct =  shadowBlend*u_PointLight.intensity * CalcPBRPointLight(u_PointLight,normalMap,albedo,metallic,roughness,fragPos);
		
		}

	}
	FragColor.rgb = direct;

}
