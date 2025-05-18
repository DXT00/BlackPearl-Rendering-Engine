  
#type vertex
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!
uniform mat4 u_ProjectionView;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;
void main()
{
	gl_Position = u_ProjectionView * u_Model * vec4(aPos,1.0);
	v_FragPos = vec3(u_Model* vec4(aPos,1.0));
	v_TexCoord = vec2(aTexCoords.x,aTexCoords.y);
	//v_Normal =  mat3(u_TranInverseModel)* aNormal;//vec3(u_Model * vec4(aNormal,1.0));
	v_Normal =  mat3(u_Model)* aNormal;//vec3(u_Model * vec4(aNormal,1.0));

}

#type fragment
#version 450 core

const float PI=3.14159265359;
struct PointLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	float constant;
	float linear;
	float quadratic;

};

uniform Material u_Material;
uniform Settings u_Settings;
in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoord;

out vec4 FragColor;
//Light
uniform PointLight u_PointLights[10];
uniform int u_PointLightNums;
//Camera
uniform vec3 u_CameraViewPos;

//IBL
uniform samplerCube u_IrradianceMap;
uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BrdfLUTMap;
uniform samplerCube u_ShadowMap[10];
uniform int u_IsPBRObjects;
uniform float u_FarPlane;
int step = 100;
vec3 sum  = vec3(0.0);
float dw = 1.0/step;
float kd=0.1;
float ks=0.1;


//TODO::法线贴图
vec3 getNormalFromMap()
{
    vec3 tangentNormal =  2.0* texture(u_Material.normal, v_TexCoord).xyz- vec3(1.0);

    vec3 Q1  = dFdx(v_FragPos);
    vec3 Q2  = dFdy(v_FragPos);
    vec2 st1 = dFdx(v_TexCoord);
    vec2 st2 = dFdy(v_TexCoord);

    vec3 N   = normalize(v_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
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
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 BRDF(vec3 Kd,vec3 Ks,vec3 specular,vec3 albedo){
	
	vec3 fLambert = albedo/PI;//diffuseColor 相当于 albedo
	return Kd * fLambert+  specular;//specular 中已经有Ks(Ks=F)了，不需要再乘以Ks *
}
vec3 LightRadiance(vec3 fragPos,PointLight light){
	float attenuation = calculateAttenuation(light,fragPos);
	//float cosTheta = max(dot(N,wi),0.0);
	vec3 radiance = light.diffuse*attenuation;
	return radiance;
}

vec3 calculateDirectLight(vec3 fragPos,vec3 normal,const PointLight light, const vec3 viewDir,vec3 diffuseColor,vec3 specularColor){
	vec3 direct = vec3(0.0);

	vec3 lightDir = light.position - fragPos;
//	//MY 
//	vec3 ligtdir = lightDirection/u_CubeSize;
//	float distToLight = length(ligtdir);

	const float distanceToLight = length(lightDir);
	lightDir =normalize(lightDir);// lightDir / distanceToLight;
	float attenuation = 1.0f/(light.constant+light.linear * distanceToLight+light.quadratic*distanceToLight*distanceToLight);

	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff* diffuseColor;

	/*specular*/
	vec3 specular;
	float spec;
	float shininess = 64.0;



		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),64);
		specular =  light.specular *spec* specularColor;//texture(material.specular,v_TexCoords).rgb;

	// vec3 emission = texture(u_Material.emission, v_TexCoord).rgb;


	//ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	//emission *= attenuation;

	direct = diffuse + specular;//+ ambient +emission;// * mix(texture(u_Texture1, v_TexCoords), texture(u_Texture2, vec2(1.0 - v_TexCoords.x, v_TexCoords.y)), u_MixValue);
	



	return direct;
	
};
float ShadowCalculation(vec3 fragPos,vec3 lightPos,samplerCube shadowMap){

		vec3 fragToLight = fragPos.xyz - lightPos; 

	 float closestDepth = texture(shadowMap, fragToLight).r;
	// It is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth = closestDepth*u_FarPlane;
    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // Now test for shadows
  
	float shadow = 0.0;
	float bias = 0.08; 
	float samples = 4.0;
	float offset = 0.1;
	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
	    for(float y = -offset; y < offset; y += offset / (samples * 0.5))
	    {
	        for(float z = -offset; z < offset; z += offset / (samples * 0.5))
	        {
	            float closestDepth = texture(shadowMap, fragToLight + vec3(x, y, z)).r; 
	            closestDepth *= u_FarPlane;   // Undo mapping [0;1]
	            if(currentDepth - bias > closestDepth)
	                shadow += 1.0;
	        }
	    }
	}
	shadow /= (samples * samples * samples);
	return shadow;

//vec3 fragToLight = fragPos.xyz - lightPos; 
//float SampledDistance = texture(u_CubeMap,fragToLight).r;
//
//    float Distance = length(fragToLight);
//
//    if (Distance < SampledDistance*u_FarPlane + bias)
//        return 0.3; // Inside the light
//    else
//        return 0.8; // Inside the shadow
}

void main(){
	//material properties
	 vec3 color;
	 int pointLightNum = u_PointLightNums;
	 int pbr = u_IsPBRObjects;
	if(pbr==1){
		vec3 albedo = pow(texture(u_Material.diffuse,v_TexCoord).rgb,vec3(2.2));
		float metallic = texture(u_Material.mentallic, v_TexCoord).r;
		float roughness = texture(u_Material.roughness, v_TexCoord).r;
		float ao = texture(u_Material.ao, v_TexCoord).r;
		int emmisionSample = u_Settings.isEmissionTextureSample;
		vec3 emission =vec3(0);// (emmisionSample==1)?texture(u_Material.emission,v_TexCoord).rgb:vec3(0);

		vec3 N = getNormalFromMap();
		vec3 V = normalize(u_CameraViewPos-v_FragPos);
		vec3 R = reflect(-V,N);

		vec3 F0 = vec3(0.04);
		F0 = mix(F0,albedo,metallic);

		//reflection equation
		vec3 Lo = vec3(0.0);
		for(int i=0;i<pointLightNum;i++){
			vec3 L = normalize(u_PointLights[i].position-v_FragPos);
			vec3 H = normalize(V+L);
			float attenuation = calculateAttenuation(u_PointLights[i],v_FragPos);
			vec3 radiance = u_PointLights[i].diffuse * attenuation;

			float NDF = NoemalDistribution_TrowbridgeReitz_GGX(N,H,roughness);
			float G = GeometrySmith(N, V,L,roughness);
			vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
			
			//CookTorrance
			vec3 nominator    = NDF * G * F;
		    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
		    vec3 specular     = nominator / denominator;
			
			// kS is equal to Fresnel
			vec3 Ks = F;
			// for energy conservation, the diffuse and specular light can't
		    // be above 1.0 (unless the surface emits light); to preserve this
		    // relationship the diffuse component (kD) should equal 1.0 - kS.
			vec3 Kd = vec3(1.0)-Ks;
			 // multiply kD by the inverse metalness such that only non-metals 
		    // have diffuse lighting, or a linear blend if partly metal (pure metals
		    // have no diffuse light).
			Kd *= (1.0 - metallic);

			
			float NdotL = max(dot(N,L),0.0);
			float shadow = ShadowCalculation(v_FragPos,u_PointLights[i].position,u_ShadowMap[i]); 

			Lo+=(1.0 - shadow)* BRDF(Kd,Ks,specular,albedo)*LightRadiance(v_FragPos,u_PointLights[i])*NdotL;
		}

		//ambient lightings (we now use IBL as the ambient term)!
		vec3 F =  FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
		vec3 Ks = F;
		vec3 Kd = vec3(1.0)-Ks;
		Kd *= (1.0 - metallic);
		vec3 environmentIrradiance = texture(u_IrradianceMap,N).rgb;
		vec3 diffuse = environmentIrradiance*albedo;

		//sample both the prefilter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part
		const float MAX_REFLECTION_LOD = 4.0;
		//sample MAX_REFLECTION_LOD level mipmap everytime !
		vec3 prefileredColor = textureLod(u_PrefilterMap,R,roughness*MAX_REFLECTION_LOD).rgb;
		vec2 brdf = texture(u_BrdfLUTMap,vec2(max(dot(N,V),0.0),roughness)).rg;

		vec3 specular = prefileredColor * (F*brdf.x+brdf.y);
		vec3 ambient = (Kd*diffuse+specular) * ao;
		color = u_Settings.GICoeffs * ambient+emission+ Lo;



	}
	else{

		vec3 N = normalize(v_Normal);//getNormalFromMap(v_FragPos,v_TexCoord);

		vec3 V = normalize(u_CameraViewPos-v_FragPos);
		vec3 R = reflect(-V,N);

		


		vec3 environmentIrradiance=texture(u_IrradianceMap,N).rgb;
		
		

		
		//vec3 diffuse = environmentIrradiance*(texture(u_Material.diffuse,v_TexCoord).rgb*u_Settings.isTextureSample+(1-u_Settings.isTextureSample)*u_Material.diffuseColor);
		int s =u_Settings.isDiffuseTextureSample;
		vec3 MaterialDiffuse;
		if(s==0)
			MaterialDiffuse =(u_Material.diffuseColor);
		else
			MaterialDiffuse = (texture(u_Material.diffuse,v_TexCoord).rgb);

		vec3 diffuse = environmentIrradiance*MaterialDiffuse;
		
		vec3 prefileredColor = vec3(0.0,0.0,0.0) ;//= vec3(1.0,1.0,1.0);

		/*specular Map只取最近的一个*/
		prefileredColor = textureLod(u_PrefilterMap,R,4).rgb;
//		for(int i=0;i<u_Kprobes;i++){
//			prefileredColor+= u_ProbeWeight[i]*textureLod(u_PrefilterMap[i],R,roughness*MAX_REFLECTION_LOD).rgb;
//		//	prefileredColor*= textureLod(u_PrefilterMap[i],R,roughness*MAX_REFLECTION_LOD).rgb;
//
//		}
		
		vec3 specularColor = (u_Material.specularColor);
		vec3 specular = prefileredColor*specularColor;
		vec3 ambient =  diffuse+specular;

//		 ambient = ambient / (ambient + vec3(1.0));
//		//gamma correction
//		  ambient = pow(ambient, vec3(1.0/2.2));  
		vec3 directLight = vec3(0);
		for(int i=0;i<pointLightNum;i++){
			float shadow = ShadowCalculation(v_FragPos,u_PointLights[i].position,u_ShadowMap[i]); 
			directLight+=(1.0 - shadow) * calculateDirectLight( v_FragPos,N,u_PointLights[i],V ,MaterialDiffuse,u_Material.specularColor);
		
		}
		color=u_Settings.GICoeffs * ambient+directLight;

	}
	bool hdr = u_Settings.hdr;
	if(hdr){
		//HDR tonemapping
		color = color / (color + vec3(1.0));
		//gamma correction
		color = pow(color, vec3(1.0/2.2));  

	}
    FragColor = vec4(color, 1.0);
}