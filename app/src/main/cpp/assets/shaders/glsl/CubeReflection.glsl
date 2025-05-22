#type vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_View;		
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!


void main()
{
	v_Normal = mat3(u_TranInverseModel)* aNormal;
	v_FragPos = vec3(u_Model* vec4(aPos,1.0));
	gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);
}



#type fragment
#version 330 core
out vec4 FragColor;

in vec3 v_Normal;
in vec3 v_FragPos;

//struct Material{
//	vec3 ambientColor;
//	vec3 diffuseColor;
//	vec3 specularColor;
//	vec3 emissionColor;
//	sampler2D diffuse;
//	sampler2D specular;
//	sampler2D emission;
//	sampler2D normal;
//	sampler2D height;
//	samplerCube cube;
//
//	float shininess;
//
//};
uniform Material u_Material;
uniform vec3 u_CameraViewPos;

void main(){
	vec3 I = normalize(v_FragPos - u_CameraViewPos);
    vec3 R = reflect(I, normalize(v_Normal));
    FragColor = vec4(texture(u_Material.cube, R).rgb, 1.0);
}
