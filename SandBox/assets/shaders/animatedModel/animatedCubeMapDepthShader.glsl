#type vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in ivec4 aJointIndices;/*与当前fragment相关的3个Joint  {1,2,4}*/
layout(location = 4) in ivec4 aJointIndices1;/*与当前fragment相关的3个Joint  {1,2,4}*/
layout(location = 5) in vec4 aWeights;/*每个Joint的权重*/
layout(location = 6) in vec4 aWeights1;/*每个Joint的权重*/


const int MAX_JOINTS = 70;//max joints allowed in a skeleton
const int MAX_WEIGHT = 8;//max number of joints that can affect a vertex


uniform mat4 u_Model;
uniform mat4 u_JointModel[MAX_JOINTS];
void main(){
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalLocalNormal = vec4(0.0);

	mat4 model = mat4(0.0);
	for(int i=0;i<MAX_WEIGHT/2;i++){
		
		model+=aWeights[i]*u_JointModel[aJointIndices[i]];

	}
		for(int i=0;i<MAX_WEIGHT/2;i++){
		
		model+=aWeights1[i]*u_JointModel[aJointIndices1[i]];

	}

	vec4 newPos = model*vec4(aPos,1.0);
	gl_Position = u_Model*vec4(newPos.xyz,1.0);
	



}

#type geometry
#version 430 core
layout (triangles) in;
layout (triangle_strip,max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

void main(){

	for(int face = 0;face<6;++face){
		gl_Layer = face;
		for(int i=0;i<3;++i){
			FragPos = gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face]*FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}

//计算每个Fragment到光源的深度
#type fragment
#version 330 core
in vec4 FragPos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;
out float FragColor;
void main(){
    // get distance between fragment and light source
	float lightDistance = length(FragPos.xyz-u_LightPos);
	
	//map to [0:1] range by dividing by farPlane;
	lightDistance = lightDistance/u_FarPlane;

	gl_FragDepth = lightDistance;
	//FragColor=lightDistance;
	
}
