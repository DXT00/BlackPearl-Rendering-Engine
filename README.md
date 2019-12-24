### PBR Texture

1.Add Texture for Sphere

2.Write PBR Texture shader:

fragment Shader:

```
void main(){

	vec3 albedo =pow(texture(u_Material.diffuse, v_TexCoord).rgb, vec3(2.2));//vec3(pow( texture(u_Material.diffuse, v_TexCoord).r, (2.2)));

	float mentallic = texture(u_Material.mentallic,v_TexCoord).r;
	float roughness  = texture(u_Material.roughness ,v_TexCoord).r;
	float ao        = texture(u_Material.ao, v_TexCoord).r;


	vec3 N = getNormalFromMap();
	vec3 V = normalize(u_CameraViewPos-v_FragPos);

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
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  

    FragColor = vec4(color, 1.0);
}


```

Texture link: https://freepbr.com/materials/curved-cobblestone-2/

![Texture PBR](/results/pbrTexture.png)


