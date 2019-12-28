### IBL irradiance convolution

#### 1.Convolving the incident light of the hemispheres of each fragment  and generates IrradianceCubeMap according to BRDF

irradianceConvolution.glsl  fragment shader
```

in vec3 v_FragPos;
out vec4 FragColor;

uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265359;

void main(){
	//采样的方向等于半球的方向
	vec3 worldup = vec3(0.0,1.0,0.0);
		vec3 irradiance = vec3(0.0);
	vec3 N = normalize(v_FragPos);//TODO::
	vec3 right = cross(worldup,N);
	vec3 up = cross(N,right);


	float sampleStep=0.025;
	float nrSamples = 0.0;
	//convolution 
	for(float phi=0.0; phi< 2.0*PI;phi+=sampleStep){
		for(float theta=0.0;theta<0.5*PI;theta+=sampleStep){
			
			//spherical to catesian coordinate
			vec3 sphereCoords = vec3(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
			// tangent space to world
			vec3 direction = sphereCoords.x*right+sphereCoords.y*up+sphereCoords.z*N;
			irradiance += texture(u_EnvironmentMap,direction).rgb*sin(theta)*cos(theta);

			nrSamples++;

		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));
	FragColor =vec4(irradiance, 1.0);

}

```



#### 2. Sample from the irradianceCubeMap as the ambient

ibl.glsl  fragment shader

```
//...
void main(){
	vec3 N = normalize(v_Normal);
	vec3 V = normalize(u_CameraViewPos-v_FragPos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0,u_Material.diffuseColor,u_metallic);

	//reflection equation
	vec3 Lo = vec3(0.0);
	for(int i=0;i<u_PointLightNums;i++){
		vec3 L = normalize(u_PointLights[i].position-v_FragPos);
		vec3 H = normalize(V+L);
		float attenuation = calculateAttenuation(u_PointLights[i],v_FragPos);
		vec3 radiance = u_PointLights[i].diffuse * attenuation;

		float NDF = NoemalDistribution_TrowbridgeReitz_GGX(N,H,u_roughness);
		float G = GeometrySmith(N, V,L,u_roughness);
		vec3 F = FrehNel(max(dot(N, V), 0.0), F0, u_roughness);

		vec3 Ks = F;
		vec3 Kd = vec3(1.0)-Ks;
		Kd *= (1.0 - u_metallic);

		//CookTorrance
		vec3 nominator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
        vec3 specular     = nominator / denominator;
		
		float NdotL = max(dot(N,L),0.0);
		Lo+= BRDF(Kd,Ks,specular)*LightRadiance(v_FragPos,u_PointLights[i])*NdotL;
	}

	//ambient lightings (we now use IBL as the ambient term)!
	vec3 Ks = FrehNel(max(dot(N, V), 0.0), F0, u_roughness);
	vec3 Kd = vec3(1.0)-Ks;
	Kd *= (1.0 - u_metallic);
	vec3 environmentIrradiance = texture(u_IrradianceMap,N).rgb;
	vec3 diffuse = environmentIrradiance*u_Material.diffuseColor;
	vec3 ambient = (Kd*diffuse) * u_ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  

    FragColor = vec4(color, 1.0);
}

```

#### 3. IBLRenderer class



![Texture HdrLoad](/results/IBLDiffuse.jpg)


