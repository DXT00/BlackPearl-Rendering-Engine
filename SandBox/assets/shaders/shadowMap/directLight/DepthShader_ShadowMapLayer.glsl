#type vertex
#version 330 core
layout (location =0) in vec3 aPos;

uniform mat4 u_LightProjectionViewMatrix;
uniform mat4 u_Model;

void main(){
	//将模型中的顶点变换到光空间
	gl_Position = u_LightProjectionViewMatrix*u_Model*vec4(aPos,1.0);
}
#type fragment
#version 330 core
void main(){
	//这个空像素着色器什么也不干，运行完后，深度缓冲会被更新
//	gl_FragDepth = gl_FragCoord.z;
}