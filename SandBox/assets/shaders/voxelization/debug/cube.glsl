#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
out vec2 TexCoords;
		
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
out vec3 fragPos;
void main()
{
	TexCoords = aTexCoords;
	fragPos = vec3(u_Model * vec4(aPos,1.0));
	gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);
}



#type fragment
#version 430 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 fragPos;
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

}u_Material;
float near = 0.1; 
float far  = 100.0; 
float LinearizeDepth(float depth){
	float z = depth*2.0-1.0;//Back to NDC coordinate
	return 2.0*near*far /(far+near - z*(far - near));

}
void main(){
	//if(fragPos.x<=0&&fragPos.x>=-1&&fragPos.y<=0&&fragPos.y>=-1&&fragPos.z<=0&&fragPos.z>=-1)
		FragColor = vec4(u_Material.diffuseColor,0.2);
	//else
			//FragColor = vec4(1.0,0.0,0.0,1.0);

	//FragColor = vec4(u_Material.diffuseColor,1.0);
//	FragColor =  vec4(1.0,0.0,0.0,1.0);//texture(u_Material.diffuse,TexCoords);//*vec4(u_Material.diffuseColor,1.0);
//	float depth = LinearizeDepth(gl_FragCoord.z)/far;
//	FragColor=vec4(vec3(depth),1.0);
}
