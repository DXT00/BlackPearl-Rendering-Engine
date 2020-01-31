#type vertex
#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;
		
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	TexCoords = aPos;
	gl_Position = u_ProjectionView*u_Model*vec4(aPos,1.0);

	//gl_Position = pos.xyww;
	/*
	
	在坐标系统教程中，我们说过在顶点着色器运行后执行透视划分，
	即用gl_Position的xyz坐标除以它的w分量。
	我们还从深度测试教程中得知，最终分割的z分量等于那个顶点的深度值。
	利用这个信息，我们可以设置输出位置的z分量等于它的w分量，
	这将导致z分量总是等于1.0，因为当应用透视除法时，
	它的z分量转换成w / w = 1.0:
	使得sykbox的位置一直在最后头
	
	*/
}



#type fragment
#version 330 core
out vec4 FragColor;
in vec3 TexCoords;

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
	samplerCube cube;
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
	//FragColor = vec4(1.0,1.0,0.1,1.0);//texture(u_Material.cube,TexCoords);//*vec4(u_Material.diffuseColor,1.0);
	//vec3 color =texture(u_Material.cube,TexCoords).rgb;
		FragColor =texture(u_Material.cube,TexCoords);//vec4(color,1.0);

//	float depth = LinearizeDepth(gl_FragCoord.z)/far;
//	FragColor=vec4(vec3(depth),1.0);
}
