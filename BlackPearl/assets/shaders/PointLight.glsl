#pragma once

#type vertex
#version 330 core
layout(location = 0) in vec3 aPos;

out vec2 TexCoord;
		
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
	gl_Position = u_ProjectionView * u_Model * vec4(aPos,1.0);
}



#type fragment
#version 330 core
out vec4 FragColor;

uniform vec3 u_LightColor;		

void main(){
	FragColor = vec4(u_LightColor,1.0);
}
