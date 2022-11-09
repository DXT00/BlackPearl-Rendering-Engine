#type vertex
#version 450 core

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

	gl_Position   = u_ProjectionView*u_Model*vec4(aPos,1.0);
	bool isSkybox = u_IsSkybox;
	int sampleHeightTexture = u_isHeightTextureSample;

	if(isSkybox)
		v_TexCoord = aPos;
	else
		v_TexCoord = vec3(aTexCoords.x,aTexCoords.y,1.0);
	v_FragPos    = vec3(u_Model*vec4(aPos,1.0));
	v_Normal     = normalize(mat3(u_TranInverseModel)*aNormal);
	if(sampleHeightTexture==1){
		v_Normal     = normalize((u_Model*vec4(aNormal,0.0)).xyz);

		v_Tangent    = normalize((u_Model*vec4(aTangent,0.0)).xyz);
		v_Bitangent  = normalize((u_Model*vec4(aBitangent,0.0)).xyz);
	}
	else{
		v_Tangent = vec3(0.0);
		v_Bitangent = vec3(0.0);
	}
	



}

#type fragment
#version 450 core

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
uniform vec2 u_HeightMapSize;

uniform int u_ObjectId;
uniform vec3 u_CameraViewPos;

vec2 s_texCoordxy = v_TexCoord.xy;

uniform Settings u_Settings;
uniform Material u_Material;
uniform int u_IsPBRObjects;
uniform bool u_IsSkybox;

uniform int u_isHeightTextureSample;

float GetHeight(vec2 texCoord){
	if(texCoord.x<0||texCoord.y<0||texCoord.x>1||texCoord.y>1)return 0.0;
	return texture(u_Material.height, texCoord).r;
}

vec3 getNormalFromHeightMap(vec2 texCoord){
    mat3 tangentToWorld = inverse(transpose(mat3(v_Tangent, v_Normal, v_Bitangent)));

	vec2 offset = vec2(1.0/u_HeightMapSize.x,1.0/u_HeightMapSize.y);
	//float curHeight = texture(u_Material.height,texCoord).r;
	
	float heightL = GetHeight(texCoord + vec2(-offset.x, 0.0));
	float heightR = GetHeight(texCoord + vec2(offset.x, 0.0));
	float heightU = GetHeight(texCoord + vec2(0.0, offset.y));
	float heightD = GetHeight(texCoord + vec2(0.0,-offset.y));
//	float diffX = texture(u_Material.height, texCoord + vec2(offset.x, 0.0)).r - curHeight;
//    float diffY = texture(u_Material.height, texCoord + vec2(0.0, offset.y)).r - curHeight;
//
//    // Tangent space bump normal
//    float bumpMult =-1.0;// -3.0;
    vec3 bumpNormal_tangent = normalize(vec3(heightL-heightR, 2.0,heightD-heightU));

	return normalize(tangentToWorld*bumpNormal_tangent);
}

//TODO::法线贴图
vec3 getNormalFromNormalMap(vec3 fragPos,vec2 texCoord)
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

 	int sampleDiffuseTexture = u_Settings.isDiffuseTextureSample;
	int samplePBRTex = u_Settings.isPBRTextureSample;
	int sampleSpecularTexture = u_Settings.isSpecularTextureSample;
	int sampleHeightTexture = u_isHeightTextureSample;
	bool isSkybox = u_IsSkybox;


	gPosition.rgb   = v_FragPos;
	gPosition.a     = float(u_IsPBRObjects*256.0+u_ObjectId); //15-8bit:u_IsPBRObjects,7-0bit:u_ObjectId

	gNormal.rgb   = (sampleHeightTexture==0)?normalize(v_Normal):getNormalFromHeightMap(s_texCoordxy);
	//gNormal.rgb   = normalize(v_Normal);

	gNormal.a     = (isSkybox)?1.0:0.0;

	gNormalMap      =  (samplePBRTex==0)?normalize(v_Normal):getNormalFromNormalMap(v_FragPos,s_texCoordxy);
	float roughness =  (samplePBRTex==0)?u_Material.roughnessValue :texture(u_Material.roughness, s_texCoordxy).r;
	float ao        =  (samplePBRTex==0)?u_Material.aoValue:texture(u_Material.ao, s_texCoordxy).r;
	float metallic  =  (samplePBRTex==0)?u_Material.shininess:texture(u_Material.mentallic,s_texCoordxy).r;//u_Material.mentallicValue *(1-u_Settings.isMetallicTextureSample)+

	vec3 diffuse;
	//if(isSkybox)
	//	diffuse=texture(u_Material.cube,v_TexCoord).rgb;
	//else{

	if(sampleDiffuseTexture==0)
		diffuse = u_Material.diffuseColor ;//u_Settings.isDiffuseTextureSample
	else
		diffuse = texture(u_Material.diffuse,s_texCoordxy).rgb;
	//}
	//vec3 albedo = pow(diffuse,vec3(2.2));
	vec3 specular;
	if(sampleSpecularTexture==0)
		specular = (u_Material.specularColor );
	else
		specular = texture(u_Material.specular,s_texCoordxy).rgb;




	gSpecular_Mentallic.rgb = specular;
	gSpecular_Mentallic.a = metallic;

	gAmbientGI_AO.a =	ao;
	gAmbientGI_AO.rgb = (sampleHeightTexture==0)?vec3(0,1,0):vec3(1,0,0);

	//gAmbientGI_AO.a =u_IsPBRObjects;//	ao;


	gDiffuse_Roughness.rgb = diffuse;

	gDiffuse_Roughness.a =  roughness;

}






