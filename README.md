

1.创建天空盒子

注意点：

Camera 的View Matrix的平移向量不再对天空盒子生效，天空盒子只有旋转向量

skybox.glsl:

```
void main()
{
	TexCoords = aPos;
	vec4 pos = u_Projection*mat4(mat3(u_View))* vec4(aPos,1.0);
	gl_Position = pos.xyww;
}

```

绘制顺序也要注意：

先绘制别的物体-->设置glDepthFunc(GL_LEQUAL)-->绘制天空盒子

SkyBoxTestLayer.h

```
	BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
	BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		
	std::vector<BlackPearl::Object*> objs;
	objs.push_back(m_SkyBoxObj);

	DrawObjectsExcept(objs);
	//小于等于当前深度缓冲的fragment才被绘制
	glDepthFunc(GL_LEQUAL);
	DrawObject(m_SkyBoxObj);

	glDepthFunc(GL_LESS);
```