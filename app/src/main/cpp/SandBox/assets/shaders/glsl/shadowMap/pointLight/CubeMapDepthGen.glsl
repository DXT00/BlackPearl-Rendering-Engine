#type vertex
#version 330 core
layout (location =0) in vec3 aPos;

uniform mat4 u_Model;
void main(){
	gl_Position = u_Model * vec4(aPos,1.0);
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
