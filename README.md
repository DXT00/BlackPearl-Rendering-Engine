### IBL
1.assimp 加载.hdr文件

2.修改CubeMapTexture API

```

CubeMapTexture(Type type, std::vector<std::string> faces, int internalFormat, int format, int dataType)
	:Texture(type, faces) {
	LoadCubeMap(faces,internalFormat,format,dataType);
};
//empty CubeMap,use in CubeShadowMap
CubeMapTexture(Type type, const int width, const int height,int internalFormat,int format,int dataType)
	:Texture(type, std::vector<std::string>()) {
	LoadCubeMap(width,height, internalFormat, format, dataType);
};

```
3.把 HDRTexture 映射到 CubeMap,CubeMap Attach到FrameBuffer中

hdrMapToCube.glsl

```
#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_Model;
uniform mat4 u_CubeMapProjectionView;

out vec3 v_LocalPos;
void main()
{
	v_LocalPos = aPos;
	gl_Position = u_CubeMapProjectionView*vec4(aPos,1.0);

}

#type fragment
#version 430 core

uniform sampler2D hdrTexture;
out vec4 FragColor;
in vec3 v_LocalPos;
const vec2 invAtan = vec2(0.1591, 0.3183);

//TODO::还没搞清楚
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{       
    vec2 uv = SampleSphericalMap(normalize(v_LocalPos)); // make sure to normalize localPos
    vec3 color = texture(hdrTexture, uv).rgb;

    FragColor = vec4(color, 1.0);
}

```


IBLRenderer.cpp

```
void IBLRenderer::Init(Object * cubeObj)
{
	//....
	glViewport(0, 0, s_Width, s_Height);
	m_FrameBuffer->Bind();
	for (unsigned int i = 0; i < 6; i++)
	{
			m_HdrMapToCubeShader->SetUniformMat4f("u_CubeMapProjectionView", captureProjectionViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,m_FrameBuffer->GetCubeMapColorTexture()->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_CubeObj->GetComponent<MeshRenderer>()->SetShaders(m_HdrMapToCubeShader);

			DrawObject(m_CubeObj, m_HdrMapToCubeShader);
	}
	m_FrameBuffer->UnBind();

		// initialize static shader uniforms before rendering
	m_IsInitialize = true;
	}
```


Hdr file's link: http://www.hdrlabs.com/sibl/archive.html
![Texture HdrLoad](/results/HdrLoad.png)


