#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main(){
	v_FragPos = vec3(u_Model*vec4(aPos,1));
	v_Normal = normalize(mat3(transpose(inverse(u_Model)))*aNormal);
	v_TexCoord=vec2(aTexCoords.x,aTexCoords.y);
	gl_Position = u_ProjectionView*vec4(v_FragPos,1);
}

#type fragment
#version 430 core


#define TSQRT2 2.828427
#define SQRT2 1.414213
#define ISQRT2 0.707106
// --------------------------------------
// Light (voxel) cone tracing settings.
// --------------------------------------
#define MIPMAP_HARDCAP 5.4f /* Too high mipmap levels => glitchiness, too low mipmap levels => sharpness. */
#define VOXEL_SIZE (1/256.0) /* Size of a voxel. 128x128x128 => 1/128 = 0.0078125. */
#define SHADOWS 1 /* Shadow cone tracing. */
#define DIFFUSE_INDIRECT_FACTOR 0.8f  
//0.52f /* Just changes intensity of diffuse indirect lighting. */
// --------------------------------------
// Other lighting settings.
// --------------------------------------
#define SPECULAR_MODE 1 /* 0 == Blinn-Phong (halfway vector), 1 == reflection model. */
#define SPECULAR_FACTOR 4.0f /* Specular intensity tweaking factor. */
#define SPECULAR_POWER 65.0f /* Specular power in Blinn-Phong. */
#define DIRECT_LIGHT_INTENSITY 0.96f /* (direct) point light intensity factor. */
#define MAX_LIGHTS 5 /* Maximum number of lights supported. */

// Lighting attenuation factors. See the function "attenuate" (below) for more information.
#define DIST_FACTOR 1.1f /* Distance is multiplied by this when calculating attenuation. */
#define CONSTANT 1
#define LINEAR 0 /* Looks meh when using gamma correction. */
#define QUADRATIC 1

// Other settings.
#define GAMMA_CORRECTION 1 /* Whether to use gamma correction or not. */
#define HDR 0 /* Whether to use tone mapping or not. */

// Basic point light.
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

//struct Material{
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
//	float diffuseReflectivity;
//	float specularReflectivity;
//	float transparency;
//	float emissivity;
//	float refractiveIndex;
//	float specularDiffusion;
//
//	float shininess;
//	bool isBlinnLight;
//	int  isTextureSample;//判断是否使用texture,或者只有color
//
//};
//
//struct Settings{
//	bool indirectSpecularLight;
//	bool indirectDiffuseLight;
//	bool directLight;
//	bool shadows;
//};

uniform Material u_Material;
uniform Settings u_Settings;
uniform vec3 u_CameraViewPos;
uniform int u_PointLightNums;
uniform PointLight u_PointLights[5];
uniform int u_State;
uniform sampler3D texture3D;
uniform vec3 u_CubeSize; //m_CubeObj的大小，控制体素化范围
//uniform vec3 u_CubePos; //m_CubeObj的大小，控制体素化范围
uniform int u_IsPBRObjects;
uniform sampler2D u_BrdfLUTMap;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 color;



vec3 normal = normalize(v_Normal);
float MAX_DISTANCE = distance(vec3(abs(v_FragPos)), vec3(-1));
float attenuate(float dist){ dist *= DIST_FACTOR; return 1.0f / (CONSTANT + LINEAR * dist + QUADRATIC * dist * dist); }

// Returns a vector that is orthogonal to u.
vec3 orthogonal(vec3 u){
	u = normalize(u);
	vec3 v = vec3(0.99146, 0.11664, 0.05832); // Pick any normalized vector.
	return abs(dot(u, v)) > 0.99999f ? cross(u, vec3(0, 1, 0)) : cross(u, v);
}
vec3 scaleAndBias(vec3 p){
	return 0.5f * p + vec3(0.5f); 
}

// Returns true if the point p is inside the unity cube. 
//bool isInsideCube(const vec3 p, float e) { return abs(p.x) < u_CubeSize.x + e && abs(p.y) < u_CubeSize.y + e && abs(p.z) < u_CubeSize.z + e; }
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }



/************************** PBR function*****************************/
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
vec3 getNormalFromMap(vec3 normal,vec3 fragPos,vec2 texCoord)
{
    vec3 tangentNormal =  2.0* texture(u_Material.normal,texCoord).xyz- vec3(1.0);

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(texCoord);
    vec2 st2 = dFdy(texCoord);

    vec3 N   = normalize(normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
vec3 CalcPBRIndirectLight(vec3 indirectSpecular,vec3 indirectDiffuse,vec3 getNormalFromMap,vec3 albedo,float metallic,float roughness,float ao,vec3 fragPos){

	vec3 N = getNormalFromMap;
	vec3 V = normalize(u_CameraViewPos-fragPos);
	vec3 R = reflect(-V,N);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0,albedo,metallic);
//ambient lightings (we now use IBL as the ambient term)!
	vec3 F =  FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 Ks = F;
	vec3 Kd = vec3(1.0)-Ks;
	Kd *= (1.0 - metallic);
	vec3 environmentIrradiance;//= vec3(1.0,1.0,1.0);
	
	environmentIrradiance=indirectDiffuse;// u_ProbeWeight[i]*texture(u_IrradianceMap[i],N).rgb;
		


	vec3 diffuse = environmentIrradiance*albedo;//u_Material.diffuseColor;

	//sample both the prefilter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part
	const float MAX_REFLECTION_LOD = 1.0;
	//sample MAX_REFLECTION_LOD level mipmap everytime !
	vec3 prefileredColor = vec3(0.0,0.0,0.0) ;//= vec3(1.0,1.0,1.0);

	/*specular Map只取最近的一个*/
	prefileredColor = indirectSpecular;//textureLod(u_PrefilterMap[0],R,roughness*MAX_REFLECTION_LOD).rgb;
//	for(int i=0;i<u_Kprobes;i++){
//		prefileredColor+= u_ProbeWeight[i]*textureLod(u_PrefilterMap[i],R,roughness*MAX_REFLECTION_LOD).rgb;
//	//	prefileredColor*= textureLod(u_PrefilterMap[i],R,roughness*MAX_REFLECTION_LOD).rgb;
//
//	}
	vec2 brdf = texture(u_BrdfLUTMap,vec2(max(dot(N,V),0.0),roughness)).rg;

	vec3 specular = prefileredColor * (F*brdf.x+brdf.y);
	vec3 ambient =  (Kd*diffuse+specular) * ao;
	return ambient;
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
	//vec3 radiance = u_PointLights[i].diffuse * attenuation;

	float NDF = NoemalDistribution_TrowbridgeReitz_GGX(N,H,roughness);
	float G = GeometrySmith(N, V,L,roughness);
	vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
	
	//CookTorrance
	vec3 nominator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
    vec3 specular     = nominator / denominator;
	
	// kS is equal to Fresnel
	vec3 Ks = F;
	// for energy conservation, the diffuse and specular light can't
       // be above 1.0 (unless the surface emits light); to preserve this
       // relationship the diffuse component (kD) should equal 1.0 - kS.
	vec3 Kd = vec3(1.0)-Ks;
	 // multiply kD by the inverse metalness such that only non-metals 
       // have diffuse lighting, or a linear blend if partly metal (pure metals
       // have no diffuse light).
	Kd *= (1.0 - metallic);

	
	float NdotL = max(dot(N,L),0.0);
	Lo+= BRDF(Kd,Ks,specular,albedo)*LightRadiance(fragPos,light)*NdotL;

	return Lo;
}


/**********************************************************************/



// Returns a soft shadow blend by using shadow cone tracing.
// Uses 2 samples per step, so it's pretty expensive.
//		shadowBlend = traceShadowCone(v_FragPos, lightDirection, distanceToLight);

float traceShadowCone(vec3 from, vec3 lightPos){
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
	const float STOP = length(direction) - 16 * VOXEL_SIZE;//16
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


vec3 traceDiffuseVoxelCone(const vec3 from,vec3 direction){
	direction = normalize(direction);
	const float CONE_SPEEAD = 0.325;

	vec4 acc = vec4(0.0f);

	
	// Controls bleeding from close surfaces.
	// Low values look rather bad if using shadow cone tracing.
	// Might be a better choice to use shadow maps and lower this value.
	float dist = 0.1953125;

	//Trace
	while(dist<SQRT2 && acc.a<1){
		
		vec3 c = from + dist * direction;
		c = scaleAndBias(c);
		float l = (1+ CONE_SPEEAD * dist/VOXEL_SIZE); //跨过了多少Voxel
		float level = log2(l);
		float ll = (level+1)*(level+1);
		
		vec4 voxel = textureLod(texture3D,c,min(MIPMAP_HARDCAP,level));
		if(voxel.a>=0)
			acc+=0.075*ll*voxel*pow(1-voxel.a,2);
		dist+= ll*VOXEL_SIZE*2;

	}

	return pow(acc.rgb*2.0,vec3(1.5));
}

// Traces a specular voxel cone.
vec3 traceSpecularVoxelCone(vec3 from, vec3 direction){
	from = (from-u_CameraViewPos)/u_CubeSize;
	direction = normalize(direction);

	const float OFFSET = 8 * VOXEL_SIZE;
	const float STEP = VOXEL_SIZE;

	from += OFFSET * normal;
	
	vec4 acc = vec4(0.0f);
	float dist = OFFSET;

	// Trace.
	while(dist < MAX_DISTANCE && acc.a < 1){ 
		vec3 c = from + dist * direction;
		if(!isInsideCube(c, 0)) break;
		c = scaleAndBias(c); 
		
		float level = 0.1 * u_Material.specularDiffusion * log2(1 + dist / VOXEL_SIZE);
		vec4 voxel = textureLod(texture3D, c, min(level, MIPMAP_HARDCAP));
		if(voxel.a>=0){

			float f = 1 - acc.a;
			acc.rgb += 0.25 * (1 + u_Material.specularDiffusion) * voxel.rgb * voxel.a * f;
			acc.a += 0.25 * voxel.a * f;
		}
		
		dist += STEP * (1.0f + 0.125f * level);
	}
	return 1.0 * pow(u_Material.specularDiffusion + 1, 0.8) * acc.rgb;
}

// Calculates indirect diffuse light using voxel cone tracing.
// The current implementation uses 9 cones. I think 5 cones should be enough, but it might generate
// more aliasing and bad blur.
vec3 indirectDiffuseLight(){
	//MY
	vec3 worldPositionFrag_=(v_FragPos-u_CameraViewPos)/u_CubeSize;

	const float ANGLE_MIX = 0.5f; // Angle mix (1.0f => orthogonal direction, 0.0f => direction of normal).

	const float w[3] = {1.0, 1.0, 1.0}; // Cone weights.

	// Find a base for the side cones with the normal as one of its base vectors.
	const vec3 ortho = normalize(orthogonal(normal));
	const vec3 ortho2 = normalize(cross(ortho, normal));

	// Find base vectors for the corner cones too.
	const vec3 corner = 0.5f * (ortho + ortho2);
	const vec3 corner2 = 0.5f * (ortho - ortho2);

	// Find start position of trace (start with a bit of offset).
	const vec3 N_OFFSET = normal * (1 + 4 * ISQRT2) * VOXEL_SIZE;
	const vec3 C_ORIGIN = worldPositionFrag_ + N_OFFSET;

	// Accumulate indirect diffuse light.
	vec3 acc = vec3(0);

	// We offset forward in normal direction, and backward in cone direction.
	// Backward in cone direction improves GI, and forward direction removes
	// artifacts.
	const float CONE_OFFSET = -0.01;

	// Trace front cone
	acc += w[0] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * normal, normal);

	// Trace 4 side cones.
	const vec3 s1 = mix(normal, ortho, ANGLE_MIX);
	const vec3 s2 = mix(normal, -ortho, ANGLE_MIX);
	const vec3 s3 = mix(normal, ortho2, ANGLE_MIX);
	const vec3 s4 = mix(normal, -ortho2, ANGLE_MIX);

	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho, s1);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho, s2);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho2, s3);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho2, s4);

	// Trace 4 corner cones.
	const vec3 c1 = mix(normal, corner, ANGLE_MIX);
	const vec3 c2 = mix(normal, -corner, ANGLE_MIX);
	const vec3 c3 = mix(normal, corner2, ANGLE_MIX);
	const vec3 c4 = mix(normal, -corner2, ANGLE_MIX);

	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner, c1);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner, c2);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner2, c3);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner2, c4);

	// Return result.
	vec3 diffuseColor=vec3(1,0,0);
	if(u_IsPBRObjects==1)
		diffuseColor=texture(u_Material.diffuse, v_TexCoord).rgb;//pow(texture(u_Material.diffuse, v_TexCoord).rgb, vec3(2.2));
	else if(u_IsPBRObjects==0)
		diffuseColor= u_Material.diffuseColor*(1-u_Settings.isTextureSample)+texture(u_Material.diffuse, v_TexCoord).rgb*u_Settings.isTextureSample;

//	if(acc==vec3(0))
//		return vec4(1,0,0,1);
//	else
//		return diffuseColor;
	//return 3.0*acc * (diffuseColor);//DIFFUSE_INDIRECT_FACTOR * u_Material.diffuseReflectivity * acc * (diffuseColor + vec3(0.001));
	return u_Settings.GICoeffs * u_Material.diffuseReflectivity* acc * (diffuseColor + vec3(0.001));//* acc * 
}

// Calculates indirect specular light using voxel cone tracing.
vec3 indirectSpecularLight(vec3 viewDirection){
	const vec3 reflection = normalize(reflect(viewDirection, normal));
	return u_Settings.GICoeffs * u_Material.specularReflectivity *( u_Material.specularColor*(1-u_IsPBRObjects)+ u_IsPBRObjects*texture(u_Material.diffuse, v_TexCoord).rgb)* traceSpecularVoxelCone(v_FragPos, reflection);
}

// Calculates refractive light using voxel cone tracing.
//vec3 indirectRefractiveLight(vec3 viewDirection){
//	const vec3 refraction = refract(viewDirection, normal, 1.0 / u_Material.refractiveIndex);
//	const vec3 cmix = mix(u_Material.specularColor, 0.5 * (u_Material.specularColor + vec3(1)), u_Material.transparency);
//	return cmix * traceSpecularVoxelCone(v_FragPos, refraction);
//}
//
// Calculates diffuse and specular direct light for a given point light.  
// Uses shadow cone tracing for soft shadows.

vec3 calculateDirectLight_orign(const PointLight light, const vec3 viewDirection){
	vec3 lightDirection = light.position - v_FragPos;
	//MY 
	vec3 ligtdir = lightDirection/u_CubeSize;
	float distToLight = length(ligtdir);

	const float distanceToLight = length(lightDirection);
	lightDirection = lightDirection / distanceToLight;
	const float lightAngle = dot(normal, lightDirection);
	
	// --------------------
	// Diffuse lighting.
	// --------------------
	float diffuseAngle = max(lightAngle, 0.0f); // Lambertian.	
	
	// --------------------
	// Specular lighting.
	// --------------------
#if (SPECULAR_MODE == 0) /* Blinn-Phong. */
	const vec3 halfwayVector = normalize(lightDirection + viewDirection);
	float specularAngle = max(dot(normal, halfwayVector), 0.0f);
#endif
	
#if (SPECULAR_MODE == 1) /* Perfect reflection. */
	const vec3 reflection = normalize(reflect(viewDirection, normal));
	float specularAngle = max(0, dot(reflection, lightDirection));
#endif

	float refractiveAngle = 0;
	if(u_Material.transparency > 0.01){
		vec3 refraction = refract(viewDirection, normal, 1.0 / u_Material.refractiveIndex);
		refractiveAngle = max(0, u_Material.transparency * dot(refraction, lightDirection));
	}

	// --------------------
	// Shadows.
	// --------------------
	float shadowBlend = 1;
#if (SHADOWS == 1)
	if(diffuseAngle * (1.0f - u_Material.transparency) > 0 )
		shadowBlend = traceShadowCone(v_FragPos,light.position);
#endif

	// --------------------
	// Add it all together.
	// --------------------
	diffuseAngle = min(shadowBlend, diffuseAngle);
	specularAngle = min(shadowBlend, max(specularAngle, refractiveAngle));
	const float df = 1.0f / (1.0f + 0.25f * u_Material.specularDiffusion); // Diffusion factor.
	const float specular = SPECULAR_FACTOR * pow(specularAngle, df * SPECULAR_POWER);
	const float diffuse = diffuseAngle * (1.0f - u_Material.transparency);

	const vec3 diff = u_Material.diffuseReflectivity * u_Material.diffuseColor * diffuse;
	const vec3 spec = u_Material.specularReflectivity * u_Material.specularColor * specular;
	const vec3 total = light.diffuse * diff + light.specular* spec;
	return attenuate(distanceToLight) * total;
};




vec3 calculateDirectLight(const PointLight light, const vec3 viewDir){
	vec3 direct = vec3(0.0);

	vec3 lightDir = light.position - v_FragPos;
//	//MY 
//	vec3 ligtdir = lightDirection/u_CubeSize;
//	float distToLight = length(ligtdir);

	const float distanceToLight = length(lightDir);
	lightDir = lightDir / distanceToLight;
	float attenuation = 1.0f/(light.constant+light.linear * distanceToLight+light.quadratic*distanceToLight*distanceToLight);

	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * (u_Material.diffuseColor);

	/*specular*/
	vec3 specular;
	float spec;
	if(u_Settings.isBlinnLight){

		vec3 halfwayDir = normalize(lightDir+viewDir);
		spec = pow(max(dot(norm,halfwayDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  *  u_Material.specularColor;
	}
	else{

		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  *  u_Material.specularColor;//texture(material.specular,v_TexCoords).rgb;
	}
	 vec3 emission = texture(u_Material.emission, v_TexCoord).rgb;


	//ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	emission *= attenuation;

	direct = diffuse + specular + emission;//+ ambient +emission;// * mix(texture(u_Texture1, v_TexCoords), texture(u_Texture2, vec2(1.0 - v_TexCoords.x, v_TexCoords.y)), u_MixValue);
	


//	const float lightAngle = dot(normal, lightDirection);
//	
//	// --------------------
//	// Diffuse lighting.
//	// --------------------
//	float diffuseAngle = max(lightAngle, 0.0f); // Lambertian.	
//	
//	// --------------------
//	// Specular lighting.
//	// --------------------
//#if (SPECULAR_MODE == 0) /* Blinn-Phong. */
//	const vec3 halfwayVector = normalize(lightDirection + viewDirection);
//	float specularAngle = max(dot(normal, halfwayVector), 0.0f);
//#endif
//	
//#if (SPECULAR_MODE == 1) /* Perfect reflection. */
//	const vec3 reflection = normalize(reflect(viewDirection, normal));
//	float specularAngle = max(0, dot(reflection, lightDirection));
//#endif
//
//	float refractiveAngle = 0;
//	if(u_Material.transparency > 0.01){
//		vec3 refraction = refract(viewDirection, normal, 1.0 / u_Material.refractiveIndex);
//		refractiveAngle = max(0, u_Material.transparency * dot(refraction, lightDirection));
//	}
//
//
//
//


	// --------------------
	// Shadows.
	// --------------------

	return direct;
	// --------------------
	// Add it all together.
	// --------------------
//	diffuseAngle = min(shadowBlend, diffuseAngle);
//	specularAngle = min(shadowBlend, max(specularAngle, refractiveAngle));
//	const float df = 1.0f / (1.0f + 0.25f * u_Material.specularDiffusion); // Diffusion factor.
//	const float specular = SPECULAR_FACTOR * pow(specularAngle, df * SPECULAR_POWER);
//	const float diffuse = diffuseAngle * (1.0f - u_Material.transparency);
//
//	const vec3 diff = u_Material.diffuseReflectivity * u_Material.diffuseColor * diffuse;
//	const vec3 spec = u_Material.specularReflectivity * u_Material.specularColor * specular;
//	const vec3 total = light.diffuse * diff + light.specular* spec;
//	return attenuate(distanceToLight) * total;
};


// Sums up all direct light from point lights (both diffuse and specular).
vec3 directLight(vec3 viewDirection){
	vec3 direct = vec3(0.0);
	if(u_IsPBRObjects==0){
		for(int i = 0; i < u_PointLightNums; ++i){
		
			direct += u_PointLights[i].intensity * calculateDirectLight_orign(u_PointLights[i], viewDirection);
		}
	}
	else{
		vec3  albedo	= pow(texture(u_Material.diffuse, v_TexCoord).rgb, vec3(2.2));
		float mentallic = texture(u_Material.mentallic,v_TexCoord).r;
		float roughness = texture(u_Material.roughness ,v_TexCoord).r;
		float ao        = texture(u_Material.ao, v_TexCoord).r;
		vec3  emission	= texture(u_Material.emission,v_TexCoord).rgb;
		vec3  normalMap = getNormalFromMap(v_Normal,v_FragPos,v_TexCoord);

		for(int i = 0; i < u_PointLightNums; ++i){
			float shadowBlend = 1.0;
			#if (SHADOWS == 1)
			shadowBlend = traceShadowCone(v_FragPos,u_PointLights[i].position);
			#endif

			direct +=  shadowBlend*u_PointLights[i].intensity * CalcPBRPointLight(u_PointLights[i],normalMap,albedo,mentallic,roughness,v_FragPos);
			
		}
	}
	



	return direct;
}

void main(){
	color = vec4(0,0,0,1);
	const vec3 viewDirection = normalize(v_FragPos-u_CameraViewPos);

	

	vec3 indirectSpecular = vec3(0.0);
	vec3 indirectDiffuse = vec3(0.0);
	vec3 directLight = vec3(0.0);

	//Indirect diffuse light
	if(u_Settings.directLight)
		directLight= directLight(viewDirection);


	if(u_Settings.indirectDiffuseLight)
		indirectDiffuse = indirectDiffuseLight();

	if(u_Settings.indirectSpecularLight)
		indirectSpecular = indirectSpecularLight(viewDirection);


	if(u_IsPBRObjects==1){
	/* pbr parametrs */
		vec3  albedo	= pow(texture(u_Material.diffuse, v_TexCoord).rgb, vec3(2.2));
		float metallic = texture(u_Material.mentallic,v_TexCoord).r;
		float roughness = texture(u_Material.roughness ,v_TexCoord).r;
		float ao        = texture(u_Material.ao, v_TexCoord).r;
		vec3  emission	= texture(u_Material.emission,v_TexCoord).rgb;
		vec3  normalMap = getNormalFromMap(v_Normal,v_FragPos,v_TexCoord);

		color.rgb =directLight+CalcPBRIndirectLight(indirectSpecular,indirectDiffuse,normalMap,albedo,metallic,roughness,ao,v_FragPos);

	}
	else{
		color.rgb = directLight+(indirectDiffuse+indirectSpecular);

	}
//	//Indirect specular light (glossy reflection)
//	if(u_Settings.indirectSpecularLight && u_Material.specularReflectivity*(1.0f-u_Material.transparency)>0.01f)
//		color.rgb += indirectSpecularLight(viewDirection);

//	//Emissivity
//	color.rgb += u_Material.emissivity * u_Material.diffuseColor;
//
//	//Transparency
//	if(u_Material.transparency>0.01f)
//		color.rgb = color.rgb*(1-u_Material.transparency) + indirectRefractiveLight(viewDirection)*u_Material.transparency;
//
	//Direct light
	
	//HDR tonemapping
#if (HDR == 1)
	 color.rgb = color.rgb / (color.rgb + vec3(1.0));
#endif
	//gamma correction
   //  direct = pow(direct, vec3(1.0/2.2));  
#if (GAMMA_CORRECTION == 1)
	color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
#endif

}