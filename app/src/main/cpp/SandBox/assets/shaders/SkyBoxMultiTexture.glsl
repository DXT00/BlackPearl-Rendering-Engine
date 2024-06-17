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
	
	������ϵͳ�̳��У�����˵���ڶ�����ɫ�����к�ִ��͸�ӻ��֣�
	����gl_Position��xyz�����������w������
	���ǻ�����Ȳ��Խ̳��е�֪�����շָ��z���������Ǹ���������ֵ��
	���������Ϣ�����ǿ����������λ�õ�z������������w������
	�⽫����z�������ǵ���1.0����Ϊ��Ӧ��͸�ӳ���ʱ��
	����z����ת����w / w = 1.0:
	ʹ��sykbox��λ��һֱ�����ͷ
	
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
