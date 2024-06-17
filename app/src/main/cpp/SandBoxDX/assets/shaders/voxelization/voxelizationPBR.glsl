#type vertex
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->�����cpu�������ٴ�����!
uniform int u_IsSkybox;
uniform mat4 u_Projection;
uniform mat4 u_View;
//uniform vec3 u_CubeSize;

out vec3 worldPositionGeom;
out vec3 normalGeom;
out vec3 texCoordGeom;

void main(){

	worldPositionGeom = vec3(u_Model * vec4(aPos,1.0));
	normalGeom = normalize(mat3(u_TranInverseModel)*aNormal);
	int uSkyBox = u_IsSkybox;
	if(uSkyBox==1)
		texCoordGeom=aPos;
	else
		texCoordGeom=vec3(aTexCoords.x,aTexCoords.y,1.0);
	gl_Position = u_ProjectionView * u_Model * vec4(aPos,1.0);//vec4(worldPositionGeom, 1.0);

	//}
}

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

/*������ɫ���������դ���׶Σ������Ҫ������ת���� [-1,1]�Ĳü�ƽ�棡��*/

void main(){
//	vec3 normal_pos[3];
//	vec3 enlarge_pos[3];
//
//	normal_pos[0]=(worldPositionGeom[0]-u_CameraViewPos)/u_CubeSize;
//	normal_pos[1]=(worldPositionGeom[1]-u_CameraViewPos)/u_CubeSize;
//	normal_pos[2]=(worldPositionGeom[2]-u_CameraViewPos)/u_CubeSize;
//
//	vec3 p1 = normal_pos[1] - normal_pos[0];
//	vec3 p2 = normal_pos[2] - normal_pos[0];
//	vec3 p = abs(cross(p1, p2)); 
//
//	/* projection */
//	for(uint i=0;i<3;++i){
//
//		if(p.z > p.x && p.z > p.y){
//			//axisIndex=0;
//			enlarge_pos[i] =vec3(normal_pos[i].x, normal_pos[i].y, normal_pos[i].z);
//		} 
//		else if (p.x > p.y && p.x > p.z){
//			//axisIndex=1;
//			enlarge_pos[i] =vec3(normal_pos[i].y, normal_pos[i].z, normal_pos[i].x);
//		} 
//		else {
//			//axisIndex=2;
//			enlarge_pos[i] =vec3(normal_pos[i].x, normal_pos[i].z, normal_pos[i].y);
//		}
//	}
//
//	float pl = 1.4142135637309 / 256.0 ;
//
//	vec3 e0 = vec3( enlarge_pos[1].xy - enlarge_pos[0].xy, 0 );
//	vec3 e1 = vec3( enlarge_pos[2].xy - enlarge_pos[1].xy, 0 );
//	vec3 e2 = vec3( enlarge_pos[0].xy - enlarge_pos[2].xy, 0 );
//	vec3 n0 = cross( e0, vec3(0,0,1) );
//	vec3 n1 = cross( e1, vec3(0,0,1) );
//	vec3 n2 = cross( e2, vec3(0,0,1) );
//
//	//dilate the triangle
//	enlarge_pos[0].xy = enlarge_pos[0].xy + pl*( (e2.xy/dot(e2.xy,n0.xy)) + (e0.xy/dot(e0.xy,n2.xy)) );
//	enlarge_pos[1].xy = enlarge_pos[1].xy + pl*( (e0.xy/dot(e0.xy,n1.xy)) + (e1.xy/dot(e1.xy,n0.xy)) );
//	enlarge_pos[2].xy = enlarge_pos[2].xy + pl*( (e1.xy/dot(e1.xy,n2.xy)) + (e2.xy/dot(e2.xy,n1.xy)) );
//
//


//	for(uint i = 0; i < 3; ++i){
//		worldPositionFrag =worldPositionGeom[i];//enlarge_pos[i];
//
//		normalFrag = normalGeom[i];
//		texCoordFrag = vec3(texCoordGeom[i].x,texCoordGeom[i].y,texCoordGeom[i].z);
//
//	//	if(p.z > p.x && p.z > p.y){
//		//	axisIndex=0;
//		gl_Position =vec4(enlarge_pos[i], 1);
//	//	} 
////		else if (p.x > p.y && p.x > p.z){
////			axisIndex=1;
////			gl_Position =vec4(enlarge_pos[i], 1);
////		} 
////		else {
////			axisIndex=2;
////			gl_Position =vec4(enlarge_pos[i], 1);
////		}
//		EmitVertex();
//	}
//



	vec3 p1 = worldPositionGeom[1] - worldPositionGeom[0];
	vec3 p2 = worldPositionGeom[2] - worldPositionGeom[0];
	vec3 p = abs(cross(p1, p2)); 
//	worldPositionFrag =worldPositionGeom[0];//enlarge_pos[i];
//	normalFrag = normalGeom[0];
//	texCoordFrag = vec3(texCoordGeom[0].x,texCoordGeom[0].y,texCoordGeom[0].z);
//	gl_Position =vec4(vec3(enlarge_pos[0]), 1);
//	EmitVertex();
//
//	worldPositionFrag =worldPositionGeom[1];//enlarge_pos[i];
//	normalFrag = normalGeom[1];
//	texCoordFrag = vec3(texCoordGeom[1].x,texCoordGeom[1].y,texCoordGeom[1].z);
//	gl_Position =vec4(vec3(enlarge_pos[1]), 1);
//	EmitVertex();
//
//	worldPositionFrag =worldPositionGeom[2];//enlarge_pos[i];
//	normalFrag = normalGeom[2];
//	texCoordFrag = vec3(texCoordGeom[2].x,texCoordGeom[2].y,texCoordGeom[2].z);
//	gl_Position =vec4(vec3(enlarge_pos[2]), 1);
//	EmitVertex();
//    EndPrimitive();


	for(uint i = 0; i < 3; ++i){
		worldPositionFrag =worldPositionGeom[i];
		vec3 normal_worldPositionFrag=(worldPositionGeom[i]-u_CameraViewPos)/u_CubeSize;
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

uniform PointLight u_PointLights[5];
uniform int        u_PointLightNums;
uniform vec3       u_CameraViewPos;
uniform vec3       u_CubeSize;
uniform int        u_IsSkybox;
uniform int        u_IsPBRObjects;

int uPointLightsNum = u_PointLightNums;
layout(rgba8, binding = 0) uniform image3D texture3D;
//layout(r32ui, binding = 0) volatile uniform uimage3D texture3D;

in vec3 worldPositionFrag;
in vec3 normalFrag;
in vec3 texCoordFrag;
uint convVec4ToRGBA8( in vec4 val )
{
    return ( uint(val.w)&0x000000FFu)<<24u | (uint(val.z)&0x000000FFu)<<16u | (uint(val.y)&0x000000FFu)<<8u | (uint(val.x)&0x000000FFu);
}
vec4 convRGBA8ToVec4( in uint val )
{
    return vec4( float( (val&0x000000FFu) ), float( (val&0x0000FF00u)>>8u),
	             float( (val&0x00FF0000u)>>16u), float( (val&0xFF000000u)>>24u) );
}

void imageAtomicRGBA8Avg( vec4 val, ivec3 coord, layout(r32ui) uimage3D  buf )
{
    val.rgb *= 255.0;
	val.a = 1;

	uint newVal = convVec4ToRGBA8( val );
	uint prev = 0u;
	uint cur;
	
	
//	uint imageAtomicCompSwap(gimage1D image,
// 	int P,
// 	uint compare,
// 	uint data);
//	

	//imageAtomicCompSwap atomically compares the value of compare with that of the texel at coordinate
	//P and sample (for multisampled forms) in the image bound to uint image. If the values are equal, 
	//data is stored into the texel, otherwise it is discarded.
	//It returns the original value of the texel regardless of the result of the comparison operation.
	while( (cur = imageAtomicCompSwap( buf, coord, prev, newVal ) ) != prev )
   {
       prev = cur;
	   vec4 rval = convRGBA8ToVec4( cur );
	   rval.xyz = rval.xyz*rval.w;
	   vec4 curVal = rval +  val;
	   curVal.xyz /= curVal.w;
	   newVal = convVec4ToRGBA8( curVal );
   }

     
}
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
	
	vec3 fLambert = albedo/PI;//texture(u_Material.diffuse, texCoordFrag.xy).rgb/PI;//u_Material.diffuseColor diffuseColor �൱�� albedo
	return Kd * fLambert+  specular;//specular ���Ѿ���Ks(Ks=F)�ˣ�����Ҫ�ٳ���Ks *
}
vec3 LightRadiance(vec3 fragPos,PointLight light){
	float attenuation = calculateAttenuation(light,fragPos);
	vec3 radiance =light.intensity* light.diffuse*attenuation;
	return radiance;
}


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
/*******************************************************************************************/

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir){
	vec3 fragColor;

	float distance = length(light.position-worldPositionFrag);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	//ambient
	vec3 ambient = light.ambient * (u_Material.ambientColor);
	
	//diffuse
	vec3 lightDir = normalize(light.position-worldPositionFrag);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);

	vec3 diffuse;
	
	int uDiffuseTextureSample = u_Settings.isDiffuseTextureSample;
	if(uDiffuseTextureSample==0)
		diffuse = light.diffuse * diff * (u_Material.diffuseColor);
	else
		diffuse = light.diffuse * diff *texture(u_Material.diffuse,texCoordFrag.xy).rgb;

	//specular
	vec3 specular;
	float spec;
	//if(u_Settings.isBlinnLight){
	int uSpecularTextureSample = u_Settings.isSpecularTextureSample;

	vec3 halfwayDir = normalize(lightDir+viewDir);
	spec = pow(max(dot(norm,halfwayDir),0.0),u_Material.shininess);
	if(uSpecularTextureSample==0)
		specular =  light.specular * spec  *  u_Material.specularColor;
	else
		specular =  light.specular * spec  *  texture(u_Material.specular,texCoordFrag.xy).rgb;

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	fragColor = light.intensity*(diffuse + ambient + specular);

	return fragColor;
}

vec3 scaleAndBias(vec3 p) { return 0.5 * p + vec3(0.5); }

bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }


void main(){
	vec3 viewDir = normalize(u_CameraViewPos-worldPositionFrag);
	vec3 color = vec3(0.0);

	vec3 normalWorldPositionFrag =worldPositionFrag-u_CameraViewPos;// vec3(worldPositionFrag.x-u_CubePos.x,worldPositionFrag.y-u_CubePos.y,worldPositionFrag.z-u_CubePos.z);
	normalWorldPositionFrag = normalWorldPositionFrag/u_CubeSize;
	if(!isInsideCube(normalWorldPositionFrag, 0.0)) return;

	int uPbr = u_IsPBRObjects;
	if(uPbr==0){
	int uSkyBox = u_IsSkybox;
		if(uSkyBox==1){
			color=texture(u_Material.cube,texCoordFrag).rgb;
		}
		else{
			for(int i = 0; i < uPointLightsNum; ++i) 
			{
				color += CalcPointLight(u_PointLights[i], normalFrag,viewDir);
			}
			//color = vec3(1.0,0,0);

		}
	}
	else{
		vec3 albedo = pow(texture(u_Material.diffuse, texCoordFrag.xy).rgb, vec3(2.2));

		float mentallic = texture(u_Material.mentallic,texCoordFrag.xy).r;
		float roughness  = texture(u_Material.roughness ,texCoordFrag.xy).r;
		float ao        = texture(u_Material.ao, texCoordFrag.xy).r;
		
		int  emissionSample = u_Settings.isEmissionTextureSample;
		vec3 emission = vec3(0);//(emissionSample==1)?texture(u_Material.emission,texCoordFrag.xy).rgb:vec3(0);

		vec3 N = getNormalFromMap(normalFrag,worldPositionFrag);
		vec3 v_FragPos=worldPositionFrag;
		vec3 V = viewDir;

		vec3 F0 = vec3(0.04);
		F0 = mix(F0,albedo,mentallic);
		//reflection equation
		vec3 Lo = vec3(0.0);
		for(int i=0;i<uPointLightsNum;i++){
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
			Lo+= BRDF(Kd,Ks,specular,albedo)*LightRadiance(v_FragPos,u_PointLights[i])*NdotL;
		}
		vec3 ambient = vec3(0.008) * albedo * ao;//vec3(0.03)
		color = emission+ambient +  Lo;
	
	}
	
		
	//color = color / (color + vec3(1.0));
	//color = pow(color, vec3(1.0/2.2));  
	vec3 voxel = scaleAndBias(normalWorldPositionFrag);
	ivec3 dim = imageSize(texture3D);// retrieve the dimensions of an image
	vec4 res = vec4(vec3(color), 1);
	//vec3 pos = vec3(voxel.x*float(dim.x),voxel.y*float(dim.y),voxel.z*float(dim.z));
	//vec4 pre = imageLoad(texture3D, ivec3(dim * voxel));//write a single texel into an image;
	//res+=pre;
	//if(pre.r+pre.g+pre.b<res.r+res.g+res.b)
	imageStore(texture3D, ivec3(dim * voxel),res);//write a single texel into an image;
}