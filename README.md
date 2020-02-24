###  ShadowMap multiple pointLights

#### 1.重构 ShadowMapPointLightRenderer 并添加多个PointLights的Shadow

ShadowMapPointLightRenderer .cpp

```
	void ShadowMapPointLightRenderer::RenderCubeMap(const std::vector<Object*>& objs, LightSources* lightSources)
	{
		float aspect = (float)s_ShadowMapPointLightWidth / (float)s_ShadowMapPointLightHeight;
		glm::mat4 pointLightProjection = glm::perspective(glm::radians(s_FOV), aspect, s_NearPlane, s_FarPlane);

			//TODO:多个PointLights
		GE_ASSERT(lightSources->GetPointLights().size() >0, "no pointlight found!");

		for (Object* pointLight:lightSources->GetPointLights())
		{
			std::shared_ptr<CubeMapTexture> shadowMap = pointLight->GetComponent<PointLight>()->GetShadowMap();
			
			m_FrameBuffer->Bind();
			m_FrameBuffer->AttachCubeMapDepthTexture(shadowMap);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			m_FrameBuffer->UnBind();
			glm::vec3 lightPos = pointLight->GetComponent<Transform>()->GetPosition();
			m_LightProjectionViewMatries[0] = (pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0f, 0.0)));
			m_LightProjectionViewMatries[1] = (pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0f, 0.0)));
			m_LightProjectionViewMatries[2] = (pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0f, 0.0), glm::vec3(0.0, 0.0, 1.0f)));
			m_LightProjectionViewMatries[3] = (pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0f, 0.0), glm::vec3(0.0, 0.0, -1.0f)));
			m_LightProjectionViewMatries[4] = (pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0, -1.0f, 0.0)));
			m_LightProjectionViewMatries[5] = (pointLightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0f), glm::vec3(0.0, -1.0f, 0.0)));


			glViewport(0, 0, shadowMap->GetWidth(), shadowMap->GetHeight());
			m_FrameBuffer->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);


			for (Object* obj : objs)
			{
				m_SimpleDepthShader->Bind();

				for (int i = 0; i < 6; i++)
					m_SimpleDepthShader->SetUniformMat4f("shadowMatrices[" + std::to_string(i) + "]", m_LightProjectionViewMatries[i]);
				m_SimpleDepthShader->SetUniform1f("u_FarPlane", s_FarPlane);
				m_SimpleDepthShader->SetUniformVec3f("u_LightPos", lightPos);

				obj->GetComponent<MeshRenderer>()->SetShaders(m_ShadowMapShader);
				DrawObject(obj, m_SimpleDepthShader);

			}

			m_FrameBuffer->UnBind();

		}
		

	}
	void ShadowMapPointLightRenderer::Render(const std::vector<Object*> backgroundObjs, LightSources* lightSources)
	{
		// 2. Render scene as normal 

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*glDisable(GL_CULL_FACE);
		m_Shader->SetUniformVec3f("u_LightPos", m_PointLight->GetComponent<BlackPearl::Transform>()->GetPosition());

		m_Shader->SetUniform1i("u_ReverseNormals", 1);
		m_BacisRender->DrawObject(Room);
		m_Shader->SetUniform1i("u_ReverseNormals", 0);

		glEnable(GL_CULL_FACE);
		m_Shader->Bind();


		m_Shader->SetUniform1f("u_FarPlane", BlackPearl::ShadowMapPointLightRenderer::s_FarPlane);*/

		for (Object* obj : backgroundObjs) {
			m_ShadowMapShader->Bind();
			GE_ASSERT(lightSources->GetPointLights().size() >= 1, "no pointlight found!");

			m_ShadowMapShader->SetUniform1f("u_FarPlane", s_FarPlane);

			unsigned int textureId = 0;
			for (Object* pointLight:lightSources->GetPointLights())
			{
				m_ShadowMapShader->SetUniform1i("u_ShadowMap["+std::to_string(textureId)+"]", textureId);
				glActiveTexture(GL_TEXTURE0+ textureId);
				pointLight->GetComponent<PointLight>()->GetShadowMap()->Bind();
				textureId++;
			}

			
			DrawObject(obj, m_ShadowMapShader);
		}
		
		DrawLightSources(lightSources);

	}
	
		
```

#### 2.每个PointLight都有一个ShadowMap,每个片元的渲染需要结合多个pointlght的shadowMap

CubeMapShadowMapping.glsl

```
#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!
uniform bool u_ReverseNormals;

/*
	这里由于使用的是CubeMap,
	可以直接使用一个方向向量采样深度值
	所以，不需要把vertex坐标转换到光空间下了
*/

void main(){
	gl_Position = u_ProjectionView* u_Model * vec4(aPos, 1.0);
	v_FragPos = vec3(u_Model * vec4(aPos, 1.0));
	if(u_ReverseNormals)
		v_Normal = transpose(inverse(mat3(u_Model))) * (-1.0*aNormal);
	else
		v_Normal =  transpose(inverse(mat3(u_Model))) * aNormal;

	v_TexCoords = aTexCoords;
}


#type fragment
#version 330 core

out vec4 FragColor;


in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoords;

uniform struct Material{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 emissionColor;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	sampler2D normal;
	sampler2D height;
	sampler2D depth;
	samplerCube cube;

	float shininess;
	bool isBlinnLight;
	int  isTextureSample;//判断是否使用texture,或者只有color


}u_Material;

struct PointLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	float constant;
	float linear;
	float quadratic;

};
uniform int u_PointLightNums;

uniform PointLight u_PointLights[20];
uniform samplerCube u_ShadowMap[20];
//uniform vec3 u_LightPos;
uniform vec3 u_CameraViewPos;
uniform float u_FarPlane ;
//uniform samplerCube u_CubeMap;
float ShadowCalculation(vec3 fragPos,vec3 lightPos,samplerCube shadowMap);
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir,samplerCube shadowMap);
void main(){
	vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	vec3 outColor ;//=vec3(0.2,0.3,0.9);
	//outColor = CalcParallelLight(u_ParallelLight,v_Normal,viewDir);
	//outColor += CalcSpotLight(u_SpotLight, v_Normal,viewDir);

	for(int i=0;i<u_PointLightNums;i++){

		outColor += CalcPointLight(u_PointLights[i], v_Normal,viewDir,u_ShadowMap[i]);

	}

	FragColor = vec4(outColor,1.0);//vec4(1.0,0.0,0.0,1.0);//


}
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir,samplerCube shadowMap){
	vec3 fragColor;

	float distance = length(light.position-v_FragPos);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	//ambient
	vec3 ambient = light.ambient*(  u_Material.ambientColor * (1-u_Material.isTextureSample)
					   + texture(u_Material.diffuse,v_TexCoords).rgb * u_Material.isTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;//u_LightColor * u_Material.ambient
	
	//diffuse
	vec3 lightDir = normalize(light.position-v_FragPos);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * ( u_Material.diffuseColor *(1-u_Material.isTextureSample)
					+ texture(u_Material.diffuse,v_TexCoords).rgb*u_Material.isTextureSample);//texture(u_Material.diffuse,v_TexCoord).rgb;// u_Material.diffuse);u_LightColor
	

//specular
	vec3 specular;
	float spec;
	if(u_Material.isBlinnLight){

		vec3 halfwayDir = normalize(lightDir+viewDir);
		spec = pow(max(dot(norm,halfwayDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  * ( u_Material.specularColor*(1-u_Material.isTextureSample)+texture(u_Material.specular,v_TexCoords).rgb*u_Material.isTextureSample);
	}
	else{

		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),u_Material.shininess);
		specular =  light.specular * spec  * ( u_Material.specularColor*(1-u_Material.isTextureSample)+ texture(u_Material.specular,v_TexCoords).rgb*u_Material.isTextureSample);//texture(u_Material.specular,v_TexCoord).rgb;
	}

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	float shadow = ShadowCalculation(v_FragPos,light.position,shadowMap);       


	fragColor = ambient+ (1.0 - shadow) *(diffuse + specular);// * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
		//fragColor = ambient+ (diffuse + specular);
	return fragColor;
}

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

```



![shadowMapMultiplePointLights](/results/shadowMapMultiplePointLights.png)
![shadowMapMultiplePointLights](/results/shadowMapMultiplePointLights1.png)


