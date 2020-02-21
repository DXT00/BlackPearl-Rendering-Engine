#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;


void main(){

	gl_Position = u_ProjectionView*u_Model*vec4(aPos,1.0);
	
	
	v_TexCoord = aTexCoords;
	v_Normal   = mat3(u_TranInverseModel)*aNormal;
	v_FragPos  = vec3(u_Model*vec4(aPos,1.0));




}




#type fragment
#version 430 core

/* render to gBuffer */
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gSpecular;

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

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
	
	float shininess;
	bool isBlinnLight;
	int  isTextureSample;//判断是否使用texture,或者只有color

}u_Material;

void main(){
    // Store the fragment position vector in the first gbuffer texture
	gPosition       = v_FragPos;
	// Also store the per-fragment normals into the gbuffer
	gNormal         = v_Normal;

	gAlbedoSpec.rgb = (u_Material.diffuseColor *(1-u_Material.isTextureSample)
					+ texture(u_Material.diffuse,v_TexCoord).rgb*u_Material.isTextureSample);


	gSpecular.rgb = (u_Material.specularColor *(1-u_Material.isTextureSample)
					+ texture(u_Material.specular,v_TexCoord).rgb*u_Material.isTextureSample);


	
}






