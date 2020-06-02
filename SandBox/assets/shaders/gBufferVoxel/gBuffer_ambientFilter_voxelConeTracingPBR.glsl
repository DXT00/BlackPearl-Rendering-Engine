#type vertex
#version 450 core
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
#version 450 core


#define TSQRT2 2.828427
#define SQRT2 1.414213
#define SQRT3 1.732
#define ISQRT2 0.707106
// --------------------------------------
// Light (voxel) cone tracing settings.
// --------------------------------------
#define MIPMAP_HARDCAP 4/* 4.0 4.0fToo high mipmap levels => glitchiness, too low mipmap levels => sharpness. */
#define CORE_SIZE 5
//sigma = 2 ,kernel size = 11
//Guassian calculator: http://dev.theomader.com/gaussian-kernel-calculator/
float kernel11[11]={
	0.0093,
	0.028002,
	0.065984,
	0.121703,
	0.175713,
	0.198596,
	0.175713,
	0.121703,	
	0.065984,
	0.028002,
	0.0093
};
float kernel3_single[9]={

0.06292	,	0.124998,	0.06292 ,
0.124998,	0.248326,	0.124998,
0.06292	,	0.124998,	0.06292	


};
//sigma = 1
float kernel5_single[25]={

0.003765,	0.015019,	0.023792,	0.015019,	0.003765,
0.015019,	0.059912,	0.094907,	0.059912,	0.015019,
0.023792,	0.094907,	0.150342,	0.094907,	0.023792,
0.015019,	0.059912,	0.094907,	0.059912,	0.015019,
0.003765,	0.015019,	0.023792,	0.015019,	0.003765,

};
//sigma = 1.5
float kernel7_single[49]={
0.0015	,	0.00438	,	0.008328,	0.010317,	0.008328,	0.00438	,	0.0015  ,
0.00438	,	0.012788,	0.024314,	0.03012	,	0.024314,	0.012788,	0.00438	,
0.008328,	0.024314,	0.046228,	0.057266,	0.046228,	0.024314,	0.008328,
0.010317,	0.03012	,	0.057266,	0.07094	,	0.057266,	0.03012	,	0.010317,
0.008328,	0.024314,	0.046228,	0.057266,	0.046228,	0.024314,	0.008328,
0.00438	,	0.012788,	0.024314,	0.03012	,	0.024314,	0.012788,	0.00438	,
0.0015	,	0.00438	,	0.008328,	0.010317,	0.008328,	0.00438	,	0.0015	

};


float kernel11_single[121]={

0.000086,	0.00026	,	0.000614,	0.001132,	0.001634,	0.001847,	0.001634,	0.001132,	0.000614,	0.00026	,	0.000086,
0.00026	,	0.000784,	0.001848,	0.003408,	0.00492	,	0.005561,	0.00492	,	0.003408,	0.001848,	0.000784,	0.00026	,
0.000614,	0.001848,	0.004354,	0.00803	,	0.011594,	0.013104,	0.011594,	0.00803	,	0.004354,	0.001848,	0.000614,
0.001132,	0.003408,	0.00803	,	0.014812,	0.021385,	0.02417	,	0.021385,	0.014812,	0.00803	,	0.003408,	0.001132,
0.001634,	0.00492	,	0.011594,	0.021385,	0.030875,	0.034896,	0.030875,	0.021385,	0.011594,	0.00492	,	0.001634,
0.001847,	0.005561,	0.013104,	0.02417	,	0.034896,	0.03944	,	0.034896,	0.02417	,	0.013104,	0.005561,	0.001847,
0.001634,	0.00492	,	0.011594,	0.021385,	0.030875,	0.034896,	0.030875,	0.021385,	0.011594,	0.00492	,	0.001634,
0.001132,	0.003408,	0.00803	,	0.014812,	0.021385,	0.02417	,	0.021385,	0.014812,	0.00803	,	0.003408,	0.001132,
0.000614,	0.001848,	0.004354,	0.00803	,	0.011594,	0.013104,	0.011594,	0.00803	,	0.004354,	0.001848,	0.000614,
0.00026	,	0.000784,	0.001848,	0.003408,	0.00492	,	0.005561,	0.00492	,	0.003408,	0.001848,	0.000784,	0.00026	,
0.000086,	0.00026	,	0.000614,	0.001132,	0.001634,	0.001847,	0.001634,	0.001132,	0.000614,	0.00026	,	0.000086



};

//float kernel7_single[49]={
//0.0015	,	0.00438	,	0.008328,	0.010317,	0.008328,	0.00438	,	0.0015  ,
//0.00438	,	0.012788,	0.024314,	0.03012	,	0.024314,	0.012788,	0.00438	,
//0.008328,	0.024314,	0.046228,	0.057266,	0.046228,	0.024314,	0.008328,
//0.010317,	0.03012	,	0.057266,	0.07094	,	0.057266,	0.03012	,	0.010317,
//0.008328,	0.024314,	0.046228,	0.057266,	0.046228,	0.024314,	0.008328,
//0.00438	,	0.012788,	0.024314,	0.03012	,	0.024314,	0.012788,	0.00438	,
//0.0015	,	0.00438	,	0.008328,	0.010317,	0.008328,	0.00438	,	0.0015	
//
//
//};
//sigma =5 ,kernel size = 31
float kernel31[31]={
	0.0009,
	0.001604,
	0.002748,
	0.004523,
	0.007154,
	0.010873,
	0.01588,
	0.022285,
	0.030051,
	0.038941,
	0.048488,
	0.058016,
	0.066703,
	0.073694,
	0.078235,
	0.07981,
	0.078235,
	0.073694,
	0.066703,
	0.058016,
	0.048488,
	0.038941,
	0.030051,
	0.022285,
	0.01588,
	0.010873,
	0.007154,
	0.004523,
	0.002748,
	0.001604,
	0.0009	
};

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

uniform float u_VoxelDim;
const float VOXEL_SIZE=1.0/u_VoxelDim; /* Size of a voxel. 128x128x128 => 1/128 = 0.0078125. */

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
uniform float u_IndirectSpecularAngle;


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

	const float MAX_REFLECTION_LOD = 1.0;
	vec3 prefileredColor = vec3(0.0,0.0,0.0) ;//= vec3(1.0,1.0,1.0);

	/*specular Map只取最近的一个*/
	prefileredColor = indirectSpecular;//textureLod(u_PrefilterMap[0],R,roughness*MAX_REFLECTION_LOD).rgb;

	vec2 brdf = texture(u_BrdfLUTMap,vec2(max(dot(N,V),0.0),roughness)).rg;

	vec3 specular = prefileredColor * (F*brdf.x+brdf.y);
	vec3 ambient =  (Kd*diffuse+specular) * ao;
	return ambient;
}


/**********************************************************************/


vec3 traceDiffuseVoxelCone(const vec3 from,vec3 direction,float coneAngle,out float occlusion){
    occlusion = 0.0;

	float tanHalfAngle = tan(radians(coneAngle/2.0));
	direction = normalize(direction);
	const float CONE_SPEEAD =0.325;// 0.1;

	vec4 acc = vec4(0.0);
	
	float dist =15.0*VOXEL_SIZE;//提前走多几步，防止采集到高分辨率的体素（这些体素会闪烁），防止闪烁

	//Trace
	while(dist<SQRT2  && acc.a<1){//dist<SQRT2 
		
		vec3 c = from + dist * direction;
		if(!isInsideCube(c, -0.05)) break;
		c = scaleAndBias(c);
		float diameter = max(VOXEL_SIZE,  2.0*tanHalfAngle*dist); 
		int level =int( log2(diameter/VOXEL_SIZE));
		
		vec4 voxel = textureLod(texture3D,c,min(MIPMAP_HARDCAP,level));
		

		float a = 1.0-acc.a;
		
		acc.rgb+=a*voxel.rgb;
		acc.a+=a*voxel.a;
		occlusion += (a * voxel.a) / (1.0 + 0.03 * diameter);
		//acc+=0.075*level*level*voxel*pow(1-voxel.a,2);
		dist+= 0.1* diameter;
		//dist+= level*level*VOXEL_SIZE*2;
	}

	return acc.rgb;//pow(0.8*acc.rgb,vec3(1.5));
}



vec3 traceDiffuseVoxelCone(const vec3 from,vec3 direction){
	direction = normalize(direction);
	const float CONE_SPEEAD =0.325;// 0.1;

	vec4 acc = vec4(0.0);
	
	float dist = 0.1953125;//OFFSET;//0.1953125;

	//Trace
	while(dist<SQRT2 && acc.a<1){//dist<SQRT2 
		
		vec3 c = from + dist * direction;
		if(!isInsideCube(c, 0))  break;
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
vec3 traceSpecularVoxelCone(vec3 from,vec3 normal,vec3 direction,int isPbr,float roughnessPBR,float coneAngle){
	float tanHalfAngle = tan(radians(coneAngle/2.0));

	float MAX_DISTANCE = distance(vec3(abs(from)), vec3(-1));

	from = (from-u_CameraViewPos)/u_CubeSize;
	direction = normalize(direction);

	const float OFFSET = 2 * VOXEL_SIZE;////提前走多几步，防止采集到高分辨率的体素（这些体素会闪烁），防止闪烁
	const float STEP = VOXEL_SIZE;
	float specularFactor = abs(from.z)>0.6?((1.0-abs(from.z))/0.4):1.0; //边界模糊


	from += OFFSET * normal;
	
	vec4 acc = vec4(0.0f);
	float dist = OFFSET;

	// Trace.
	while(dist < MAX_DISTANCE && acc.a < 1){ 
		vec3 c = from + dist * direction;
		if(!isInsideCube(c, 0)) break;
//		c = scaleAndBias(c); 
		float l = max(VOXEL_SIZE,  2.0*tanHalfAngle*dist); 
		float level =  log2(l / VOXEL_SIZE);// ( (1- isPbr)* u_Material.specularDiffusion + isPbr * roughnessPBR )* log2(l / VOXEL_SIZE);//0.1
		vec4 voxel;
		if(u_Settings.guassian_mipmap){
			voxel = vec4(0.0);
			if(abs(c.x)>abs(c.z)&&abs(c.x)>abs(c.y)){//z方向采样
				for(int j=-CORE_SIZE/2;j<=CORE_SIZE/2;j++){
					for(int i=-CORE_SIZE/2;i<=CORE_SIZE/2;i++){
						int x= i+CORE_SIZE/2,y = j+CORE_SIZE/2;

						vec3 c_offset = vec3(c.x,c.y+j*VOXEL_SIZE,c.z+i*VOXEL_SIZE);
						if(!isInsideCube(c_offset, 0)) continue;
						c_offset = scaleAndBias(c_offset); 
						voxel+=textureLod(texture3D, c_offset, min(level, MIPMAP_HARDCAP))*kernel5_single[x+CORE_SIZE*y];
					}
				}
			}
			else if (abs(c.z)>abs(c.x)&&abs(c.z)>abs(c.y)){
				for(int j=-CORE_SIZE/2;j<=CORE_SIZE/2;j++){
					for(int i=-CORE_SIZE/2;i<=CORE_SIZE/2;i++){
						int x= i+CORE_SIZE/2,y = j+CORE_SIZE/2;
						vec3 c_offset = vec3(c.x+i*VOXEL_SIZE,c.y+j*VOXEL_SIZE,c.z);
						if(!isInsideCube(c_offset, 0)) continue;
						c_offset = scaleAndBias(c_offset); 
						voxel+=textureLod(texture3D, c_offset, min(level, MIPMAP_HARDCAP))*kernel5_single[x+CORE_SIZE*y];
					}
				}
			}
			else{
				for(int j=-CORE_SIZE/2;j<=CORE_SIZE/2;j++){
					for(int i=-CORE_SIZE/2;i<=CORE_SIZE/2;i++){
						int x= i+CORE_SIZE/2,y = j+CORE_SIZE/2;

						vec3 c_offset = vec3(c.x+i*VOXEL_SIZE,c.y,c.z+j*VOXEL_SIZE);
						if(!isInsideCube(c_offset, 0)) continue;
						c_offset = scaleAndBias(c_offset); 
						voxel+=textureLod(texture3D, c_offset, min(level, MIPMAP_HARDCAP))*kernel5_single[x+CORE_SIZE*y];
					}
				}
			}
		}//if(u_Settings.guassian_mipmap)
		else{
			c = scaleAndBias(c); 
			voxel = textureLod(texture3D, c,min(level, MIPMAP_HARDCAP) );//min(level, MIPMAP_HARDCAP)

		}


		if(voxel.a>=0){
			float a = 1.0-acc.a;
			acc.rgb+=a*voxel.rgb;
			acc.a+=a*voxel.a;
		
			
//			float f = 1 - acc.a;
//			acc.rgb += 0.25 * (1 + u_Material.specularDiffusion) * voxel.rgb * voxel.a * f;
//			acc.a += 0.25 * voxel.a * f;
		}
		dist+= 0.2* l;
		//dist += STEP * (1.0f + 0.125f * level);
	}
	//return specularFactor* pow(u_Material.specularDiffusion + 1, 0.8) * acc.rgb;
		//return specularFactor* acc.rgb;
				return acc.rgb;

}


vec3 indirectDiffuseLight(vec3 fragPos,vec3 diffuseColor,vec3 normal,out float occlusion){
	//MY
	vec3 worldPositionFrag_=(fragPos-u_CameraViewPos)/u_CubeSize;

	const float ANGLE_MIX = 0.5f; // Angle mix (1.0f => orthogonal direction, 0.0f => direction of normal).

	const float w[3] = {3.0*2.0*PI/9.0, (2.0*PI-2.0*PI*3.0/9.0)/8.0, (2.0*PI-2.0*PI*3.0/9.0)/8.0}; // Cone weights.

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
	occlusion  = 0.0;
	float occ  = 0.0;

	acc += w[0] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * normal, normal,45,occ);
	occlusion+=w[0] * occ;
	// Trace 4 side cones.
	const vec3 s1 = mix(normal, ortho, ANGLE_MIX);
	const vec3 s2 = mix(normal, -ortho, ANGLE_MIX);
	const vec3 s3 = mix(normal, ortho2, ANGLE_MIX);
	const vec3 s4 = mix(normal, -ortho2, ANGLE_MIX);


//	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho, s1,45,occ);
//	occlusion+=w[1] *occ;
//	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho, s2,45,occ);
//	occlusion+=w[1] *occ;
//	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho2, s3,45,occ);
//	occlusion+=w[1] *occ;
//	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho2, s4,45,occ);
//	occlusion+=w[1] *occ;
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * s1, s1,45,occ);
	occlusion+=w[1] *occ;
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * s2, s2,45,occ);
	occlusion+=w[1] *occ;
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * s3, s3,45,occ);
	occlusion+=w[1] *occ;
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * s4, s4,45,occ);
	occlusion+=w[1] *occ;
	// Trace 4 corner cones.
	const vec3 c1 = mix(normal, corner, ANGLE_MIX);
	const vec3 c2 = mix(normal, -corner, ANGLE_MIX);
	const vec3 c3 = mix(normal, corner2, ANGLE_MIX);
	const vec3 c4 = mix(normal, -corner2, ANGLE_MIX);

	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * c1, c1,45,occ);
	occlusion+= w[2] *occ;
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * c2, c2,45,occ);
	occlusion+= w[2] *occ;
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * c3, c3,45,occ);
	occlusion+= w[2] *occ;
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * c4, c4,45,occ);
	occlusion+= w[2] *occ;

	occlusion=1.0-occlusion;
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
	acc.rgb = acc.rgb / (acc.rgb + vec3(1.0));

	acc.rgb = pow(acc.rgb, vec3(1.0 / 2.2));
	if(acc.r>1.0||acc.g>1.0||acc.b>1.0)
	return vec3(1,0,0);
	else
	return acc* (diffuseColor + vec3(0.001));

	//return  acc* (diffuseColor + vec3(0.001));
}

// Calculates indirect specular light using voxel cone tracing.
vec3 indirectSpecularLight(vec3 viewDirection,vec3 normal,vec3 fragPos,int isPbr,vec3 specularColor,vec3 albedoPBR,float roughnessPBR){
	const vec3 reflection = normalize(reflect(viewDirection, normal));
	return (specularColor*(1-isPbr)+ isPbr * albedoPBR)* traceSpecularVoxelCone(fragPos,normal,reflection,isPbr,roughnessPBR,u_IndirectSpecularAngle);
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
	if(u_Settings.indirectDiffuseLight){
	  float occlusion = 0;
		indirectDiffuse = indirectDiffuseLight(gBuffer.fragPos ,gBuffer.diffuseColor,normal,occlusion);
	//	occlusion = min(1.0, 1.5 * occlusion); 
		//indirectDiffuse*=occlusion;
	
		
	}

	if(u_Settings.indirectSpecularLight)
		indirectSpecular = indirectSpecularLight(viewDirection,normal,gBuffer.fragPos,gBuffer.isPBRObject,gBuffer.specularColor,gBuffer.diffuseColor,gBuffer.roughness);



	if(gBuffer.isPBRObject==1){
	/* pbr parametrs */
		vec3  albedo	= pow(gBuffer.diffuseColor, vec3(2.2));
		float metallic  = gBuffer.metallic;
		float roughness = gBuffer.roughness;
		float ao        = gBuffer.ao;
		vec3  normalMap = gBuffer.getNormalFromMap;
		vec3  fragPos   = gBuffer.fragPos;
		FragColor.rgb =u_Settings.GICoeffs*CalcPBRIndirectLight(indirectSpecular,indirectDiffuse,normalMap,albedo,metallic,roughness,ao,fragPos);
		
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