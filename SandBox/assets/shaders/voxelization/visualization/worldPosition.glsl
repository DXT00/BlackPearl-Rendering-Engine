// Author:	Fredrik Präntare <prantare@gmail.com>
// Date:	11/26/2016
#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;


out vec3 worldPosition;

void main(){
	worldPosition = vec3(u_Model * vec4(aPos, 1));
	gl_Position = u_ProjectionView* vec4(worldPosition, 1);
}

#type fragment
#version 430 core

// Author:	Fredrik Präntare <prantare@gmail.com>
// Date:	11/26/2016


in vec3 worldPosition;

out vec4 color;

void main(){ 
	//color.rgb = worldPosition; 
	color = vec4(worldPosition,1.0);
}