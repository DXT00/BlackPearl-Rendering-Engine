#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->�����cpu�������ٴ�����!
uniform mat4 u_ProjectionView;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;
void main()
{
	gl_Position = vec4(aPos, 1.0); //u_ProjectionView * u_Model * vec4(aPos, 1.0);
	v_FragPos = vec3(u_Model * vec4(aPos, 1.0));
	v_TexCoord = vec2(aTexCoords.x, aTexCoords.y);
	v_Normal = mat3(u_TranInverseModel) * aNormal;//vec3(u_Model * vec4(aNormal,1.0));

}

#type fragment
#version 430 core

#define PI 3.14159
struct PointLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	float constant;
	float linear;
	float quadratic;

};
//uniform struct Material {
//	vec3 ambientColor;
//	vec3 diffuseColor;
//	vec3 specularColor;
//	vec3 emissionColor;
//	sampler2D diffuse; //or call it albedo
//	sampler2D specular;
//	sampler2D emission;
//	sampler2D normal;
//	sampler2D height;
//	sampler2D ao;
//	sampler2D roughness;
//	sampler2D mentallic;
//
//
//	float shininess;
//	bool isBlinnLight;
//	int  isTextureSample;//�ж��Ƿ�ʹ��texture,����ֻ��color
//
//}u_Material;
uniform Material u_Material;
uniform sampler2D u_CameraDepthTexture;
uniform sampler2D u_FinalScreenTexture;
uniform sampler3D u_Texture3DNoise;
uniform sampler2D m_WeatherTexture;
uniform float	  u_TextureScale;
uniform float u_densityOffset;
uniform float u_lightAbsorptionTowardSun;
uniform float u_densityMultiplier;

uniform vec3 u_boundsMax;
uniform vec3 u_boundsMin;

in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoord;

out vec4 FragColor;
uniform PointLight u_PointLights[100];
uniform int u_PointLightNums;
uniform vec3 u_CameraViewPos;
uniform mat4 u_InverseProjectionMatrix;
uniform mat4 u_InverseViewMatrix;

uniform vec3 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_ao;

uniform float u_rayStep;
uniform float u_step;

uniform vec3 u_colA;
uniform vec3 u_colB;
uniform float u_colorOffset1;
uniform float u_colorOffset2;

int step = 100;
vec3 sum = vec3(0.0);
float dw = 1.0 / step;
float kd = 0.1;
float ks = 0.1;

//��������ռ�����
vec4 GetWorldSpacePosition(float depth, vec2 uv)
{
	// ��Ļ�ռ� --> ��׶�ռ�
	vec2 uv_ = 2.0 * uv - 1.0;
	vec4 view_vector = u_InverseProjectionMatrix * vec4(uv_.x, uv_.y, depth, 1.0);
	view_vector.xyz = view_vector.xyz / view_vector.w;
	//��׶�ռ� --> ����ռ�
	mat4 l_matViewInv = u_InverseViewMatrix;
	vec4 world_vector = (l_matViewInv * vec4(view_vector.xyz, 1));
	return world_vector;
}

float saturate(float val) {
	return clamp(val, 0.0, 1.0);
}

float calculateAttenuation(PointLight light, vec3 fragPos) {
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	return attenuation;
}

float NoemalDistribution_TrowbridgeReitz_GGX(vec3 N, vec3 H, float roughness) {
	float roughness2 = roughness * roughness;
	float NHDOT = max(abs(dot(N, H)), 0.0);
	float tmp = (NHDOT * NHDOT) * (roughness2 - 1.0) + 1.0;
	float NDF = roughness2 / (PI * tmp * tmp);
	return NDF;
}

float Geometry_SchlickGGX(float NdotV, float roughness) {
	return NdotV / (NdotV * (1.0 - roughness) + roughness);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx1 = Geometry_SchlickGGX(NdotV, roughness);
	float ggx2 = Geometry_SchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 FrehNel(float NdotV, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);
	// F0 + pow(1.0 - NdotV, 5.0) -F0*pow(1.0 - NdotV, 5.0)
	//F0*(1-pow(1.0 - NdotV, 5.0)) + pow(1.0 - NdotV, 5.0)
}

vec3 BRDF(vec3 Kd, vec3 Ks, vec3 specular) {

	vec3 fLambert = u_Material.diffuseColor / PI;//diffuseColor �൱�� albedo
	return Kd * fLambert + specular;//specular ���Ѿ���Ks(Ks=F)�ˣ�����Ҫ�ٳ���Ks *
}
vec3 LightRadiance(vec3 fragPos, PointLight light) {
	float attenuation = calculateAttenuation(light, fragPos);
	//float cosTheta = max(dot(N,wi),0.0);
	vec3 radiance = light.diffuse * attenuation;
	return radiance;
}


//void main() {
//	vec3 N = normalize(v_Normal);
//	vec3 V = normalize(u_CameraViewPos - v_FragPos);
//
//	vec3 F0 = vec3(0.04);
//	F0 = mix(F0, u_Material.diffuseColor, u_metallic);
//
//	//reflection equation
//	vec3 Lo = vec3(0.0);
//	for (int i = 0; i < u_PointLightNums; i++) {
//		vec3 L = normalize(u_PointLights[i].position - v_FragPos);
//		vec3 H = normalize(V + L);
//		float attenuation = calculateAttenuation(u_PointLights[i], v_FragPos);
//		vec3 radiance = u_PointLights[i].diffuse * attenuation;
//
//		float NDF = NoemalDistribution_TrowbridgeReitz_GGX(N, H, u_roughness);
//		float G = GeometrySmith(N, V, L, u_roughness);
//		vec3 F = FrehNel(max(dot(H, V), 0.0), F0);
//
//		vec3 Ks = F;
//		vec3 Kd = vec3(1.0) - Ks;
//		Kd *= (1.0 - u_metallic);
//
//		//CookTorrance
//		vec3 nominator = NDF * G * F;
//		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
//		vec3 specular = nominator / denominator;
//
//		float NdotL = max(dot(N, L), 0.0);
//		Lo += BRDF(Kd, Ks, specular) * LightRadiance(v_FragPos, u_PointLights[i]) * NdotL;
//	}
//	vec3 ambient = vec3(0.03) * u_Material.diffuseColor * u_ao;
//	vec3 color = ambient + Lo;
//
//	color = color / (color + vec3(1.0));
//	color = pow(color, vec3(1.0 / 2.2));
//
//	FragColor = vec4(color, 1.0);
//}
float cloudRayMarching(vec3 startPoint, vec3 direction)
{
	vec3 testPoint = startPoint;
	float sum = 1.0;
	direction *= 0.5;//ÿ�β������
	for (int i = 0; i < 256; i++)//�����ܳ���
	{
		testPoint += direction;
		if (testPoint.x < 10.0 && testPoint.x > -10.0 &&
			testPoint.z < 10.0 && testPoint.z > -10.0 &&
			testPoint.y < 10.0 && testPoint.y > -10.0)
			sum += 0.01;
	}
	return sum;
}

// case 1: ���ߴ��ⲿ�ཻ (0 <= dstA <= dstB)
// dstA��dst������Ľ���㣬dstB dst��Զ����
// case 2: ���ߴ��ڲ��ཻ (dstA < 0 < dstB)
// dstA��dst�����ߺ��ཻ��, dstB��dst�����򽻼�
// case 3: ����û���ཻ (dstA > dstB)
// 
//�߽����Сֵ       �߽�����ֵ         
vec2 rayBoxDst(vec3 boundsMin, vec3 boundsMax,
	//�������λ��      ��������ռ���߷���
	vec3 rayOrigin, vec3 invRaydir)
{
	vec3 t0 = (boundsMin - rayOrigin) * invRaydir;
	vec3 t1 = (boundsMax - rayOrigin) * invRaydir;
	vec3 tmin = min(t0, t1);
	vec3 tmax = max(t0, t1);

	float dstA = max(max(tmin.x, tmin.y), tmin.z); //�����
	float dstB = min(tmax.x, min(tmax.y, tmax.z)); //��ȥ��

	float dstToBox = max(0, dstA);
	float dstInsideBox = max(0, dstB - dstToBox);
	return vec2(dstToBox, dstInsideBox);
}

vec3 sampleDensity(vec3 rayPos)
{
	vec3 uvw = mod(rayPos, u_TextureScale) / u_TextureScale;// *u_TextureScale;
	vec4 shapeNoise = textureLod(u_Texture3DNoise, uvw*2.0, 0);
	return shapeNoise.rgb;
}
float remap(float original_value, float original_min, float original_max, float new_min, float new_max)
{
	return new_min + (((original_value - original_min) / (original_max - original_min)) * (new_max - new_min));
}

float sampleDensityFinal(vec3 rayPos)
{
	vec3 boundsCentre = (u_boundsMax + u_boundsMin) * 0.5;
	vec3 size = u_boundsMax - u_boundsMin;
	/*float speedShape = _Time.y * _xy_Speed_zw_Warp.x;
	float speedDetail = _Time.y * _xy_Speed_zw_Warp.y;*/

	/*vec3 uvwShape = rayPos * _shapeTiling + vec3(speedShape, speedShape * 0.2, 0);
	vec3 uvwDetail = rayPos * _detailTiling + vec3(speedDetail, speedDetail * 0.2, 0);*/

	vec2 uv = (size.xz * 0.5f + (rayPos.xz - boundsCentre.xz)) / max(size.x, size.z);

	//vec4 maskNoise = tex2Dlod(_maskNoise, vec4(uv + vec2(speedShape * 0.5, 0), 0, 0));
	//vec4 weatherMap = tex2Dlod(_weatherMap, vec4(uv + vec2(speedShape * 0.4, 0), 0, 0));
	vec4 weatherMap = texture(m_WeatherTexture, uv);

	vec3 uvw = mod(rayPos, u_TextureScale) / u_TextureScale;// *u_TextureScale;
	vec4 shapeNoise = textureLod(u_Texture3DNoise, uvw, 0);

	//vec4 shapeNoise = texture(_noiseTex, vec4(uvwShape + (maskNoise.r * _xy_Speed_zw_Warp.z * 0.1), 0));
	//vec4 detailNoise = tex3Dlod(_noiseDetail3D, vec4(uvwDetail + (shapeNoise.r * _xy_Speed_zw_Warp.w * 0.1), 0));

	//��Ե˥��
	const float containerEdgeFadeDst = 100;
	float _heightWeights = 0.5;
	float dstFromEdgeX = min(containerEdgeFadeDst, min(rayPos.x - u_boundsMin.x, u_boundsMax.x - rayPos.x));
	float dstFromEdgeZ = min(containerEdgeFadeDst, min(rayPos.z - u_boundsMin.z, u_boundsMax.z - rayPos.z));
	float edgeWeight = min(dstFromEdgeZ, dstFromEdgeX) / containerEdgeFadeDst;

	float gMin = remap(weatherMap.x, 0, 1, 0.1, 0.6);
	float gMax = remap(weatherMap.x, 0, 1, gMin, 0.9);
	float heightPercent = (rayPos.y - u_boundsMin.y) / size.y;
	float heightGradient = saturate(remap(heightPercent, 0.0, gMin, 0, 1)) * saturate(remap(heightPercent, 1, gMax, 0, 1));
	float heightGradient2 = saturate(remap(heightPercent, 0.0, weatherMap.r, 1, 0)) * saturate(remap(heightPercent, 0.0, gMin, 0, 1));
	heightGradient = saturate(mix(heightGradient, heightGradient2, _heightWeights));

	heightGradient *= edgeWeight;
	vec4 _shapeNoiseWeights = vec4(-0.17, 27.17, -3.65, -0.08);
	vec4 normalizedShapeWeights = _shapeNoiseWeights / dot(_shapeNoiseWeights, vec4(1));
	float shapeFBM = dot(shapeNoise, normalizedShapeWeights) * heightGradient;
	float baseShapeDensity = shapeFBM + u_densityOffset * 0.01;
	if (baseShapeDensity > 0)
	{
		/*float detailFBM = pow(detailNoise.r, _detailWeights);
		float oneMinusShape = 1 - baseShapeDensity;
		float detailErodeWeight = oneMinusShape * oneMinusShape * oneMinusShape;*/
		float cloudDensity = baseShapeDensity;// -detailFBM * detailErodeWeight * _detailNoiseWeight;

		return saturate(cloudDensity * u_densityMultiplier);
	}
	return 0;
}
vec3 lightmarch(vec3 position, float dstTravelled)
{
	for (int i = 0; i < u_PointLightNums; i++) {

		vec3 dirToLight = normalize(u_PointLights[i].position - v_FragPos);
		//�ƹⷽ����߽���󽻣��������ֲ�����
		float dstInsideBox = rayBoxDst(u_boundsMin, u_boundsMax, position, 1 / dirToLight).y;
		float stepSize = dstInsideBox / u_boundsMin.x;
		float totalDensity = 0;
		//u_colA = vec3(1.0, 0.64274913, 0.3443396);
		//u_colB = vec3(0.4717871, 0.54310095, 0.5849056);
		
		
		float _darknessThreshold = 0.1;
		for (int step = 0; step < 8; step++) //�ƹⲽ������
		{
			position += dirToLight * stepSize; //��ƹⲽ��
			totalDensity += max(0, sampleDensity(position).r * stepSize); //������ʱ����������ۼ��ܵƹ�Ӱ���ܶ�
		}
		float transmittance = exp(-totalDensity * u_lightAbsorptionTowardSun);
		//����������ӳ��Ϊ 3����ɫ ,��->�ƹ���ɫ ��->ColorA ��->ColorB
		vec3 cloudColor = mix(u_colA, u_PointLights[i].diffuse, saturate(transmittance * u_colorOffset1));
		cloudColor = mix(u_colB, cloudColor, saturate(pow(transmittance * u_colorOffset2, 3)));
		return transmittance * cloudColor;//vec3(_darknessThreshold) + transmittance * (1 - _darknessThreshold) * cloudColor;

	}
	return vec3(0);
}

void main() {
	vec3 color = texture(u_FinalScreenTexture, v_TexCoord).rgb;

	//������Ļ���
	float depth = texture(u_CameraDepthTexture, v_TexCoord).x;
	//����ռ�����
	vec4 worldPos = GetWorldSpacePosition(depth, v_TexCoord);
	vec3 rayPos = u_CameraViewPos;
	//�����ÿ�����ص����緽��
	vec3 worldViewDir = normalize(worldPos.xyz - rayPos.xyz);
	//vec3 color = vec3(1.0);
	float depthEyeLinear = length(worldPos.xyz - u_CameraViewPos);
	vec2 rayToContainerInfo = rayBoxDst(u_boundsMin, u_boundsMax, rayPos, (1 / worldViewDir));
	float dstToBox = rayToContainerInfo.x; //����������ľ���
	float dstInsideBox = rayToContainerInfo.y; //���ع����Ƿ���������

	//���������ľ��� - ����������ľ��룬����� �����Ƿ��������� ȡ��С�����˵�һЩ��Чֵ
	float dstLimit = min(depthEyeLinear - dstToBox, dstInsideBox);

	// �����������Ľ����
	vec3 entryPoint = rayPos + worldViewDir * dstToBox;

	float sumDensity =1.0;
	float _dstTravelled = 0;
	//float u_rayStep = 1.2;
	//float u_step = 1.2;
	vec3 lightEnergy = vec3(0);
	float stepSize = exp(u_step) * u_rayStep;

	for (int j = 0; j < u_TextureScale; j++)
	{
		if ( _dstTravelled < dstLimit) //����סʱ��������
		{
			rayPos = entryPoint + (worldViewDir * _dstTravelled);
			float density = sampleDensityFinal(rayPos).r;
			//sumDensity += pow(sampleDensity(rayPos), 5); //sampleDensity(rayPos);// 0.05;//
			//if (sumDensity > 1.0)
			//	break;
			if (density > 0)
			{
				/*vec3 lightTransmittance = lightmarch(rayPos, dstTravelled);
				lightEnergy += density * stepSize * sumDensity * lightTransmittance * phaseVal;*/
				vec3 lightTransmittance = lightmarch(rayPos, _dstTravelled);
				lightEnergy += density * stepSize * sumDensity * lightTransmittance;
				sumDensity *= exp(-density * stepSize); // exp(-density * stepSize);

				if (sumDensity < 0.1)
					break;
			}
			
		}
		_dstTravelled += stepSize; //ÿ�β�������
	}
	vec3 density1 = sampleDensity(rayPos);
	float cloud = sumDensity;// cloudRayMarching(u_CameraViewPos.xyz, worldViewDir);
	vec4 cloudColor = vec4(lightEnergy, sumDensity);
	color.rgb *= cloudColor.a;
	color.rgb += cloudColor.rgb;
	/*if (lightEnergy.r <= 0 && lightEnergy.g <= 0 && lightEnergy.b<= 0) {
		FragColor = vec4(color, 1.0);

	}
	else {*/
	FragColor = vec4(color, cloudColor.a);// vec4(lightEnergy, sumDensity);
		//FragColor = vec4(vec3(sumDensity), 1.0);
		//FragColor = vec4(density1.r, density1.g, density1.b, 1.0);

		//FragColor = vec4(color * cloud + vec3(0.1), 1.0);// vec4(color * 0.7 + cloud * 0.3, 1.0);// 

	//}

	//FragColor = vec4(color, 1.0);
}
