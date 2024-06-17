#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in uint drawid;
layout(location = 4) in mat4 aModel;

layout (location = 0) out vec2 uv;
layout (location = 1) flat out uint drawID;
uniform mat4 u_ProjectionView;
layout (location = 2) out vec4 v_fragPos;
void main(void)
{
  uv = aTexCoord;
  drawID = drawid;
  gl_Position = u_ProjectionView  * aModel* vec4(aPos,1.0);
  v_fragPos = aModel* vec4(aPos,1.0);

}

#type fragment
#version 430 core

layout (location = 0) in vec2 uv;
layout (location = 1) flat in uint drawID;

layout (location = 2)  in vec4 v_fragPos;

out vec4 color;

void main(void)
{
	vec3 tmp =  vec3(v_fragPos.x,v_fragPos.y,v_fragPos.z)* float(drawID)/10000.0;

	if( drawID >= 10000/2){
	    color = vec4(1.0,1.0,0.0,1.0);
	}
	else{
	    color = vec4(normalize(tmp),1.0);
	}
}
