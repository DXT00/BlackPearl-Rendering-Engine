#type vertex
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;


void main(){

	gl_Position = u_ProjectionView*u_Model*vec4(aPos,1.0);
	
	
	v_TexCoord = aTexCoords;
	v_Normal   = mat3(u_TranInverseModel)*aNormal;
	v_FragPos  = vec3(u_Model*vec4(aPos,1.0));




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
in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BrdfLUTMap;
uniform int u_Kprobes;
uniform float u_ProbeWeight[10];

uniform vec3 u_CameraViewPos;
uniform vec3 u_SHCoeffs[10*9];//最多10个probe
uniform int u_IsPBRObjects;



//struct Settings;
//
//
//struct Material;
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
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 CalculateAmbientGI(vec3 albedo,vec3 specularColor){
	
	vec3 N = normalize(v_Normal);//getNormalFromMap(v_FragPos,v_TexCoord);

	vec3 V = normalize(u_CameraViewPos-v_FragPos);
	vec3 R =normalize( reflect(-V,N));

	


	vec3 environmentIrradiance=vec3(0.0);
	int kProbe = u_Kprobes;

	for(int i=0;i<kProbe;i++){
		environmentIrradiance+=u_ProbeWeight[i]*SHDiffuse(i,N);// u_ProbeWeight[i]*texture(u_IrradianceMap[i],N).rgb;
	
	}
	vec3 diffuse = environmentIrradiance*albedo;

	
	vec3 prefileredColor = vec3(0.0,0.0,0.0) ;//= vec3(1.0,1.0,1.0);

	/*specular Map只取最近的一个*/
	prefileredColor = texture(u_PrefilterMap,R).rgb;


	vec3 specular = prefileredColor*specularColor;
	vec3 ambient =  diffuse+specular;

//	 ambient = ambient / (ambient + vec3(1.0));
//	//gamma correction
//    ambient = pow(ambient, vec3(1.0/2.2));  
	return ambient;//ambient;


}





vec3 CalculateAmbientGI(vec3 albedo,float metallic, float roughness,float ao){

//	vec3 albedo = pow(texture(u_Material.diffuse,v_TexCoord).rgb,vec3(2.2));
//	float metallic = texture(u_Material.mentallic, v_TexCoord).r;
//    float roughness = texture(u_Material.roughness, v_TexCoord).r;
//    float ao = texture(u_Material.ao, v_TexCoord).r;
	//vec3 normal = texture(u_Material.normal,v_TexCoord).xyz;
	//normal = normalize(normal);
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
	int kProbe = u_Kprobes;
	for(int i=0;i<kProbe;i++){
		environmentIrradiance+=u_ProbeWeight[i]*SHDiffuse(i,N);// u_ProbeWeight[i]*texture(u_IrradianceMap[i],N).rgb;

	}
	vec3 diffuse = environmentIrradiance*albedo;

	//sample both the prefilter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part
	const float MAX_REFLECTION_LOD = 4.0;//1.0;
	//sample MAX_REFLECTION_LOD level mipmap everytime !
	/*specular Map只取最近的一个*/
	vec3 prefileredColor = textureLod(u_PrefilterMap,R,roughness*MAX_REFLECTION_LOD).rgb;//= vec3(1.0,1.0,1.0);


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
	return ambient;

}

void main(){

	 // Store the fragment position vector in the first gbuffer texture
	gPosition.rgb   = v_FragPos;
	// Also store the per-fragment normals into the gbuffer
	gNormal.rgb     = v_Normal;

	int s = u_Settings.isPBRTextureSample;
	gNormalMap      =  (s==0)? normalize(v_Normal):getNormalFromMap(v_FragPos,v_TexCoord);
	float roughness =  (s==0)?u_Material.roughnessValue :texture(u_Material.roughness, v_TexCoord).r;
	float ao        =  (s==0)?u_Material.aoValue:texture(u_Material.ao, v_TexCoord).r;
	float metallic = (s==0)?u_Material.mentallicValue:texture(u_Material.mentallic, v_TexCoord).r;


	int diffuseSample = u_Settings.isDiffuseTextureSample;
	vec3 diffuse = (diffuseSample==0)?u_Material.diffuseColor :texture(u_Material.diffuse,v_TexCoord).rgb;

	vec3 albedo =diffuse;// pow(diffuse,vec3(2.2));

	int specularSample = u_Settings.isSpecularTextureSample;
	vec3 specular = (specularSample==0)?u_Material.specularColor:texture(u_Material.specular,v_TexCoord).rgb;

	//int metallicSample = u_Settings.isMetallicTextureSample;




	

	gSpecular_Mentallic.rgb = specular;

	gSpecular_Mentallic.a = metallic;

	int pbr = u_IsPBRObjects;
	if(pbr==1)
		gAmbientGI_AO.rgb =	CalculateAmbientGI(albedo,metallic,roughness,ao);
	else
		gAmbientGI_AO.rgb =	CalculateAmbientGI(diffuse,specular);

	gAmbientGI_AO.a =u_IsPBRObjects;//	ao;


	gDiffuse_Roughness.rgb = diffuse;

	gDiffuse_Roughness.a =  roughness;

	





}






