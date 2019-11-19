

1.使用FrameBuffer 存储深度值 --》ShadowMap 960x540x16bit

2.注意 VeiwProjection 是平行光的ViewProjection,所有的物体都要变换到光空间中，使用DepthShader_ShadowMapLayer.glsl:

```
#type vertex
#version 330 core
layout (location =0) in vec3 aPos;

uniform mat4 lightProjectionViewMatrix;
uniform mat4 u_Model;

void main(){
	//将模型中的顶点变换到光空间
	gl_Position = lightProjectionViewMatrix*u_Model*vec4(aPos,1.0f);
}
#type fragment
#version 330 core
void main(){
	//这个空像素着色器什么也不干，运行完后，深度缓冲会被更新
	gl_FragDepth = gl_FragCoord.z;
}
```

3.通过Quad 获取FrameBuffer中的DepthTextureAttachment -->即ShadowMap 作为自己的Texture ,Quad_ShadowMapLayer.glsl:

```
#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 v_TexCoords;

void main(){
	v_TexCoords = aTexCoords;
	gl_Position = vec4(aPos,1.0);
}

#type fragment
#version 330 core

out vec4 FragColor;
in  vec2 v_TexCoords;


struct Material{
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

	float shininess;

};
uniform Material u_Material;
void main(){
	
	float depthValue = texture(u_Material.depth,v_TexCoords).r;
	FragColor = vec4(vec3(depthValue),1.0);


}
```