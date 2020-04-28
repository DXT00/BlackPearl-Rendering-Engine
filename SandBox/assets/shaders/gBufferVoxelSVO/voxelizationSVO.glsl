#type vertex
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!
uniform bool u_IsSkybox;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform vec3 u_CubeSize;

out vec3 worldPositionGeom;
out vec3 normalGeom;
out vec3 texCoordGeom;

void main(){
//	if(u_IsSkybox){
//		worldPositionGeom=aPos;//*(u_CubeSize-vec3(0.1));
//		normalGeom = normalize(aNormal);
//		texCoordGeom=aPos;//*(u_CubeSize-vec3(0.1));
//		vec4 pos = u_Projection*mat4(mat3(u_View))*vec4(worldPositionGeom,1.0);//vec4(aPos,1.0);
//
//		gl_Position =pos;// pos.xyww;
//	}
//	else{

	worldPositionGeom = vec3(u_Model * vec4(aPos,1.0));
	normalGeom = normalize(mat3(transpose(inverse(u_Model)))*aNormal);
	//normalGeom=normalize(normalGeom);
	if(u_IsSkybox)
		texCoordGeom=aPos;
	else
		texCoordGeom=vec3(aTexCoords.x,aTexCoords.y,1.0);
	gl_Position = u_ProjectionView * u_Model * vec4(aPos,1.0);//vec4(worldPositionGeom, 1.0);

	//}
}
//
#type geometry
#version 450 core

layout(triangles) in;
layout(triangle_strip,max_vertices = 3) out;

in vec3 worldPositionGeom[];
in vec3 normalGeom[];
in vec3 texCoordGeom[];

out vec3 worldPositionFrag;
out vec3 normalFrag;
out vec3 texCoordFrag;//v_TexCoord;
//flat out int axisIndex;
uniform vec3 u_CameraViewPos;
uniform vec3 u_CubeSize;
//out vec3 normal_worldPositionFrag;

/*几何着色器后会进入光栅化阶段，因此需要把坐标转换的 [-1,1]的裁剪平面！！*/

void main(){

	vec3 p1 = worldPositionGeom[1] - worldPositionGeom[0];
	vec3 p2 = worldPositionGeom[2] - worldPositionGeom[0];
	vec3 p = abs(cross(p1, p2)); 
	vec3 cubePos =vec3(0.0);//u_CameraViewPos

	for(uint i = 0; i < 3; ++i){
		worldPositionFrag =worldPositionGeom[i];
		vec3 normal_worldPositionFrag=(worldPositionGeom[i]-cubePos)/u_CubeSize;
//		if(normal_worldPositionFrag.x>=1||normal_worldPositionFrag.x<=-1||
//		normal_worldPositionFrag.y>=1||normal_worldPositionFrag.y<=-1||
//		normal_worldPositionFrag.z>=1||normal_worldPositionFrag.z<=-1)
//			return;
		normalFrag = normalGeom[i];
		texCoordFrag = vec3(texCoordGeom[i].x,texCoordGeom[i].y,texCoordGeom[i].z);

		if(p.z > p.x && p.z > p.y){
			//axisIndex=0;
		//	gl_Position =gl_in[i].gl_Position;// vec4(normal_worldPositionFrag,1.0);// vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.y, 0, 1);
			 gl_Position =vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.y, normal_worldPositionFrag.z, 1);
		} 
		else if (p.x > p.y && p.x > p.z){
		//	axisIndex=1;
		//	gl_Position = gl_in[i].gl_Position;//vec4(normal_worldPositionFrag,1.0);//vec4(normal_worldPositionFrag.y, normal_worldPositionFrag.z, 0, 1);
			gl_Position =vec4(normal_worldPositionFrag.y, normal_worldPositionFrag.z, normal_worldPositionFrag.x, 1);
		} 
		else {
			//axisIndex=2;
		//	gl_Position = gl_in[i].gl_Position;//vec4(normal_worldPositionFrag,1.0);//vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.z, 0, 1);
			gl_Position =vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.z, normal_worldPositionFrag.y, 1);
		}
		EmitVertex();
	}
    EndPrimitive();
}


#type fragment
#version 450 core

//automic counter
 layout(binding = 3,offset = 0) uniform atomic_uint u_voxelFragCount;
//layout(std430,binding = 3)  buffer voxelFragCount{uint u_voxelFragCount;};

//layout(rgba8, binding = 0) uniform image3D texture3D;
 layout(binding = 0, rgb10_a2ui)uniform uimageBuffer u_voxelPos;
 layout(binding = 1, rgba8 )uniform imageBuffer u_voxelKd;
 layout(binding = 2, rgba16f) uniform imageBuffer u_voxelNrml;
 layout(binding = 4,rgba8ui) uniform uimageBuffer u_debugBuffer;


in vec3 worldPositionFrag;
in vec3 normalFrag;
in vec3 texCoordFrag;
const float PI=3.14159;

struct PointLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float intensity;
	vec3 position;
	float constant;
	float linear;
	float quadratic;

};

uniform Material u_Material;

uniform Settings u_Settings;
//
uniform PointLight u_PointLights[5];
uniform int u_PointLightNums;
uniform vec3 u_CameraViewPos;
//
uniform vec3 u_CubeSize;
uniform bool u_IsSkybox;
uniform int u_IsPBRObjects;
uniform bool u_StoreData;

uniform int u_VoxelSize;

int ulightNum = u_PointLightNums;
int uPBR = u_IsPBRObjects;
bool uStoreData= u_StoreData;
int uIsTextureSample = u_Settings.isTextureSample;

/************************************* PBR fuction ***************************************************/
/*****************************************************************************************************/

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

vec3 FrehNel(float NdotV,vec3 F0){
	return F0+(1.0-F0)*pow(1.0-NdotV,5.0);
}

vec3 BRDF(vec3 Kd,vec3 Ks,vec3 specular,vec3 albedo){
	
	vec3 fLambert = albedo/PI;//texture(u_Material.diffuse, texCoordFrag.xy).rgb/PI;//u_Material.diffuseColor diffuseColor 相当于 albedo
	return Kd * fLambert+  specular;//specular 中已经有Ks(Ks=F)了，不需要再乘以Ks *
}
vec3 LightRadiance(vec3 fragPos,PointLight light){
	float attenuation = calculateAttenuation(light,fragPos);
	vec3 radiance =light.intensity* light.diffuse*attenuation;
	return radiance;
}

//TODO::法线贴图
vec3 getNormalFromMap(vec3 normal,vec3 fragPos)
{
    vec3 tangentNormal =  2.0* texture(u_Material.normal,texCoordFrag.xy).xyz- vec3(1.0);

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(texCoordFrag.xy);
    vec2 st2 = dFdy(texCoordFrag.xy);

    vec3 N   = normalize(normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
///*******************************************************************************************/

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir,vec3 fragPos){
	vec3 fragColor;

	float distance = length(light.position-fragPos);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	//ambient
	vec3 ambient = light.ambient * (u_Material.ambientColor );
	
	//diffuse
	vec3 lightDir = normalize(light.position-fragPos);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * (u_Material.diffuseColor*(1-u_Settings.isTextureSample)+texture(u_Material.diffuse,texCoordFrag.xy).rgb*u_Settings.isTextureSample );

	//specular
	vec3 specular;
	float spec;
	vec3 reflectDir = normalize(reflect(-lightDir,norm));
	spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
	specular =  light.specular * spec  *  u_Material.specularColor;
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	fragColor = diffuse + ambient + specular;

	return fragColor;
}

vec3 scaleAndBias(vec3 p) { return 0.5f * p + vec3(0.5f); }

bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }


void main(){
	vec3 cubePos = vec3(0.0);//u_CameraViewPos
	vec3 viewDir = normalize(u_CameraViewPos-worldPositionFrag);
	vec3 color = vec3(0.0);
	vec3 PBRcolor = vec3(0.0);

	//uint idx = atomicAdd(u_voxelFragCount,1u);
	imageStore(u_debugBuffer,0,uvec4(2u,2u,2u,2u));
	vec3 normalWorldPositionFrag =worldPositionFrag-cubePos;// vec3(worldPositionFrag.x-u_CubePos.x,worldPositionFrag.y-u_CubePos.y,worldPositionFrag.z-u_CubePos.z);
	normalWorldPositionFrag = normalWorldPositionFrag/u_CubeSize;
	if(!isInsideCube(normalWorldPositionFrag, 0.0)) return;

	if(uPBR==0){
//		if(u_IsSkybox){
//			color=texture(u_Material.cube,texCoordFrag).rgb;
//		}
		//else{
			for(int i = 0; i < ulightNum; ++i) 
			{

				vec3 fragColor;

				float distance = length(u_PointLights[i].position-worldPositionFrag);
				float attenuation = 1.0f/(u_PointLights[i].constant+u_PointLights[i].linear * distance+u_PointLights[i].quadratic*distance*distance);
				//ambient
				vec3 ambient =u_PointLights[i].ambient *  (u_Material.ambientColor );
				
				//diffuse
				vec3 lightDir = normalize(u_PointLights[i].position-worldPositionFrag);
				vec3 norm = normalize(normalFrag);
				float diff = max(dot(lightDir,norm),0.0f);
				vec3 diffuse;

				if(uIsTextureSample==0){
					diffuse =  u_PointLights[i].diffuse * diff * (u_Material.diffuseColor );
				}
				else
					diffuse =  u_PointLights[i].diffuse * diff *texture(u_Material.diffuse,texCoordFrag.xy).rgb;
				//diffuse =  u_PointLights[i].diffuse * diff * ((u_Material.diffuseColor )*(1-uIsTextureSample)+ texture(u_Material.diffuse,texCoordFrag.xy).rgb*uIsTextureSample);

			

				//specular
				vec3 specular;
				float spec;


				vec3 reflectDir = normalize(reflect(-lightDir,norm));
				spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
				specular =  u_PointLights[i].specular * spec  *  u_Material.specularColor;
			
				ambient  *= attenuation;
				diffuse  *= attenuation;
				specular *= attenuation;

				fragColor = diffuse + ambient + specular;


				color +=fragColor;//CalcPointLight(u_PointLights[i], normalFrag,viewDir,worldPositionFrag);//vec3(1,0,0);// 
			}//for
		//}//else
	}//if(u_IsPBRObjects==0)
	else {
		vec3 albedo = pow(texture(u_Material.diffuse, texCoordFrag.xy).rgb, vec3(2.2));//vec3(pow( texture(u_Material.diffuse, v_TexCoord).r, (2.2)));

		float mentallic = texture(u_Material.mentallic,texCoordFrag.xy).r;
		float roughness  = texture(u_Material.roughness ,texCoordFrag.xy).r;
		float ao        = texture(u_Material.ao, texCoordFrag.xy).r;
		vec3 emission = texture(u_Material.emission,texCoordFrag.xy).rgb;

		vec3 N = getNormalFromMap(normalFrag,worldPositionFrag);
		vec3 v_FragPos=worldPositionFrag;
		vec3 V = viewDir;

		vec3 F0 = vec3(0.04);
		F0 = mix(F0,albedo,mentallic);
		//reflection equation
		vec3 Lo = vec3(0.0);
		for(int i=0;i<ulightNum;i++){
			vec3 L = normalize(u_PointLights[i].position-v_FragPos);
			vec3 H = normalize(V+L);
			float attenuation = calculateAttenuation(u_PointLights[i],v_FragPos);
			vec3 radiance = u_PointLights[i].diffuse * attenuation;
			float NDF = NoemalDistribution_TrowbridgeReitz_GGX(N,H,roughness);
			float G = GeometrySmith(N, V,L,roughness);
			vec3 F = FrehNel(max(dot(H,V),0.0), F0);

			vec3 Ks = F;
			vec3 Kd = vec3(1.0)-Ks;
			Kd *= (1.0 - mentallic);

			//CookTorrance
			vec3 nominator    = NDF * G * F;
			float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
			vec3 specular     = nominator / denominator;
			
			float NdotL = max(dot(N,L),0.0);
			Lo= Lo+ BRDF(Kd,Ks,specular,albedo)*LightRadiance(v_FragPos,u_PointLights[i])*NdotL;
		}
		vec3 ambient =0.008 * albedo * ao;//vec3(0.03)
		color=Lo+ambient;

	}


	//只有cubeSize里的fragment才会记录到u_voxelFragCount
	uint idx = atomicCounterIncrement(u_voxelFragCount);
	vec3 voxel = scaleAndBias(normalWorldPositionFrag);
	vec4 res = vec4(color,1);
	
	if(uStoreData){
			uvec3 uvoxel_pos = clamp(uvec3(u_VoxelSize * voxel), uvec3(0u), uvec3(u_VoxelSize - 1u));

		imageStore(u_voxelPos, int(idx), uvec4(uvoxel_pos,1));
		imageStore(u_voxelKd,int(idx),res);
		imageStore(u_voxelNrml,int(idx),vec4(normalFrag,0));
	}
}