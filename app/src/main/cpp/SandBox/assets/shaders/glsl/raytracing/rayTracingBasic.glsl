#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}


#type fragment
#version 430 core

layout(location = 0) out vec4 out_origin_curRayNum;
layout(location = 1) out vec4 out_dir_tMax;
layout(location = 2) out vec4 out_color_time;
layout(location = 3) out vec3 out_rayTracingRst;

uniform sampler2D origin_curRayNum;
uniform sampler2D dir_tMax;
uniform sampler2D color_time;
uniform sampler2D RTXRst;

uniform float u_rayNumMax;

in vec2 TexCoords;

void main(){
	float curRayNum = texture(origin_curRayNum,TexCoords).w;
	out_origin_curRayNum = vec4(vec3(0),min(curRayNum+1,u_rayNumMax));
	out_rayTracingRst =(curRayNum/u_rayNumMax)*vec3(TexCoords,1.0);


}