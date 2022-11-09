#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec3 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;
out vec3 v_Tangent;
out vec3 v_Bitangent;

uniform bool u_IsSkybox;
uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;
uniform int u_isHeightTextureSample;

void main(){

	gl_Position = u_ProjectionView*u_Model*vec4(aPos,1.0);
	
	if(u_IsSkybox)
		v_TexCoord = aPos;
	else
		v_TexCoord = vec3(aTexCoords.x,aTexCoords.y,1.0);
	v_Normal   = normalize(mat3(u_TranInverseModel)*aNormal);
	v_FragPos  = vec3(u_Model*vec4(aPos,1.0));
	if(u_isHeightTextureSample==1){
		v_Tangent    = normalize((u_Model*vec4(aTangent,0.0)).xyz);
		v_Bitangent  = normalize((u_Model*vec4(aBitangent,0.0)).xyz);
	}
	else{
		v_Tangent = vec3(0.0);
		v_Bitangent = vec3(0.0);
	}



}




#type fragment
#version 430 core

/* MRT */
/* render to gBuffer */
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gDiffuse_Roughness; //u_Material.diffuse + u_Material.roughness
layout (location = 3) out vec4 gSpecular_Mentallic; //u_Material.specular + u_Material.metallic

/* 存储全局光照中的 diffuse 和specular (vec3 ambient =  (Kd*diffuse+specular) * ao;)的颜色*/
layout (location = 4) out vec4 gAmbientGI_AO; //GIAmbient + u_Material.ao
layout (location = 5) out vec3 gNormalMap;
in vec3 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;
in vec3 v_Tangent;
in vec3 v_Bitangent;
uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BrdfLUTMap;

uniform int u_ObjectId;

uniform vec3 u_CameraViewPos;

vec2 s_texCoordxy = v_TexCoord.xy;
//struct Settings;
//
//
//struct Material;
//

uniform Material u_Material;
uniform Settings u_Settings;
uniform int u_IsPBRObjects;
uniform bool u_IsSkybox;

uniform vec2 u_HeightMapSize;
uniform int u_isHeightTextureSample;

vec3 getNormalFromHeightMap(vec2 texCoord){
    mat3 tangentToWorld = inverse(transpose(mat3(v_Tangent, v_Normal, v_Bitangent)));

	vec2 offset = vec2(1.0)/u_HeightMapSize;
	float curHeight = texture(u_Material.height,texCoord).r;
	
	float diffX = texture(u_Material.height, texCoord + vec2(offset.x, 0.0)).r - curHeight;
    float diffY = texture(u_Material.height, texCoord + vec2(0.0, offset.y)).r - curHeight;

    // Tangent space bump normal
    float bumpMult = -3.0;
    vec3 bumpNormal_tangent = normalize(vec3(bumpMult*diffX, 1.0, bumpMult*diffY));

	return normalize(tangentToWorld*bumpNormal_tangent);
}


//TODO::法线贴图
vec3 getNormalFromMap(vec3 fragPos,vec2 texCoord)
{
    vec3 tangentNormal =  2.0* texture(u_Material.normal, texCoord).xyz- vec3(1.0);
	//vec3 tangentNormal =  2.0* normal- vec3(1.0);

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(texCoord);
    vec2 st2 = dFdy(texCoord);

    vec3 N   = normalize(v_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}



void main(){
 
	gPosition.rgb   = v_FragPos;
	gPosition.a = float(u_IsPBRObjects*256.0+u_ObjectId); //15-8bit:u_IsPBRObjects,7-0bit:u_ObjectId

	gNormal.rgb     = (u_isHeightTextureSample==0)?normalize(v_Normal):getNormalFromHeightMap(s_texCoordxy);//v_Normal;
	gNormal.a = (u_IsSkybox)?1.0:0.0;

	int s = u_Settings.isPBRTextureSample;
	gNormalMap      =  (s==0)?v_Normal:getNormalFromMap(v_FragPos,s_texCoordxy);
	float roughness =  (s==0)?u_Material.roughnessValue :texture(u_Material.roughness, s_texCoordxy).r;
	float ao        =  (s==0)?u_Material.aoValue :texture(u_Material.ao, s_texCoordxy).r;
	float metallic  =  (s==0)?u_Material.mentallicValue:texture(u_Material.mentallic, s_texCoordxy).r;

	vec3 diffuse;
//	if(u_IsSkybox)
//		diffuse=texture(u_Material.cube,v_TexCoord).rgb;
//	else
	diffuse = (u_Settings.isDiffuseTextureSample==0)?u_Material.diffuseColor :texture(u_Material.diffuse,s_texCoordxy).rgb;

	//vec3 albedo = pow(diffuse,vec3(2.2));

	vec3 specular ;
	if(u_Settings.isSpecularTextureSample==0)
		specular= u_Material.specularColor ;
	else
		specular=texture(u_Material.specular,s_texCoordxy).rgb;//u_Settings.isSpecularTextureSample

//	float metallic;
//	if(u_Settings.isMetallicTextureSample==0)
//		metallic= u_Material.mentallicValue ;
//	else
//		metallic= texture(u_Material.mentallic, s_texCoordxy).r;




	gSpecular_Mentallic.rgb = specular;
	gSpecular_Mentallic.a = metallic;

	gAmbientGI_AO.a =	ao;
	

	//gAmbientGI_AO.a =u_IsPBRObjects;//	ao;


	gDiffuse_Roughness.rgb = diffuse;

	gDiffuse_Roughness.a =  roughness;

	

	

}






