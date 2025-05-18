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

	worldPositionGeom = vec3(u_Model * vec4(aPos,1.0));
	normalGeom = aNormal;
	bool isSkybox = u_IsSkybox;
	if(isSkybox)
		texCoordGeom=aPos;
	else
	texCoordGeom=vec3(aTexCoords.xy,1);
	gl_Position = vec4(aPos, 1.0f);//vec4(worldPositionGeom, 1.0);


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
out vec3 voxelPosFrag;
//flat out int axisIndex;
uniform vec3 u_CameraViewPos;
uniform int u_VoxelSize;
//out vec3 normal_worldPositionFrag;

/*几何着色器后会进入光栅化阶段，因此需要把坐标转换的 [-1,1]的裁剪平面！！*/
vec2 Project(in vec3 v, in int axis) { return axis == 0 ? v.yz : (axis == 1 ? v.xz : v.xy); }

void main(){

	vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz ;
	vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz ;
	vec3 p = abs(cross(p1, p2)); 

	int axis = 2;
	if(p.x >= p.y && p.x > p.z) axis = 0;
	else if(p.y >= p.z && p.y > p.x) axis = 1;


	//project the positions
	vec3 pos0 = gl_in[0].gl_Position.xyz;
	vec3 pos1 = gl_in[1].gl_Position.xyz;
	vec3 pos2 = gl_in[2].gl_Position.xyz;


	vec3 cubePos =vec3(0.0);//u_CameraViewPos

	
	texCoordFrag = texCoordGeom[0];
	normalFrag = normalize(normalGeom[0]);
	voxelPosFrag = (pos0 + 1.0f) * 0.5f * u_VoxelSize; //[-1,1]-->[0,1]-->[0,u_VoxelSize-1]
	gl_Position = vec4(Project(pos0, axis), 1.0f, 1.0f);
	EmitVertex();

	texCoordFrag = texCoordGeom[1];
	normalFrag = normalize(normalGeom[1]);
	voxelPosFrag = (pos1 + 1.0f) * 0.5f * u_VoxelSize;
	gl_Position = vec4(Project(pos1, axis), 1.0f, 1.0f);
	EmitVertex();

	texCoordFrag = texCoordGeom[2];
	normalFrag = normalize(normalGeom[2]);
	voxelPosFrag = (pos2 + 1.0f) * 0.5f * u_VoxelSize;
	gl_Position = vec4(Project(pos2, axis), 1.0f, 1.0f);
	EmitVertex();

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
in vec3 texCoordFrag;//v_TexCoord;
in vec3 voxelPosFrag;

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
//	if(u_Settings.isBlinnLight){
//
//		vec3 halfwayDir = normalize(lightDir+viewDir);
//		spec = pow(max(dot(norm,halfwayDir),0.0),u_Material.shininess);
//		specular =  light.specular * spec  *  u_Material.specularColor;
//	}
	//else{

		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  *  u_Material.specularColor;
	//}
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
//	vec3 normalWorldPositionFrag =worldPositionFrag-cubePos;// vec3(worldPositionFrag.x-u_CubePos.x,worldPositionFrag.y-u_CubePos.y,worldPositionFrag.z-u_CubePos.z);
//	normalWorldPositionFrag = normalWorldPositionFrag/u_CubeSize;
//	if(!isInsideCube(normalWorldPositionFrag, 0.0)) return;
	int pbr = u_IsPBRObjects;
	if(pbr==0){
		if(u_IsSkybox){
			color=texture(u_Material.cube,texCoordFrag).rgb;
		}
		else{
			for(int i = 0; i < u_PointLightNums; ++i) 
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
				vec3 diffuse ;
				//if(u_Settings.isTextureSample==0){
					diffuse =  u_PointLights[i].diffuse * diff * (u_Material.diffuseColor );

				//}
//				else{
//					diffuse = diff * texture(u_Material.diffuse,texCoordFrag.xy).rgb;
//				}

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
		}//else
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
		for(int i=0;i<u_PointLightNums;i++){
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
		PBRcolor=Lo+ambient;
	//	color =vec3( 0);//ambient ;//+  Lo;
//		color.x = PBRcolor.x;
//		color.y = PBRcolor.y;
//		color.z = PBRcolor.z;
	}


	//只有cubeSize里的fragment才会记录到u_voxelFragCount
	uint idx = atomicCounterIncrement(u_voxelFragCount);
	//vec3 voxel = scaleAndBias(normalWorldPositionFrag);
	//float dim = 128;//128;//256;//imageSize(texture3D);// retrieve the dimensions of an image
	vec4 res ;

	//if(pbr==0)
		res = vec4(vec3(color), 0);
	//else
		//res = vec4(vec3(PBRcolor), 0);

	if(u_StoreData){
		uvec3 uvoxel_pos = clamp(uvec3(voxelPosFrag), uvec3(0u), uvec3(u_VoxelSize - 1u));
		imageStore(u_voxelPos, int(idx), uvec4(uvoxel_pos,1));
		imageStore(u_voxelKd,int(idx),res);
		imageStore(u_voxelNrml,int(idx),vec4(normalFrag,0));
	}
}