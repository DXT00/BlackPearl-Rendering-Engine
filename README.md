### 添加ShadowBox

优化渲染范围，找到Camera的frustum,计算ShadowBox（AABB）
阴影范围半径可通过	float ShadowBox::s_ShadowDistance = 30.0f修改

ShadowMapRenderer.cpp
```
void ShadowMapRenderer::Render(const std::vector<Object*>& objs,  ParallelLight* sun, const std::vector<Object*>&exceptObjs)
	{
		m_LightPos = sun->GetDirection();
		m_ShadowBox->Update();
		UpdateLightOrthoProjectionMatrix(m_ShadowBox->GetWidth(), m_ShadowBox->GetHeight(), m_ShadowBox->GetLength());
		UpdateLightViewMatrix(sun->GetDirection(), m_ShadowBox->GetCenter());
		m_LightProjectionViewMatrix = m_LightProjection * m_LightView;

		glViewport(0, 0, s_ShadowMapWidth, s_ShadowMapHeight);
		m_FrameBuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		if (exceptObjs.empty())
			DrawObjects(objs);
		else
			DrawObjectsExcept(objs, exceptObjs);
		m_FrameBuffer->UnBind();

	}
```

### 改善阴影

对ShadowMap采用时取周围的9个Texel的平均：
directLight/ShadowMaping_ShadowMapLayer.glsl
```
float ShadowCalculation(vec4 fragPosLightSpace,vec3 normal,vec3 lightDir)
{
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;//透视除法，变换到[-1.0,1.0]
   projCoords = projCoords * 0.5 + 0.5;//变换到[0,1]坐标

   float closestDepth = texture(u_Material.depth,projCoords.xy).r;//从shadowMap 取最小的z值

   float currentDepth = projCoords.z;
   float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

   ///////////////////////////////////////////////////////////////////
   ////////////////////从纹理像素四周对深度贴图采样，然后把结果平均起来///////////////////
   float shadow = 0.0;
   vec2 texelSize = 1.0/textureSize(u_Material.depth,0);
   for(int x = -1;x<=1;x++){
	for(int y= -1;y<=1;y++){
		float pcfDepth = texture(u_Material.depth,projCoords.xy+vec2(x,y)*texelSize).r;
		shadow +=currentDepth-bias>pcfDepth?1.0:0.0;
	}
   }
   shadow = shadow/9.0;
   ///////////////////////////////////////////////////////////////////////////////////


//    //比较当前深度和最近采样点深度
 //   float shadow = (currentDepth-bias) > closestDepth? 1.0 : 0.1;

   return shadow;
}


```
###修改 PointLight ShadowMap 的Bug

-->没更新LightProjectionViewMatrix
```
void ShadowMapPointLightRenderer::Render(const std::vector<Object*>& objs, Object* pointLight,const std::vector<Object*>& exceptObjs)
	{
		float aspect = (float)s_ShadowMapPointLightWidth / (float)s_ShadowMapPointLightHeight;
		glm::mat4 pointLightProjection = glm::perspective(glm::radians(s_FOV), aspect, s_NearPlane, s_FarPlane);
		
		GE_ASSERT(pointLight->HasComponent<PointLight>(), "this object is not a pointlight!");
		m_LightPos = pointLight->GetComponent<Transform>()->GetPosition();
		
		m_LightProjectionViewMatries[0]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0f, 0.0)));
		m_LightProjectionViewMatries[1]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0f, 0.0)));
		m_LightProjectionViewMatries[2]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 1.0f, 0.0), glm::vec3(0.0, 0.0, 1.0f)));
		m_LightProjectionViewMatries[3]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, -1.0f, 0.0), glm::vec3(0.0, 0.0, -1.0f)));
		m_LightProjectionViewMatries[4]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0, -1.0f, 0.0)));
		m_LightProjectionViewMatries[5]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, -1.0f), glm::vec3(0.0, -1.0f, 0.0)));
		
		glViewport(0, 0, s_ShadowMapPointLightWidth, s_ShadowMapPointLightHeight);
		m_FrameBuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		if (exceptObjs.empty())
			DrawObjects(objs);
		else
			DrawObjectsExcept(objs, exceptObjs);
		m_FrameBuffer->UnBind();

	}
```
![DynamicLight](/results/DynamicLight.png)
![shadowAverage9](/results/shadowAverage9.png)
![shadowMap3](/results/shadowMap3.png)
