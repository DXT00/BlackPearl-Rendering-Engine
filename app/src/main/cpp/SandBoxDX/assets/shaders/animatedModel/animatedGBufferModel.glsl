#type vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 5) in ivec4 aJointIndices;/*�뵱ǰfragment��ص�3��Joint  {1,2,4}*/
layout(location = 6) in ivec4 aJointIndices1;/*�뵱ǰfragment��ص�3��Joint  {1,2,4}*/
layout(location = 7) in vec4 aWeights;/*ÿ��Joint��Ȩ��*/
layout(location = 8) in vec4 aWeights1;/*ÿ��Joint��Ȩ��*/

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

const int MAX_JOINTS = 70;//max joints allowed in a skeleton
const int MAX_WEIGHT = 8;//max number of joints that can affect a vertex

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;
uniform mat4 u_JointModel[MAX_JOINTS];
void main(){
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalLocalNormal = vec4(0.0);

	mat4 model = mat4(0.0);
	for(int i=0;i<MAX_WEIGHT/2;i++){
		
		model+=aWeights[i]*u_JointModel[aJointIndices[i]];

	}
		for(int i=0;i<MAX_WEIGHT/2;i++){
		
		model+=aWeights1[i]*u_JointModel[aJointIndices1[i]];

	}

	vec4 newPos = model*vec4(aPos,1.0);
	gl_Position = u_ProjectionView*u_Model*vec4(newPos.xyz,1.0);
	v_FragPos = vec3(u_Model*newPos);//vec3(u_Model*vec4(newPos.xyz,1.0));

	vec4 newNormal = model*vec4(aNormal,0.0);
	v_Normal =  mat3(u_TranInverseModel)*newNormal.xyz;//vec3(u_Model*newNormal);
	v_TexCoord = aTexCoords;

}

#type fragment
#version 330 core


/* MRT */
/* render to gBuffer */
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffuse_Roughness; //u_Material.diffuse + u_Material.roughness
layout (location = 3) out vec4 gSpecular_Mentallic; //u_Material.specular + u_Material.metallic

/* �洢ȫ�ֹ����е� diffuse ��specular (vec3 ambient =  (Kd*diffuse+specular) * ao;)����ɫ*/
layout (location = 4) out vec4 gAmbientGI_AO; //GIAmbient + u_Material.ao
layout (location = 5) out vec3 gNormalMap;
in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BrdfLUTMap;
uniform int u_Kprobes;
uniform float u_ProbeWeight[10];

uniform vec3 u_CameraViewPos;
uniform vec3 u_SHCoeffs[10*9];//���10��probe
uniform int u_IsPBRObjects;



//uniform struct Material{
//	vec3 ambientColor;
//	vec3 diffuseColor;
//	vec3 specularColor;
//	vec3 emissionColor;
//	float roughnessValue;
//	float mentallicValue;
//	float aoValue;
//	sampler2D diffuse; //or call it albedo
//	sampler2D specular;
//	sampler2D emission;
//	sampler2D normal;
//	sampler2D height;
//	sampler2D ao;
//	sampler2D roughness;
//	sampler2D mentallic;
//	
//	float shininess;
//	bool isBlinnLight;
//	int  isTextureSample;//�ж��Ƿ�ʹ��texture,����ֻ��color
//	int isDiffuseTextureSample;
//	int isSpecularTextureSample;
//	int isMetallicTextureSample;
//
//}u_Material;
//
uniform Material u_Material;
uniform Settings u_Settings;

vec3 SHDiffuse(const int probeIndex,const vec3 normal){
	float x = normal.x;
	float y = normal.y;
	float z = normal.z;

	vec3 result = (
		u_SHCoeffs[0+probeIndex*9] +
		
		u_SHCoeffs[1+probeIndex*9] * x +
		u_SHCoeffs[2+probeIndex*9] * y +
		u_SHCoeffs[3+probeIndex*9] * z +
		
		u_SHCoeffs[4+probeIndex*9] * z * x +
		u_SHCoeffs[5+probeIndex*9] * y * z +
		u_SHCoeffs[6+probeIndex*9] * y * x +
		u_SHCoeffs[7+probeIndex*9] * (3.0 * z * z - 1.0) +
		u_SHCoeffs[8+probeIndex*9] * (x*x - y*y)
  );

  return max(result, vec3(0.0));
}
//TODO::������ͼ
vec3 getNormalFromMap(vec3 fragPos,vec2 texCoord)
{
    vec3 tangentNormal =  2.0* texture(u_Material.normal, v_TexCoord).xyz- vec3(1.0);
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
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 CalculateAmbientGI(vec3 albedo,vec3 specularColor){
	vec3 emission =u_Material.emissionColor; //texture(u_Material.emission,v_TexCoord).rgb;
	vec3 N = normalize(v_Normal);//getNormalFromMap(v_FragPos,v_TexCoord);

	vec3 V = normalize(u_CameraViewPos-v_FragPos);
	vec3 R = reflect(-V,N);

	


	vec3 environmentIrradiance=vec3(0.0);//= vec3(1.0,1.0,1.0);
	for(int i=0;i<u_Kprobes;i++){
		environmentIrradiance+=u_ProbeWeight[i]*SHDiffuse(i,N);// u_ProbeWeight[i]*texture(u_IrradianceMap[i],N).rgb;
		//environmentIrradiance*= texture(u_IrradianceMap[i],N).rgb;

	}
	vec3 diffuse = environmentIrradiance*albedo;

	
	vec3 prefileredColor = vec3(0.0,0.0,0.0) ;//= vec3(1.0,1.0,1.0);

	/*specular Mapֻȡ�����һ��*/
	prefileredColor = texture(u_PrefilterMap,R).rgb;
//	for(int i=0;i<u_Kprobes;i++){
//		prefileredColor+= u_ProbeWeight[i]*textureLod(u_PrefilterMap[i],R,roughness*MAX_REFLECTION_LOD).rgb;
//	//	prefileredColor*= textureLod(u_PrefilterMap[i],R,roughness*MAX_REFLECTION_LOD).rgb;
//
//	}
	

	vec3 specular = prefileredColor*specularColor;
	vec3 ambient =  diffuse+specular;

//	 ambient = ambient / (ambient + vec3(1.0));
//	//gamma correction
//    ambient = pow(ambient, vec3(1.0/2.2));  
	return ambient+emission;


}





vec3 CalculateAmbientGI(vec3 albedo,float metallic, float roughness,float ao){

//	vec3 albedo = pow(texture(u_Material.diffuse,v_TexCoord).rgb,vec3(2.2));
//	float metallic = texture(u_Material.mentallic, v_TexCoord).r;
//    float roughness = texture(u_Material.roughness, v_TexCoord).r;
//    float ao = texture(u_Material.ao, v_TexCoord).r;
	//vec3 normal = texture(u_Material.normal,v_TexCoord).xyz;
	//normal = normalize(normal);
	vec3 emission =u_Material.diffuseColor; //texture(u_Material.emission,v_TexCoord).rgb;

	vec3 N = getNormalFromMap(v_FragPos,v_TexCoord);

	vec3 V = normalize(u_CameraViewPos-v_FragPos);
	vec3 R = reflect(-V,N);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0,albedo,metallic);

	//ambient lightings (we now use IBL as the ambient term)!
	vec3 F =  FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 Ks = F;
	vec3 Kd = vec3(1.0)-Ks;
	Kd *= (1.0 - metallic);
	vec3 environmentIrradiance=vec3(0.0);//= vec3(1.0,1.0,1.0);
	for(int i=0;i<u_Kprobes;i++){
		environmentIrradiance+=u_ProbeWeight[i]*SHDiffuse(i,N);// u_ProbeWeight[i]*texture(u_IrradianceMap[i],N).rgb;
		//environmentIrradiance*= texture(u_IrradianceMap[i],N).rgb;

	}
	vec3 diffuse = environmentIrradiance*albedo;

	//sample both the prefilter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part
	const float MAX_REFLECTION_LOD = 1.0;
	//sample MAX_REFLECTION_LOD level mipmap everytime !
	vec3 prefileredColor = vec3(0.0,0.0,0.0) ;//= vec3(1.0,1.0,1.0);

	/*specular Mapֻȡ�����һ��*/
	prefileredColor = textureLod(u_PrefilterMap,R,roughness*MAX_REFLECTION_LOD).rgb;
//	for(int i=0;i<u_Kprobes;i++){
//		prefileredColor+= u_ProbeWeight[i]*textureLod(u_PrefilterMap[i],R,roughness*MAX_REFLECTION_LOD).rgb;
//	//	prefileredColor*= textureLod(u_PrefilterMap[i],R,roughness*MAX_REFLECTION_LOD).rgb;
//
//	}
	vec2 brdf = texture(u_BrdfLUTMap,vec2(max(dot(N,V),0.0),roughness)).rg;

	vec3 specular = prefileredColor * (F*brdf.x+brdf.y);
	vec3 ambient =  (Kd*diffuse+specular) * ao;

//	 ambient = ambient / (ambient + vec3(1.0));
//	//gamma correction
//    ambient = pow(ambient, vec3(1.0/2.2));  
	return ambient+emission;

}

void main(){
    // Store the fragment position vector in the first gbuffer texture
	gPosition       = v_FragPos;
	// Also store the per-fragment normals into the gbuffer
	gNormal         = v_Normal;

	int s = u_Settings.isPBRTextureSample;

	gNormalMap      =  v_Normal*(1-s) + getNormalFromMap(v_FragPos,v_TexCoord)*s;//texture(u_Material.normal, v_TexCoord).xyz;
	float metallic = u_Material.mentallicValue *(1-s)+texture(u_Material.mentallic, v_TexCoord).r*s;
	float roughness = u_Material.roughnessValue *(1-s)+texture(u_Material.roughness, v_TexCoord).r*s;
	float ao = u_Material.aoValue *(1-s)+texture(u_Material.ao, v_TexCoord).r*s;
	


	vec3 diffuse = (u_Material.diffuseColor *(1-u_Settings.isDiffuseTextureSample)
					+ texture(u_Material.diffuse,v_TexCoord).rgb*u_Settings.isDiffuseTextureSample);

	vec3 albedo = pow(diffuse,vec3(2.2));

	vec3 specular = (u_Material.specularColor *(1-u_Settings.isSpecularTextureSample)
					+ texture(u_Material.specular,v_TexCoord).rgb*u_Settings.isSpecularTextureSample);

	


	gSpecular_Mentallic.rgb = specular;

	gSpecular_Mentallic.a = metallic;


	if(u_IsPBRObjects==1)
		gAmbientGI_AO.rgb =	CalculateAmbientGI(albedo,metallic,roughness,ao);
	else
		gAmbientGI_AO.rgb =	CalculateAmbientGI(albedo,specular);

	gAmbientGI_AO.a =u_IsPBRObjects;//	ao;


	gDiffuse_Roughness.rgb = diffuse;

	gDiffuse_Roughness.a =  roughness;

	

}






