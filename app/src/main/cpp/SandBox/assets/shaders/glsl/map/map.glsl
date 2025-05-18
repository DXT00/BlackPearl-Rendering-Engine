#type vertex
#version 450 core 

layout (location = 0) in vec3 aPos;
uniform mat4 u_ProjectionView;
out vec3 v_FragPos;

out vec4 v_vertex;
void main(){
	gl_Position = u_ProjectionView*vec4(aPos,1);
	v_FragPos = aPos;
}


#type geometry
#version 450 core 

uniform float u_AreaSize;
layout (points) in;
in vec3 v_FragPos[];
uniform mat4 u_ProjectionView;

layout(line_strip,max_vertices = 6) out;

void main(){
	
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	vec3 up = v_FragPos[0]+vec3(0,u_AreaSize,0);
	gl_Position = u_ProjectionView*vec4(up,1);
	EmitVertex();
	EndPrimitive();

	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	vec3 front = v_FragPos[0]+vec3(0,0,u_AreaSize);
	gl_Position = u_ProjectionView*vec4(front,1);
	EmitVertex();
	EndPrimitive();

	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	vec3 right = v_FragPos[0]+vec3(u_AreaSize,0,0);
	gl_Position = u_ProjectionView*vec4(right,1);
	EmitVertex();
	EndPrimitive();
}
#type fragment
#version 450 core 
out vec4 FragColor;
void main(){
	
	FragColor = vec4(0,1,0,0.5);

}