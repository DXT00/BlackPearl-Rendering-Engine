#type vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
out vec2 TexCoords;
		
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);
}



#type fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

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

	float shininess;

};
uniform Material u_Material;
float near = 0.1; 
float far  = 100.0; 
float LinearizeDepth(float depth){
	float z = depth*2.0-1.0;//Back to NDC coordinate
	return 2.0*near*far /(far+near - z*(far - near));

}
void main(){
	//FragColor = vec4(u_Material.diffuseColor,1.0);
	FragColor = texture(u_Material.diffuse,TexCoords);//*vec4(u_Material.diffuseColor,1.0);
//	float depth = LinearizeDepth(gl_FragCoord.z)/far;
//	FragColor=vec4(vec3(depth),1.0);
}
