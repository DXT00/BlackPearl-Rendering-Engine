### LightProbes -- 融合多个lightProbes渲染,背景为SkyBox


#### 1.对于每个要渲染的物体，采集离它最近的k个probes,混合k个probes的DiffuseMap和SpecularMap进行渲染

IBLProbesRenderer.cpp RenderSpecularObjects:

```
		void IBLProbesRenderer::RenderSpecularObjects(const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes)
	{

		for (Object* obj : objects) {

			glm::vec3 pos = obj->GetComponent<Transform>()->GetPosition();

			std::vector<LightProbe*> kProbes = FindKnearProbes(pos, probes);

			unsigned int k = m_K;
			std::vector<unsigned int> distances;
			unsigned int distancesSum = 0;
			for (auto probe : kProbes)
			{
				distances.push_back(glm::length(probe->GetPosition() - pos));
				distancesSum += glm::length(probe->GetPosition() - pos);
			}
			m_IBLShader->Bind();
			m_IBLShader->SetUniform1i("u_Kprobes", k);
			unsigned int textureK = 0;
			for (int i = 0; i < k; i++)
			{
				m_IBLShader->SetUniform1f("u_ProbeWeight["+std::to_string(i)+"]", (float)distances[i]/distancesSum);
				
				m_IBLShader->SetUniform1i("u_IrradianceMap[" + std::to_string(i) + "]", textureK);
				glActiveTexture(GL_TEXTURE0+ textureK);
				kProbes[i]->GetDiffuseIrradianceCubeMap()->Bind();
				textureK++;
				m_IBLShader->SetUniform1i("u_PrefilterMap[" + std::to_string(i) + "]", textureK);
				glActiveTexture(GL_TEXTURE0 + textureK);
				kProbes[i]->GetSpecularPrefilterCubeMap()->Bind();
				textureK++;
			
			}


			m_IBLShader->SetUniform1i("u_BrdfLUTMap", textureK);
			glActiveTexture(GL_TEXTURE0 + textureK);
			m_SpecularBrdfLUTTexture->Bind();



			DrawObject(obj, m_IBLShader);


		}

	}
```

#### 2.在lightProbes/iblTexture.glsl中对多个probes的 Map进行融合，权重为当前object里每个probe的距离的归一化


```
#type fragment
#version 430 core

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
uniform struct Material{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 emissionColor;
	sampler2D diffuse; //or call it albedo
	sampler2D specular;
	sampler2D emission;
	sampler2D normal;
	sampler2D height;
	sampler2D ao;
	sampler2D roughness;
	sampler2D mentallic;


	float shininess;
	bool isBlinnLight;
	int  isTextureSample;//判断是否使用texture,或者只有color

}u_Material;


in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoord;

out vec4 FragColor;
//Light
uniform PointLight u_PointLights[100];
uniform int u_PointLightNums;
//Camera
uniform vec3 u_CameraViewPos;

//IBL probes
//目前最多10个probes
uniform samplerCube u_IrradianceMap[10];
uniform samplerCube u_PrefilterMap[10];
uniform sampler2D u_BrdfLUTMap;

uniform int u_Kprobes = 0;

uniform float u_ProbeWeight[10];




int step = 100;
vec3 sum  = vec3(0.0);
float dw = 1.0/step;
float kd=0.1;
float ks=0.1;


//......

//......
void main(){
	//material properties
	vec3 albedo = pow(texture(u_Material.diffuse,v_TexCoord).rgb,vec3(2.2));
	float metallic = texture(u_Material.mentallic, v_TexCoord).r;
    float roughness = texture(u_Material.roughness, v_TexCoord).r;
    float ao = texture(u_Material.ao, v_TexCoord).r;
       
	vec3 N = getNormalFromMap();
	vec3 V = normalize(u_CameraViewPos-v_FragPos);
	vec3 R = reflect(-V,N);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0,albedo,metallic);

	//reflection equation
	vec3 Lo = vec3(0.0);
	for(int i=0;i<u_PointLightNums;i++){
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
		Lo+= BRDF(Kd,Ks,specular)*LightRadiance(v_FragPos,u_PointLights[i])*NdotL;
	}

	//ambient lightings (we now use IBL as the ambient term)!
	vec3 F =  FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 Ks = F;
	vec3 Kd = vec3(1.0)-Ks;
	Kd *= (1.0 - metallic);
	vec3 environmentIrradiance ;
	for(int i=0;i<u_Kprobes;i++){
		environmentIrradiance+= u_ProbeWeight[i]*texture(u_IrradianceMap[i],N).rgb;
	}
	vec3 diffuse = environmentIrradiance*u_Material.diffuseColor;

	//sample both the prefilter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part
	const float MAX_REFLECTION_LOD = 4.0;
	//sample MAX_REFLECTION_LOD level mipmap everytime !
	vec3 prefileredColor;
	for(int i=0;i<u_Kprobes;i++){
		prefileredColor+= u_ProbeWeight[i]*textureLod(u_PrefilterMap[i],R,roughness*MAX_REFLECTION_LOD).rgb;
	}
	vec2 brdf = texture(u_BrdfLUTMap,vec2(max(dot(N,V),0.0),roughness)).rg;

	vec3 specular = prefileredColor * (F*brdf.x+brdf.y);
	vec3 ambient = (Kd*diffuse+specular) * ao;
    vec3 color =ambient + Lo;

	//HDR tonemapping
    color = color / (color + vec3(1.0));
	//gamma correction
    color = pow(color, vec3(1.0/2.2));  
    FragColor = vec4(color, 1.0);
}
	
```
#### 3.注意SkyBox在EnvironmentMap中的渲染顺序

```

void IBLProbesRenderer::RenderEnvironmerntCubeMaps(const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probe, Object* skyBox)
	{
		glm::vec3 center = probe->GetPosition();
		probe->UpdateCamera();
		auto camera = probe->GetCamera();
		auto cameraComponent = camera->GetObj()->GetComponent<PerspectiveCamera>();

		auto projection = cameraComponent->GetProjectionMatrix();
		//	cameraComponent->SetPosition(probe->GetPosition());

		std::vector<glm::mat4> ProbeView = {
		   glm::lookAt(center, center - camera->Front(),-camera->Up()),
		   glm::lookAt(center, center + camera->Front(),-camera->Up()),
		   glm::lookAt(center, center + camera->Up(),-camera->Right()),//-camera->Front()
		   glm::lookAt(center, center - camera->Up(),camera->Right()),//camera->Front()

		   glm::lookAt(center, center - camera->Right(), -camera->Up()),
		   glm::lookAt(center, center + camera->Right(), -camera->Up()),

		};
		std::vector<glm::mat4> ProbeProjectionViews = {
			projection * ProbeView[0],
			projection * ProbeView[1],
			projection * ProbeView[2],
			projection * ProbeView[3],
			projection * ProbeView[4],
			projection * ProbeView[5]
		};


		std::shared_ptr<CubeMapTexture> environmentCubeMap = probe->GetHdrEnvironmentCubeMap();
		//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glm::vec2 mipMapSize = { environmentCubeMap->GetWidth(),environmentCubeMap->GetHeight() };
		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());

		frameBuffer->Bind();
		frameBuffer->AttachCubeMapColorTexture(0, environmentCubeMap);
		
		frameBuffer->AttachRenderBuffer(environmentCubeMap->GetWidth(), environmentCubeMap->GetHeight());
	
		frameBuffer->BindRenderBuffer();
		
		glViewport(0, 0, mipMapSize.x, mipMapSize.y);
		
		for (unsigned int i = 0; i < 6; i++)
		{
			Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetPosition(),cameraComponent->Front(),lightSources });
			
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environmentCubeMap->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//BasicRenderer::DrawLightSources(lightSources, scene);
			glDepthFunc(GL_LEQUAL);
			DrawObject(skyBox, scene);
			glDepthFunc(GL_LESS);

			DrawObjects(objects, scene);
			delete scene;
			scene = nullptr;

		}

		
	}
		frameBuffer->UnBind();
		frameBuffer->CleanUp();


	}
```

![lightProbesBlending](/results/lightProbesBlending1.png)
![lightProbesBlending](/results/lightProbesBlending2.png)
![lightProbesBlending](/results/lightProbesBlending3.png)
![lightProbesBlending](/results/lightProbesBlending4.png)



