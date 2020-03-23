#type vertex
#version 430 core

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

#type geometry
#version 430 core

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
#version 430 core

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
uniform int u_PointLightNums;
uniform vec3 u_CameraViewPos;
uniform vec3 u_CubeSize;
uniform bool u_IsSkybox;
uniform int u_IsPBRObjects;



layout(rgba8, binding = 0) uniform image3D texture3D;

in vec3 worldPositionFrag;
in vec3 normalFrag;
in vec3 texCoordFrag;

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

vec3 BRDF(vec3 Kd,vec3 Ks,vec3 specular){
	
	vec3 fLambert = u_Material.diffuseColor/PI;//diffuseColor 相当于 albedo
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
/*******************************************************************************************/

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir){
	vec3 fragColor;

	float distance = length(light.position-worldPositionFrag);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	//ambient
	vec3 ambient = light.ambient * (u_Material.ambientColor );
	
	//diffuse
	vec3 lightDir = normalize(light.position-worldPositionFrag);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * (u_Material.diffuseColor*(1-u_Settings.isTextureSample)+texture(u_Material.diffuse,texCoordFrag.xy).rgb*u_Settings.isTextureSample );

	//specular
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
		specular =  light.specular * spec  *  u_Material.specularColor;
	}
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	fragColor = diffuse + ambient + specular;
	
	return fragColor;
}

vec3 scaleAndBias(vec3 p) { return 0.5f * p + vec3(0.5f); }

bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }


void main(){
	vec3 viewDir = normalize(u_CameraViewPos-worldPositionFrag);
	vec3 color = vec3(0.0);

	vec3 normalWorldPositionFrag =worldPositionFrag-u_CameraViewPos;// vec3(worldPositionFrag.x-u_CubePos.x,worldPositionFrag.y-u_CubePos.y,worldPositionFrag.z-u_CubePos.z);
	normalWorldPositionFrag = normalWorldPositionFrag/u_CubeSize;
	if(!isInsideCube(normalWorldPositionFrag, 0.2)) return;


	if(u_IsPBRObjects==0){
		if(u_IsSkybox)
			color=texture(u_Material.cube,texCoordFrag).rgb;
		else
			for(int i = 0; i < u_PointLightNums; ++i) 
			{
				color += CalcPointLight(u_PointLights[i], normalFrag,viewDir);
			}
	}
	else{
		vec3 albedo =pow(texture(u_Material.diffuse, texCoordFrag.xy).rgb, vec3(2.2));//vec3(pow( texture(u_Material.diffuse, v_TexCoord).r, (2.2)));

		float mentallic = texture(u_Material.mentallic,texCoordFrag.xy).r;
		float roughness  = texture(u_Material.roughness ,texCoordFrag.xy).r;
		float ao        = texture(u_Material.ao, texCoordFrag.xy).r;
		vec3 emission = texture(u_Material.emission,texCoordFrag.xy).rgb;
//
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
			Lo+= BRDF(Kd,Ks,specular)*LightRadiance(v_FragPos,u_PointLights[i])*NdotL;
		}
		vec3 ambient = vec3(0.03) * albedo * ao;
		color = ambient + emission+ Lo;

		color = color / (color + vec3(1.0));
		color = pow(color, vec3(1.0/2.2));  
	}
	


	vec3 voxel = scaleAndBias(normalWorldPositionFrag);
	ivec3 dim = imageSize(texture3D);// retrieve the dimensions of an image
	vec4 res = vec4(vec3(color), 1);
    imageStore(texture3D, ivec3(dim * voxel), res);//write a single texel into an image;
}