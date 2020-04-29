#type vertex
#version 450 core

layout(location = 0) in vec3 aPos;
void main(){
	
	gl_Position = vec4(aPos,1.0);

}
#type fragment
#version 450 core


layout(rgba32f,binding = 0) uniform image2D u_Color;
//layout(rgba8,binding = 1) uniform image2D u_Albedo;
//layout(rgba8_snorm,binding = 2) uniform image2D u_Normal;


uniform layout(binding = 3,r32ui) uimageBuffer u_octreeBuffer;
uniform layout(binding = 4,r32ui) uimageBuffer u_octreeKd;
//uniform layout(binding = 6,rgba16f) imageBuffer u_debugBuffer;
//uniform layout(binding = 6,rgba8ui) uimageBuffer u_debugBuffer;

layout(std430,binding = 5) readonly buffer SobolSSBO{vec2 u_SobolSSBO[];};
const uint STACK_SIZE = 23u;
const float PI=3.14159265359;
#define OFFSET 3.552713678800501e-15
#define ISQRT2 0.707106

#define TWO_PI 6.28318530718f
#define HALF_PI 1.570796327f
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse_Roughness;
uniform sampler2D gSpecular_Mentallic;
uniform sampler2D gAmbientGI_AO;
uniform sampler2D gNormalMap;

uniform Settings u_Settings;
uniform int  u_octreeLevel;
uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_TranInverseModel;
uniform vec3 u_CameraViewPos;
uniform int  u_ViewType;
uniform int  u_SPP;
uniform int  u_ScreenWidth;
uniform int  u_ScreenHeight;
uniform int  u_Bounce;
uniform vec3 u_SunRadiance;
uniform vec3 u_CubeSize;
uniform int  u_VoxelDim;
uniform sampler2D u_BrdfLUTMap;

int uSPP = u_SPP;
bool uDirectLight = u_Settings.directLight;
bool uIndirecLight = u_Settings.indirectDiffuseLight;
bool uIndirectSpecularLight = u_Settings.indirectSpecularLight;
const float SURFACE_OFFSET =(1.0/float(u_VoxelDim));
const float VOXEL_SIZE = 1.0/float(u_VoxelDim);
//uniform int u_voxelDim;
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


out vec4 FragColor;

struct StackItem{
	uint  node;
	float t_max;
}stack[STACK_SIZE];

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
uniform int u_PointLightNums;
uniform PointLight u_PointLights[10];

int  uOctreeLevel=u_octreeLevel;

int  uViewType=u_ViewType;
int uLightNums=u_PointLightNums;
/************************************ PBR function ************************************************/
#define DIST_FACTOR 1.1f /* Distance is multiplied by this when calculating attenuation. */
#define CONSTANT 1
#define LINEAR 0 
#define QUADRATIC 1
float attenuate(float dist){ 
	dist *= DIST_FACTOR;
	return 1.0f / (CONSTANT + LINEAR * dist + QUADRATIC * dist * dist); 
}

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
/***************************************************************************************************/
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
//	if(u_Settings.isBlinnLight){
//
//		vec3 halfwayDir = normalize(lightDir+viewDir);
//		spec = pow(max(dot(norm,halfwayDir),0.0),shininess);
//		specular =  light.specular * min(spec,shadow)  *  specularColor;
//	}
	//else{

		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),shininess);
		specular =  light.specular * min(spec,shadow) * specularColor;//texture(material.specular,v_TexCoords).rgb;
	//}
	// vec3 emission = texture(u_Material.emission, v_TexCoord).rgb;


	//ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	//emission *= attenuation;

	direct = diffuse + specular;//+ ambient +emission;// * mix(texture(u_Texture1, v_TexCoords), texture(u_Texture2, vec2(1.0 - v_TexCoords.x, v_TexCoords.y)), u_MixValue);
	



	return direct;
	
};

/******************************************************************************************************/

vec4 convRGBA8ToVec4( in uint val )
{
    return vec4( float( (val&0x000000FFu) ), float( (val&0x0000FF00u)>>8u),
	             float( (val&0x00FF0000u)>>16u), float( (val&0xFF000000u)>>24u) );
}



vec3 SampleHemishpere(vec2 r,in const float e){
	r.x *= TWO_PI;
	float cos_phi = cos(r.x);
	float sin_phi = sin(r.x);
	float cos_theta = pow((1.0f - r.y), 1.0f/(e + 1.0f));
	float sin_theta = sqrt(1.0f - cos_theta*cos_theta);
	float pu = sin_theta * cos_phi;
	float pv = sin_theta * sin_phi;
	float pw = cos_theta;
	return normalize(vec3(pu, pv, pw)); 

}
float Clamp(float x){
//	if(x>=1) return 1;
//	else if(x<=0) return 0;
//	else
//		return x;
if(x<0)return 0;
else if(x>=0)return 1;
}
bool findLeafPos(vec3 normalPos,out vec3 leafPos){
	vec3 fmin = vec3(0,0,0);
	vec3 fmax = vec3(1,1,1);
	leafPos = vec3(-1,-1,-1);
	float voxelSize = 1;
	uint node = imageLoad(u_octreeBuffer,0).r;
	int subnode=0;

	int childIdx = 0;
	for(int i=0;i<uOctreeLevel-2;i++){
		voxelSize*=0.5;
		if((node & 0x80000000u)==0u){
			//flag = false;
			return false;
		}
		childIdx =int(node & 0x3FFFFFFFu); 

		vec3 offset = vec3(Clamp(normalPos.x-fmin.x-voxelSize),Clamp(normalPos.y-fmin.y-voxelSize),Clamp(normalPos.z-fmin.z-voxelSize));


		subnode = int(offset.x);
		subnode += 4*int(offset.y);
		subnode += 2*int(offset.z);

		childIdx = childIdx+subnode;
		fmin.x += voxelSize * offset.x;
	    fmin.y += voxelSize * offset.y;
	    fmin.z += voxelSize * offset.z;

		node = imageLoad(u_octreeBuffer, childIdx).r;
	}
	//directColor =convRGBA8ToVec4(imageLoad(u_octreeKd,childIdx).r).rgb/255.0;
	//if((node & 0x40000000u) ==0u) return false;

	leafPos = fmin+vec3(voxelSize*0.5);
	return true;
}
int Clamp1(int x){
	if(x>=1) return 1;
	else if(x<=0) return 0;
	else
		return x;
}
bool findLeafPos1(vec3 normalPos){
	uint voxelDim = uint(u_VoxelDim);
	uvec3 umin = uvec3(0,0,0);
	uvec3 umax = uvec3( voxelDim, voxelDim, voxelDim );
	int subnode=0;
	int childIdx = 0;

	uvec3 loc =uvec3(u_VoxelDim*normalPos);// uvec3(normalPos.x*voxelDim,normalPos.y*voxelDim,normalPos.z*voxelDim);

	uint node = imageLoad(u_octreeBuffer,0).r;

	for(int i=0;i<uOctreeLevel-1;i++){//避免匹配不精确，所以只要到u_octreeLevel-1层有节点即可！
		voxelDim/=2u;
		if((node & 0x80000000u)==0u){
			return  false;
			
		}
		childIdx =int(node & 0x3FFFFFFFu); 

		subnode = Clamp1(1+int(loc.x)-int(umin.x)-int(voxelDim));
		subnode += 4*Clamp1(1+int(loc.y)-int(umin.y)-int(voxelDim));
		subnode += 2*Clamp1(1+int(loc.z)-int(umin.z)-int(voxelDim));

		childIdx = childIdx+subnode;
		umin.x += voxelDim * uint(Clamp1(1+int(loc.x)-int(umin.x)-int(voxelDim)));
	    umin.y += voxelDim * uint(Clamp1(1+int(loc.y)-int(umin.y)-int(voxelDim)));
	    umin.z += voxelDim * uint(Clamp1(1+int(loc.z)-int(umin.z)-int(voxelDim)));

		node = imageLoad( u_octreeBuffer, childIdx).r;
	}
//	if((node & 0x80000000u)==0u){
//			return  false;
//			
//		}
	//vec3 tmpPos = vec3(float(umin.x+0.5*float(voxelDim)),float(umin.y+0.5*float(voxelDim)),float(umin.z+0.5*float(voxelDim)));
	// leafPos = tmpPos*1.0/float(u_VoxelDim);
	 return true;
}
vec3 findParentColor(int level,vec3 pos){
	vec3 color=vec3(0);
	uint voxelDim = uint(u_VoxelDim);
	uvec3 umin = uvec3(0,0,0);
	uvec3 umax = uvec3( voxelDim, voxelDim, voxelDim );
	int subnode=0;
	int childIdx = 0;

	uvec3 loc =uvec3(u_VoxelDim*pos);// uvec3(normalPos.x*voxelDim,normalPos.y*voxelDim,normalPos.z*voxelDim);

	uint node = imageLoad(u_octreeBuffer,0).r;

	for(int i=0;i<level;i++){//避免匹配不精确，所以只要到u_octreeLevel-1层有节点即可！
		voxelDim/=2u;
		if((node & 0x80000000u)==0u){
			return color;
			
		}
		childIdx =int(node & 0x3FFFFFFFu); 

		subnode = Clamp1(1+int(loc.x)-int(umin.x)-int(voxelDim));
		subnode += 4*Clamp1(1+int(loc.y)-int(umin.y)-int(voxelDim));
		subnode += 2*Clamp1(1+int(loc.z)-int(umin.z)-int(voxelDim));

		childIdx = childIdx+subnode;
		umin.x += voxelDim * uint(Clamp1(1+int(loc.x)-int(umin.x)-int(voxelDim)));
	    umin.y += voxelDim * uint(Clamp1(1+int(loc.y)-int(umin.y)-int(voxelDim)));
	    umin.z += voxelDim * uint(Clamp1(1+int(loc.z)-int(umin.z)-int(voxelDim)));

		node = imageLoad( u_octreeBuffer, childIdx).r;

	}
		color = convRGBA8ToVec4(imageLoad(u_octreeKd,childIdx).r).rgb/255.0;
		return color;
}
//vec3 AvgColor(uint curNode){
//	if((curNode&0x4FFFFFFF)!=0u)
//		return vec3(0);//孩子节点后面处理
//
//	vec3 res = vec3(0);
//	int flagNum = 0;
//	uint childIdx =( curNode& 0x3fffffffu);//0
//	
//	for(int i=0;i<8;i++){
//		uint node = imageLoad(u_octreeBuffer,int(childIdx+i)).r;
//
//		if((node&0x80000000u)!=0){
//				res+=  convRGBA8ToVec4(imageLoad(u_octreeKd,int(childIdx)).r).rgb/255.0;
//				flagNum++;
//			}
//
//	}
//	
//	
//	res = res/8.0;
//	return res;
//}
#define SQRT2 1.414213

vec3 RayMarch(vec3 o,vec3 d){
	d = normalize(d);
	vec3 accColor = vec3(0);
	float dist = 1.0*VOXEL_SIZE;

	while(dist<SQRT2){
		vec3 pos = o+dist*d;
		float l = (1+ dist/VOXEL_SIZE); ////跨过了多少Voxel
		int level = int(log2(l));
		float ll = (level+1)*(level+1);
		accColor+=findParentColor(uOctreeLevel-level,pos);
		dist+= ll*VOXEL_SIZE*2;
	}

	return accColor;
}
bool RayMarchLeaf(vec3 o,vec3 d,out vec3 o_pos,out vec3 o_color,out vec3 o_normal){
	//o_color = vec3(1,1,0);
	//return true;
	
	d.x = abs(d.x) >= OFFSET ? d.x : (d.x >= 0 ? OFFSET : -OFFSET);
	d.y = abs(d.y) >= OFFSET ? d.y : (d.y >= 0 ? OFFSET : -OFFSET);
	d.z = abs(d.z) >= OFFSET ? d.z : (d.z >= 0 ? OFFSET : -OFFSET);

	// Precompute the coefficients of tx(x), ty(y), and tz(z).
	// The octree is assumed to reside at coordinates [1, 2].
	vec3 resColor = vec3(0.0);
	vec3 t_coef = 1.0f / -abs(d);
	vec3 t_bias = t_coef * o;

	uint oct_mask = 0u;
	
	//要改
	if(d.x>0.0f) oct_mask^=1u, t_bias.x = 3.0f * t_coef.x - t_bias.x;
	if(d.z>0.0f) oct_mask^=2u, t_bias.z = 3.0f * t_coef.z - t_bias.z;
	if(d.y>0.0f) oct_mask^=4u, t_bias.y = 3.0f * t_coef.y - t_bias.y; 

	float t_min = max(max(2.0f * t_coef.x - t_bias.x,2.0f * t_coef.y - t_bias.y),2.0f * t_coef.z - t_bias.z);
	float t_max = max(max(1.0f * t_coef.x - t_bias.x,1.0f * t_coef.y - t_bias.y),1.0f *t_coef.z - t_bias.z);
	t_min = max(t_min,0.0f);
	float h = t_max;

	uint parent = imageLoad(u_octreeBuffer,0).r;//root = 0
	if((parent & 0x80000000u) ==0u)return false;
	parent &=0x3fffffffu;
	resColor+=  1.0/float(u_VoxelDim)*convRGBA8ToVec4(imageLoad(u_octreeKd,int(parent)).r).rgb/255.0;//(1.0/float(u_VoxelDim))*

	uint cur = 0u;
	vec3 pos = vec3(1.0f);
	uint idx = 0u;
	
	int leafIdx;

	if(1.5f * t_coef.x - t_bias.x > t_min) idx ^= 1u, pos.x = 1.5f;
	if(1.5f * t_coef.z - t_bias.z > t_min) idx ^= 2u, pos.z = 1.5f;
	if(1.5f * t_coef.y - t_bias.y > t_min) idx ^= 4u, pos.y = 1.5f;
	
	uint scale = STACK_SIZE-1u;
	uint scale_flag;
	float scale_exp2 = 0.5f;
	while(scale<STACK_SIZE){
		//++iter;
		if(cur==0u) 
			cur = imageLoad(u_octreeBuffer,int(parent+(idx^oct_mask))).r;  
			///+ 运算符是优先于 ^ 的！
			//cur = imageLoad(u_octreeBuffer,int(parent+idx^oct_mask)).r;
		vec3 t_corner = pos*t_coef-t_bias;
		float tc_max = min(min(t_corner.x,t_corner.y),t_corner.z);
		//imageStore(u_debugBuffer,int(gl_FragCoord.y*540+gl_FragCoord.x),uvec4(tc_max,t_max,0u,0u));

		if((cur&0x80000000u)!=0u && t_min<=t_max){
			//INTERSECT

			//resColor+=(1.0/scale_exp2/128.0* AvgColor(cur));
			resColor+= (1.0/scale_exp2/float(u_VoxelDim))* convRGBA8ToVec4(imageLoad(u_octreeKd,int(parent+(idx^oct_mask))).r).rgb/255.0;

			float tv_max = min(t_max,tc_max);
			float half_scale_exp2 = scale_exp2*0.5f;
			vec3 t_center = half_scale_exp2*t_coef+ t_corner;

			if(t_min <= tv_max){
				if((cur & 0x40000000u)!=0u){
					//leaf node
					leafIdx = int(parent+(idx^oct_mask));
					//leafIdx = int(parent+idx^oct_mask);
					break;
				}
					

				//PUSH 
				if(tc_max<h){
					stack[scale].node = parent;
					stack[scale].t_max = t_max;
					
				}
				h = tc_max;

				parent = (cur& 0x3fffffffu);

				idx = 0u;
				-- scale;
				scale_flag = scale;
				scale_exp2 = half_scale_exp2;

				if(t_center.x > t_min) idx ^= 1u, pos.x += scale_exp2;
				if(t_center.z > t_min) idx ^= 2u, pos.z += scale_exp2;
				if(t_center.y > t_min) idx ^= 4u, pos.y += scale_exp2;

				cur = 0u;
				t_max = tv_max;

				continue;
			}//if(t_min<tv_max)

		}//if((cur&0x80000000u)!=0u && t_min<=t_max)

		//ADVANCE
		uint step_mask = 0u;
		if(t_corner.x <= tc_max) step_mask ^= 1u, pos.x -= scale_exp2;
		if(t_corner.z <= tc_max) step_mask ^= 2u, pos.z -= scale_exp2;
		if(t_corner.y <= tc_max) step_mask ^= 4u, pos.y -= scale_exp2;

		// Update active t-span and flip bits of the child slot index.
		t_min = tc_max;
		idx ^= step_mask;

		// Proceed with pop if the bit flips disagree with the ray direction.
		if( (idx & step_mask) != 0u ) {
			// POP
			// Find the highest differing bit between the two positions.
			uint differing_bits = 0u;
			if ((step_mask & 1u) != 0u) differing_bits |= floatBitsToUint(pos.x) ^ floatBitsToUint(pos.x + scale_exp2);
			if ((step_mask & 2u) != 0u) differing_bits |= floatBitsToUint(pos.z) ^ floatBitsToUint(pos.z + scale_exp2);
			if ((step_mask & 4u) != 0u) differing_bits |= floatBitsToUint(pos.y) ^ floatBitsToUint(pos.y + scale_exp2);
			scale = findMSB(differing_bits);
			scale_exp2 = uintBitsToFloat((scale - (STACK_SIZE) + 127u) << 23u); // exp2f(scale - s_max)

			// Restore parent voxel from the stack.
			parent = stack[scale].node;
			t_max  = stack[scale].t_max;

			// Round cube position and extract child slot index.
			uint shx = floatBitsToUint(pos.x) >> scale;
			uint shy = floatBitsToUint(pos.y) >> scale;
			uint shz = floatBitsToUint(pos.z) >> scale;
			pos.x = uintBitsToFloat(shx << scale);
			pos.y = uintBitsToFloat(shy << scale);
			pos.z = uintBitsToFloat(shz << scale);
			idx  = (shx & 1u) | ((shz & 1u) << 1u) | ((shy & 1u) << 2u);

			// Prevent same parent from being stored again and invalidate cached child descriptor.
			h = 0.0f;
			cur = 0u;
		}
	}//while(scale<STACK_SIZE)

	vec3 norm,t_corner = t_coef*(pos+scale_exp2)-t_bias;
	if(t_corner.x>t_corner.y && t_corner.x>t_corner.z)
		norm = vec3(-1,0,0);
	else if(t_corner.y>t_corner.z)
		norm = vec3(0,-1,0);
	else
		norm = vec3(0,0,-1);
	if((oct_mask & 1u)==0u ) norm.x = -norm.x;
	if((oct_mask & 2u)==0u ) norm.z = -norm.z;
	if((oct_mask & 4u)==0u ) norm.y = -norm.y;


	// Undo mirroring of the coordinate system.
	if ((oct_mask & 1u) != 0u) pos.x = 3.0f - scale_exp2 - pos.x;
	if ((oct_mask & 2u) != 0u) pos.z = 3.0f - scale_exp2 - pos.z;
	if ((oct_mask & 4u) != 0u) pos.y = 3.0f - scale_exp2 - pos.y;

	//Output result
	o_pos = clamp(o+t_min*d,pos,pos+scale_exp2);
	if(norm.x != 0) o_pos.x = norm.x > 0 ? pos.x + scale_exp2 + OFFSET*2 : pos.x - OFFSET;
	if(norm.y != 0) o_pos.y = norm.y > 0 ? pos.y + scale_exp2 + OFFSET*2 : pos.y - OFFSET;
	if(norm.z != 0) o_pos.z = norm.z > 0 ? pos.z + scale_exp2 + OFFSET*2 : pos.z - OFFSET;

	o_normal = norm;
	uint color =imageLoad(u_octreeKd,leafIdx).r;
	const float CONE_SPEEAD = 1.0;//0.325;
	float dist = length(o_pos-o);
	float voxelNum =(1+ CONE_SPEEAD*dist/VOXEL_SIZE);
	int level = int(log2(voxelNum));
	level = uOctreeLevel-level;
	if(level >=uOctreeLevel-1)
		resColor+=convRGBA8ToVec4(color).rgb/255.0;
	else
		resColor+=findParentColor(level,o_pos-vec3(1.0));

	o_color = resColor;

	return scale<STACK_SIZE && t_min<=t_max;

}

vec2 CalcTexCoord()
{
    return vec2(gl_FragCoord.xy) / vec2(u_ScreenWidth,u_ScreenHeight);
}
vec3 GenRay(){
	vec2 texcoord = vec2(gl_FragCoord.xy)/vec2(u_ScreenWidth,u_ScreenHeight);
	texcoord = 2.0*texcoord - 1.0;
	return normalize(  mat3(inverse(u_View))*( inverse(u_Projection) * vec4(texcoord, 1, 1) ).xyz  );
}
//半球坐标转换到fragment坐标系
vec3 AlignDirection(in const vec3 dir,in const vec3 target){
	vec3 u = normalize(cross(abs(target.x)>0.01?vec3(0,1,0):vec3(1,0,0) ,target ));
	vec3 v = cross(target,u);
	return dir.x*u+dir.y*v+dir.z*target;
}
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }
vec3 CalcluateDirectLight(GBuffer gBuffer){
	vec3 direct = vec3(0.0);
	const vec3 viewDirection = normalize(gBuffer.fragPos-u_CameraViewPos);

	if(gBuffer.isPBRObject == 0){
		
		for(int i=0;i<uLightNums;i++)
			direct += u_PointLights[i].intensity * calculateDirectLight(1,gBuffer.fragPos,gBuffer.normal,u_PointLights[i], viewDirection,gBuffer.diffuseColor,gBuffer.specularColor);
			//direct = vec3(1,0,0);
	}
	else{
			vec3  albedo	= pow(gBuffer.diffuseColor, vec3(2.2));
			float metallic  = gBuffer.metallic;
			float roughness = gBuffer.roughness;
			float ao        = gBuffer.ao;
			vec3  normalMap = gBuffer.getNormalFromMap;
			vec3  fragPos   = gBuffer.fragPos;
			
			for(int i=0;i<uLightNums;i++)
				direct += u_PointLights[i].intensity * CalcPBRPointLight(u_PointLights[i],normalMap,albedo,metallic,roughness,fragPos);
		
		}
	return direct;
}
// Returns a vector that is orthogonal to u.
vec3 orthogonal(vec3 u){
	u = normalize(u);
	vec3 v = vec3(0.99146, 0.11664, 0.05832); // Pick any normalized vector.
	return abs(dot(u, v)) > 0.99999f ? cross(u, vec3(0, 1, 0)) : cross(u, v);
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
		


	vec3 diffuse = environmentIrradiance*albedo;
	/////u_Material.diffuseColor;

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
void main(){
	

	/* calculate direct light */
	vec2 texCoords = CalcTexCoord();
	vec3 radiance = vec3(0.0);
	

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

	ivec2 pixelCoord = ivec2(gl_FragCoord.xy);
	pixelCoord.y = u_ScreenHeight - 1 -pixelCoord.y;

	/* calculate direct light */
	vec3 direct = vec3(0.0);


		if(uDirectLight){
			direct = CalcluateDirectLight(gBuffer);
			radiance+=direct;

		}


		/* calculate indirect light */
		vec3 radiance1 = vec3(0.0);//bounce one
		vec3 radiance2 = vec3(0.0);//bounce two
		

		vec3 normalFragPos = (gBuffer.fragPos-vec3(0.0))/u_CubeSize; //只绘制CubeSize范围内的物体
		if(!isInsideCube(normalFragPos, 0.0)) return;
		vec3 voxelPos =  0.5f * normalFragPos + vec3(0.5f);
		voxelPos = clamp(voxelPos,0,1);
		vec3 octreePos;
		int hitNumber = 0;
		vec3 d_color = vec3(0.0);
		vec3 indirectSpecular = vec3(0.0);

		if(findLeafPos1(voxelPos)){
		//if(direct!=vec3(0)){
			//octreePos+=vec3(1.0);
			octreePos = voxelPos;//+vec3(1.0);

			//imageStore(u_debugBuffer,int(gl_FragCoord.x),uvec4(10*octreePos.x,10*octreePos.y,10*octreePos.z,1u));
			//vec3 direct_radiance = gBuffer.diffuseColor;//vec3(1);

			float attenuation1 =(2.0*PI-2.0*PI*5.0/9.0)/8.0;// 0.3;//0.3;
			float attenuation2 = 0.5*attenuation1;

			/* calculate indirect diffuse light */
			if(uIndirecLight){
				vec3 pos,albedo,normal;

				vec3 o ;
				vec3 d ;
				float nrSamples1 = 0.0;
				float nrSamples2 = 0.0;

				//o = normalFragPos+vec3(1.0)+SURFACE_OFFSET*normalize(gBuffer.normal);//octreePos;
				
				
				const vec3 ortho = normalize(orthogonal(gBuffer.normal));
				const vec3 ortho2 = normalize(cross(ortho, gBuffer.normal));

				// Find base vectors for the corner cones too.
				const vec3 corner = 0.5f * (ortho + ortho2);
				const vec3 corner2 = 0.5f * (ortho - ortho2);

				// Find start position of trace (start with a bit of offset).
				const vec3 N_OFFSET = gBuffer.normal * (1 + 4 * ISQRT2) * u_VoxelDim;
				const vec3 C_ORIGIN = octreePos +gBuffer.normal *  SURFACE_OFFSET;//N_OFFSET;

				// Accumulate indirect diffuse light.
				vec3 acc = vec3(0);

				// We offset forward in normal direction, and backward in cone direction.
				// Backward in cone direction improves GI, and forward direction removes
				// artifacts.
				const float CONE_OFFSET =0;// -SURFACE_OFFSET;//-0.01;//0;//

				o= C_ORIGIN;
		//		d = gBuffer.normal;
				vec3 o1,o2,o3,o4,o5,o6,o7,o8;
				vec3 d1,d2,d3,d4,d5,d6,d7,d8;





				const float ANGLE_MIX = 0.5f;
				o1=C_ORIGIN + CONE_OFFSET * ortho;  d1=mix(gBuffer.normal, ortho, ANGLE_MIX);
				o2=C_ORIGIN - CONE_OFFSET * ortho;  d2=mix(gBuffer.normal, -ortho, ANGLE_MIX);
				o3=C_ORIGIN + CONE_OFFSET * ortho2; d3=mix(gBuffer.normal, ortho2, ANGLE_MIX);
				o4=C_ORIGIN - CONE_OFFSET * ortho2; d4=mix(gBuffer.normal, -ortho2, ANGLE_MIX);

				o5=C_ORIGIN + CONE_OFFSET * corner;  d5=mix(gBuffer.normal, corner, ANGLE_MIX);
				o6=C_ORIGIN - CONE_OFFSET * corner;  d6=mix(gBuffer.normal, -corner, ANGLE_MIX);
				o7=C_ORIGIN + CONE_OFFSET * corner2; d7=mix(gBuffer.normal, corner2, ANGLE_MIX);
				o8=C_ORIGIN - CONE_OFFSET * corner2; d8=mix(gBuffer.normal, -corner2, ANGLE_MIX);

//
//				int hitnumber = 0;
//				if(RayMarchLeaf(o,d,pos,albedo,normal)){
//					d_color+= (albedo* attenuation1);hitnumber++;}
//				if(RayMarchLeaf(o1,d1,pos,albedo,normal)){		
//					d_color+= (albedo* attenuation1);hitnumber++;}
//				if(RayMarchLeaf(o2,d2,pos,albedo,normal)){								
//					d_color+= (albedo* attenuation1);hitnumber++;}
//				if(RayMarchLeaf(o3,d3,pos,albedo,normal)){								
//					d_color+= (albedo* attenuation1);hitnumber++;}
//				if(RayMarchLeaf(o4,d4,pos,albedo,normal)){								
//					d_color+= (albedo* attenuation1);hitnumber++;}
//				if(RayMarchLeaf(o5,d5,pos,albedo,normal)){								
//					d_color+= (albedo* attenuation1);hitnumber++;}
//				if(RayMarchLeaf(o6,d6,pos,albedo,normal)){								
//					d_color+= (albedo* attenuation1);hitnumber++;}
//				if(RayMarchLeaf(o7,d7,pos,albedo,normal)){								
//					d_color+= (albedo* attenuation1);hitnumber++;}
//				if(RayMarchLeaf(o8,d8,pos,albedo,normal)){								
//					d_color+= (albedo* attenuation1);hitnumber++;}
////
//				if(RayMarchLeaf(o,d,pos,albedo,normal))
//					d_color+= (albedo* 10.0*PI/9.0);
//				if(RayMarchLeaf(o1,d1,pos,albedo,normal))
//				d_color+= (albedo* attenuation1);
//				if(RayMarchLeaf(o2,d2,pos,albedo,normal))					
//				d_color+= (albedo* attenuation1);
//				if(RayMarchLeaf(o3,d3,pos,albedo,normal))					
//				d_color+= (albedo* attenuation1);
//				if(RayMarchLeaf(o4,d4,pos,albedo,normal))						
//				d_color+= (albedo* attenuation1);
//				if(RayMarchLeaf(o5,d5,pos,albedo,normal))						
//				d_color+= (albedo* attenuation1);
//				if(RayMarchLeaf(o6,d6,pos,albedo,normal))						
//				d_color+= (albedo* attenuation1);
//				if(RayMarchLeaf(o7,d7,pos,albedo,normal))						
//				d_color+= (albedo* attenuation1);
//				if(RayMarchLeaf(o8,d8,pos,albedo,normal))							
//				d_color+= (albedo* attenuation1);

				
				d_color+= (RayMarch(o,d)* 10.0*PI/9.0);
				d_color+= (RayMarch(o1,d1)* attenuation1);
				d_color+= (RayMarch(o2,d2)* attenuation1);
				d_color+= (RayMarch(o3,d3)* attenuation1);
				d_color+= (RayMarch(o4,d4)* attenuation1);
				d_color+= (RayMarch(o5,d5)* attenuation1);
				d_color+= (RayMarch(o6,d6)* attenuation1);
				d_color+= (RayMarch(o7,d7)* attenuation1);
				d_color+= (RayMarch(o8,d8)* attenuation1);
				d_color/=9.0;
				// trace indirect specular light



			}


			if(uIndirectSpecularLight){
				vec3 pos,albedo,normal;
				vec3 o;
				vec3 d;
				vec3 viewDirection = normalize(gBuffer.fragPos-u_CameraViewPos);
				const vec3 reflection = normalize(reflect(viewDirection, gBuffer.normal));
				if(RayMarchLeaf(o,reflection,pos,albedo,normal)){
				if(gBuffer.isPBRObject==0)
					indirectSpecular+=gBuffer.specularColor*(albedo* attenuation1);
				else
					indirectSpecular+=gBuffer.diffuseColor*(albedo* attenuation1);

			}

		}
		
			
	}//if(findLeafPos(voxelPos,octreePos))
		

	
	if(gBuffer.isPBRObject==0)
		radiance+=u_Settings.GICoeffs*(d_color);
	else
		radiance+= u_Settings.GICoeffs*CalcPBRIndirectLight(indirectSpecular,d_color,gBuffer.getNormalFromMap,gBuffer.diffuseColor,gBuffer.metallic,gBuffer.roughness,gBuffer.ao,gBuffer.fragPos);


	
			

	

	FragColor = vec4(pow(radiance, vec3(1.0f / 2.2f)), 1);




	
}//main()





			//for(float phi=0.0; phi< TWO_PI;phi+=sampleStep){
			//for(float theta=0.0;theta<HALF_PI;theta+=sampleStep){
				
				//spherical to catesian coordinate
				//vec3 sphereCoords = vec3(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
				// tangent space to world
	//			d = AlignDirection(SampleHemishpere(fract(u_SobolSSBO[0]),0.0f),gBuffer.normal);;//AlignDirection(normalize(sphereCoords),normalize(gBuffer.normal));
				//d = normalize(d);
//				if(RayMarchLeaf(o,d,pos,albedo,normal)){
					//nrSamples1++;
					//radiance1+= (albedo* attenuation1)*sin(theta)*cos(theta);
//					d_color+= (albedo* attenuation1);//*cos(theta);

					//bounce 2
//					vec3 d_color2 = vec3(0.0);
//					for(float phi2=0.0; phi2< 2.0*PI;phi2+=sampleStep){
//					for(float theta2=0.0;theta2<0.5*PI;theta2+=sampleStep){
//						//nrSamples2++;
//						vec3 sphereCoords = vec3(sin(theta2)*cos(phi2),sin(theta2)*sin(phi2),cos(theta2));
//						// tangent space to world
//						vec3 normal2 = normal;
//						vec3 d2 = AlignDirection(sphereCoords,normalize(normal2));
//						vec3 o2= pos+SURFACE_OFFSET*normal2;
//						vec3 pos2;
//						if(RayMarchLeaf(o2,d2,pos2,albedo,normal2)){
//							nrSamples2++;
//							d_color2+= (albedo* attenuation2)*cos(theta2);
//						}
//
//					}
//					}
//					d_color2 = d_color2/nrSamples2;
//					d_color+=d_color2;

				//}//	if(RayMarchLeaf(o,d,pos,albedo,normal))

		

			//}//for phi
			//}//for theta
			//d_color/=nrSamples1;

//			radiance1=d_color;
			//radiance2*=1.0/float(nrSamples2);
