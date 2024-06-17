#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
out vec2 TexCoords;
		
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);
}

#type fragment
#version 430 core
out vec4 FragColor;
in vec2 TexCoords;


uniform Material u_Material;

float near = 0.1; 
float far  = 100.0; 
float LinearizeDepth(float depth){
	float z = depth*2.0-1.0;//Back to NDC coordinate
	return 2.0*near*far /(far+near - z*(far - near));

}
void main(){
	FragColor = vec4(u_Material.diffuseColor,0.2);
//	FragColor =  vec4(1.0,0.0,0.0,1.0);//texture(u_Material.diffuse,TexCoords);//*vec4(u_Material.diffuseColor,1.0);
//	float depth = LinearizeDepth(gl_FragCoord.z)/far;
//	FragColor=vec4(vec3(depth),1.0);
}
