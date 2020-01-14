### IBL specular part

#### 1.计算specular irradiance --> PrefilterMAp --RenderPrefilterMap()

prefilterMap.glsl fragment shader

```
void main(){

	vec3 N = normalize(WorldPos);
	//make the simplyfying assumption that view = reflection = normal direction
	vec3 R=N;
	vec3 V=R;

	//蒙特卡洛采样
	const uint SAMPLE_COUNT=1024u;
	vec3 prefilterColor = vec3(0.0);
	float totalWeight = 0.0;

	for(uint i=0u;i<SAMPLE_COUNT;i++){
		
		vec2 Xi = HammersleyNoBitOps(i,SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(Xi,N,u_roughness);
		//计算反射光线方向
		vec3 L = normalize(2.0* dot(V,H)*H-V);

		float NdotL = max(dot(N,L),0.0);

		if(NdotL>0.0){
			float D = NoemalDistribution_TrowbridgeReitz_GGX(N,H,u_roughness);
			float NdotH = max(dot(N,H),0.0);
			float HdotV = max(dot(N,V),0.0);
			float pdf = D*NdotH/(4.0*HdotV)+0.0001;

			float resolution = 512.0;// resolution of source cubemap (per face)
			float  saTexel = 4.0*PI/(6.0*resolution*resolution);
			float saSample= 1.0/(float(SAMPLE_COUNT)*pdf +0.0001);

			float mipLevel = (u_roughness == 0.0)? 0.0:0.5*log2(saSample / saTexel); 

			prefilterColor += textureLod(u_EnvironmentMap,L,mipLevel).rgb*NdotL;
			totalWeight+= NdotL;

		}

	}
	prefilterColor = prefilterColor/totalWeight;
	FragColor = vec4(prefilterColor,1.0);

}

```

#### 2.计算 BEDF look up table -->BRDFLUTMap --RenderBRDFLUTMap()

brdl.glsl fragment shader

```
vec2 InterateBRDF(float NdotV,float roughness){
	
	vec3 V;
	V.x = sqrt(1.0-NdotV*NdotV);
	V.y = 0.0;
	V.z = NdotV;

	float A = 0.0;
	float B = 0.0;

	vec3 N = vec3(0.0,0.0,1.0);

	const uint SAMPLE_COUNT = 1024u;
	for(uint i=0u;i<SAMPLE_COUNT;++i){
		
		vec2 Xi= HammersleyNoBitOps(i,SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(Xi,N,roughness);

		vec3 L = normalize(2.0 * dot(V, H) * H - V);
		float NdotL = max(L.z,0.0);
		float NdotH = max(H.z,0.0);
		float VdotH = max(dot(V,H),0.0);

		if(NdotL>0.0){
			float G = GeometrySmith(N,V,L,roughness);
			float G_Vis = (G*VdotH)/(NdotH* NdotV);
			float Fc = pow(1.0-VdotH,5.0);
			A +=(1.0-Fc)*G_Vis;
			B += Fc*G_Vis;
		}
	}
	A/=float(SAMPLE_COUNT);
	B/=float(SAMPLE_COUNT);

	return vec2(A,B);



}
void main(){

	vec2 integratedBRDF = InterateBRDF(v_TexCoord.x,v_TexCoord.y);
	FragColor = vec3(integratedBRDF.x,integratedBRDF.y,0.0);
	//FragColor = vec2(integratedBRDF.x,integratedBRDF.y);
}


```
#### 3.叠加 Diffuse 和 Specular ambient light

ibs.glsl fragment shader

```
	//ambient lightings (we now use IBL as the ambient term)!
	vec3 F =  FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, u_roughness);
	vec3 Ks = F;
	vec3 Kd = vec3(1.0)-Ks;
	Kd *= (1.0 - u_metallic);
	vec3 environmentIrradiance = texture(u_IrradianceMap,N).rgb;
	vec3 diffuse = environmentIrradiance*u_Material.diffuseColor;

	//sample both the prefilter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefileredColor = textureLod(u_PrefilterMap,R,u_roughness*MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(u_BrdfLUTMap,vec2(max(dot(N,V),0.0),u_roughness)).rg;
	//vec3 brdf = texture(u_BrdfLUTMap,vec2(max(dot(N,V),0.0),u_roughness)).rgb;

	vec3 specular = prefileredColor * (F*brdf.x+brdf.y);
	vec3 ambient = (Kd*diffuse+specular) * u_ao;
    vec3 color =ambient + Lo;

	//HDR tonemapping
    color = color / (color + vec3(1.0));
	//gamma correction
    color = pow(color, vec3(1.0/2.2));  

    FragColor = vec4(color, 1.0);
```

![BRDFLUPMap](/results/BRDFLUPMap.jpg)
![BRDFLUPMap1](/results/BRDFLUPMap1.jpg)
![IBLDiffuseSpecular](/results/IBLDiffuseSpecular.png)




