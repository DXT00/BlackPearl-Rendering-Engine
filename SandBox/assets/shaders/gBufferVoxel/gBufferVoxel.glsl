#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

uniform bool u_IsSkybox;
uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;


void main(){

	gl_Position = u_ProjectionView*u_Model*vec4(aPos,1.0);
	
	if(u_IsSkybox)
		v_TexCoord = aPos;
	else
		v_TexCoord = vec3(aTexCoords.x,aTexCoords.y,1.0);
	v_Normal   = mat3(u_TranInverseModel)*aNormal;
	v_FragPos  = vec3(u_Model*vec4(aPos,1.0));




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

uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BrdfLUTMap;
uniform int u_Kprobes;
uniform float u_ProbeWeight[10];

uniform vec3 u_CameraViewPos;
uniform vec3 u_SHCoeffs[10*9];//最多10个probe

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
	gPosition.a = u_IsPBRObjects;

	gNormal.rgb     = v_Normal;
	gNormal.a = (u_IsSkybox)?1.0:0.0;

	int s = u_Settings.isTextureSample;
	gNormalMap      =  v_Normal*(1-s) + getNormalFromMap(v_FragPos,s_texCoordxy)*s;
	float roughness =  u_Material.roughnessValue *(1-s)+texture(u_Material.roughness, s_texCoordxy).r*s;
	float ao        =  u_Material.aoValue *(1-s)+texture(u_Material.ao, s_texCoordxy).r*s;
	
	vec3 diffuse;
//	if(u_IsSkybox)
//		diffuse=texture(u_Material.cube,v_TexCoord).rgb;
//	else
		diffuse = u_Material.diffuseColor *(1-s)+ texture(u_Material.diffuse,s_texCoordxy).rgb*s;//_Settings.isDiffuseTextureSample

	//vec3 albedo = pow(diffuse,vec3(2.2));

	vec3 specular = (u_Material.specularColor *(1-s)+ texture(u_Material.specular,s_texCoordxy).rgb*s);//u_Settings.isSpecularTextureSample

	float metallic = u_Material.mentallicValue *(1-u_Settings.isMetallicTextureSample)+
					texture(u_Material.mentallic, s_texCoordxy).r*u_Settings.isMetallicTextureSample;




	gSpecular_Mentallic.rgb = specular;
	gSpecular_Mentallic.a = metallic;

	gAmbientGI_AO.a =	ao;
	

	//gAmbientGI_AO.a =u_IsPBRObjects;//	ao;


	gDiffuse_Roughness.rgb = diffuse;

	gDiffuse_Roughness.a =  roughness;

	

	

}






