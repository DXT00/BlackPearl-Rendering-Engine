添加衰减(Attenuation)设置，根据距离设置光线衰减范围
多光源叠加：一个SpotLight,三个PointLight:

void main(){
	//根据Application中设置的光源选择
	vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	vec3 outColor;
	outColor = CalcParallelLight(u_ParallelLight,v_Normal,viewDir);
	outColor += CalcSpotLight(u_SpotLight, v_Normal,viewDir);

	for(int i=0;i<u_PointLightNums;i++){

	outColor += CalcPointLight(u_PointLights[i], v_Normal,viewDir);

	}

	FragColor = vec4(outColor,1.0);
}