#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

//out vec3 v_FragPos;
//out vec3 v_Normal;
//out vec2 v_TexCoord;

void main(){
//	v_FragPos = vec3(u_Model*vec4(aPos,1));
//	v_Normal = normalize(mat3(transpose(inverse(u_Model)))*aNormal);
//	v_TexCoord=vec2(aTexCoords.x,aTexCoords.y);
//	gl_Position = u_ProjectionView*vec4(v_FragPos,1);

	//v_TexCoord = aTexCoords;
	gl_Position =vec4(aPos,1.0);
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
//#define CONSTANT 1
//#define LINEAR 0 /* Looks meh when using gamma correction. */
//#define QUADRATIC 1

// Other settings.
#define GAMMA_CORRECTION 1 /* Whether to use gamma correction or not. */
//#define HDR 1 /* Whether to use tone mapping or not. */


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


uniform Material u_Material;//material info from gBuffer
uniform Settings u_Settings;

uniform sampler3D texture3D;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse_Roughness;
uniform sampler2D gSpecular_Mentallic;
uniform sampler2D gAmbientGI_AO;
uniform sampler2D gNormalMap;

uniform vec3 u_CameraViewPos;

uniform vec3 u_CubeSize; //m_CubeObj的大小，控制体素化范围
uniform sampler2D u_BrdfLUTMap;
uniform vec2 gScreenSize;
uniform float u_SpecularBlurThreshold;



out vec4 FragColor;


struct GBuffer{
	vec3 fragPos;
	vec3 normal;
	vec3 getNormalFromMap;
	vec3 diffuseColor;
	vec3 specularColor;
	//int isSkyBox;
	int isPBRObject;
	float roughness;
	float metallic ;
	float ao;

}gBuffer;

//vec3 normal = normalize(v_Normal);




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

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}


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
	vec3 environmentIrradiance=indirectDiffuse;// u_ProbeWeight[i]*texture(u_IrradianceMap[i],N).rgb;
		
	vec3 diffuse = environmentIrradiance*albedo;

	//sample both the prefilter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part
	const float MAX_REFLECTION_LOD = 1.0;
	//sample MAX_REFLECTION_LOD level mipmap everytime !
	vec3 prefileredColor = vec3(0.0,0.0,0.0) ;//= vec3(1.0,1.0,1.0);

	/*specular Map只取最近的一个*/
	prefileredColor = indirectSpecular;//textureLod(u_PrefilterMap[0],R,roughness*MAX_REFLECTION_LOD).rgb;

	vec2 brdf = texture(u_BrdfLUTMap,vec2(max(dot(N,V),0.0),roughness)).rg;

	vec3 specular = prefileredColor * (F*brdf.x+brdf.y);
	vec3 ambient =  (Kd*diffuse+specular) * ao;
	return ambient;
}


/**********************************************************************/





vec3 traceDiffuseVoxelCone(const vec3 from,vec3 direction){
	direction = normalize(direction);
	const float CONE_SPEEAD = 0.325;

	vec4 acc = vec4(0.0f);

	
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
vec3 traceSpecularVoxelCone(vec3 from,vec3 normal,vec3 direction,int isPbr,float roughnessPBR){
	float MAX_DISTANCE = distance(vec3(abs(from)), vec3(-1));

	from = (from-u_CameraViewPos)/u_CubeSize;
	direction = normalize(direction);

	const float OFFSET = 2 * VOXEL_SIZE;//8*
	const float STEP = VOXEL_SIZE;

	from += OFFSET * normal;
	
	vec4 acc = vec4(0.0f);
	float dist = OFFSET;

	// Trace.
	while(dist < MAX_DISTANCE && acc.a < 1){ 
		vec3 c = from + dist * direction;
		if(!isInsideCube(c, 0)) break;
		c = scaleAndBias(c); 
		
		float level = 0.1 *( (1- isPbr)* u_Material.specularDiffusion + isPbr * roughnessPBR )* log2(1 + dist / VOXEL_SIZE);//0.1
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
vec3 indirectDiffuseLight(vec3 fragPos,vec3 diffuseColor,vec3 normal){
	//MY
	vec3 worldPositionFrag_=(fragPos-u_CameraViewPos)/u_CubeSize;

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
//	vec3 diffuseColor=vec3(1,0,0);
//	if(u_IsPBRObjects==1)
//		diffuseColor=texture(u_Material.diffuse, v_TexCoord).rgb;//pow(texture(u_Material.diffuse, v_TexCoord).rgb, vec3(2.2));
//	else if(u_IsPBRObjects==0)
//		diffuseColor= u_Material.diffuseColor*(1-u_Settings.isTextureSample)+texture(u_Material.diffuse, v_TexCoord).rgb*u_Settings.isTextureSample;
//


//	if(acc==vec3(0))
//		return vec4(1,0,0,1);
//	else
//		return diffuseColor;
	//return 3.0*acc * (diffuseColor);//DIFFUSE_INDIRECT_FACTOR * u_Material.diffuseReflectivity * acc * (diffuseColor + vec3(0.001));
	return  acc * (diffuseColor + vec3(0.001));//* acc * 
}

// Calculates indirect specular light using voxel cone tracing.
vec3 indirectSpecularLight(vec3 viewDirection,vec3 normal,vec3 fragPos,int isPbr,vec3 specularColor,vec3 albedoPBR,float roughnessPBR){
	const vec3 reflection = normalize(reflect(viewDirection, normal));
	return (specularColor*(1-isPbr)+ isPbr * albedoPBR)* traceSpecularVoxelCone(fragPos,normal,reflection,isPbr,roughnessPBR);
}



void main(){
	FragColor = vec4(0,0,0,1);
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


	vec3 indirectSpecular = vec3(0.0);
	vec3 indirectDiffuse = vec3(0.0);
	//vec3 directLight = vec3(0.0);

	//Indirect diffuse light
//	if(u_Settings.directLight)
//		directLight= directLight(viewDirection);
	vec3 normal =normalize(gBuffer.normal);//gBuffer.isPBRObject*gBuffer.getNormalFromMap+(1-gBuffer.isPBRObject)*
	if(u_Settings.indirectDiffuseLight)
		indirectDiffuse = indirectDiffuseLight(gBuffer.fragPos ,gBuffer.diffuseColor,normal);

	if(u_Settings.indirectSpecularLight)
		indirectSpecular = indirectSpecularLight(viewDirection,normal,gBuffer.fragPos,gBuffer.isPBRObject,gBuffer.specularColor,gBuffer.diffuseColor,gBuffer.roughness);

	if(indirectSpecular.r>u_SpecularBlurThreshold&&indirectSpecular.g>u_SpecularBlurThreshold&&indirectSpecular.b>u_SpecularBlurThreshold)
			FragColor.a = 1;//表示需要filter
		else
			FragColor.a = 0;//表示不需要filter


	if(gBuffer.isPBRObject==1){
	/* pbr parametrs */
		vec3  albedo	= pow(gBuffer.diffuseColor, vec3(2.2));
		float metallic  = gBuffer.metallic;
		float roughness = gBuffer.roughness;
		float ao        = gBuffer.ao;
		vec3  normalMap = gBuffer.getNormalFromMap;
		vec3  fragPos   = gBuffer.fragPos;
		FragColor.rgb =u_Settings.GICoeffs *CalcPBRIndirectLight(indirectSpecular,indirectDiffuse,normalMap,albedo,metallic,roughness,ao,fragPos);
		
	}
	else{
		FragColor.rgb = u_Settings.GICoeffs *(indirectDiffuse+indirectSpecular);

	}

	//HDR tonemapping
//	if (u_Settings.hdr)
//	 FragColor.rgb = FragColor.rgb / (FragColor.rgb + vec3(1.0));
//
//	FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / 2.2));
//#endif

}