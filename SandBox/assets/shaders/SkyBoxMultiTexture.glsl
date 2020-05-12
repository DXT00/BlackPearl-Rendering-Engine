#type vertex
#version 450 core
layout(location = 0) in vec3 aPos;
out vec3 TexCoords;
		
uniform mat4 u_Model;
//uniform mat4 u_ProjectionView;
uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	TexCoords = aPos;
	vec4 pos = u_Projection*mat4(mat3(u_View))*vec4(aPos,1.0);


	gl_Position = pos.xyww;
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
#version 450 core
out vec4 FragColor;
in vec3 TexCoords;


uniform samplerCube u_Skybox0;
uniform samplerCube u_Skybox1;
uniform samplerCube u_Skybox2;

uniform float u_Factor0;
uniform float u_Factor1;
uniform float u_Factor2;

float near = 0.1; 
float far  = 100.0; 
float LinearizeDepth(float depth){
	float z = depth*2.0-1.0;//Back to NDC coordinate
	return 2.0*near*far /(far+near - z*(far - near));

}
void main(){

	FragColor =u_Factor0*texture(u_Skybox0,TexCoords)+u_Factor1*texture(u_Skybox1,TexCoords)+u_Factor2*texture(u_Skybox2,TexCoords);//*vec4(u_Material.diffuseColor,1.0);

}
