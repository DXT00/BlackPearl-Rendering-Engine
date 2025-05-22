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
uniform vec3 u_CameraViewPos;
uniform vec3 u_CubeSize;
layout(rgba32f, binding=1) uniform image2D u_Image;

out vec4 color;

void main(){ 
//	

	vec3 normalWorldPosition = (worldPosition-u_CameraViewPos)/u_CubeSize.x; //[-1,1.0]

	normalWorldPosition = 0.5*normalWorldPosition+0.5; //[0,1]
	color = vec4(worldPosition,1.0);


	ivec2 dim = imageSize(u_Image);
	imageStore(u_Image, ivec2(dim * vec2(normalWorldPosition)), vec4(worldPosition,1.0));//write a single texel into an image;

//	if(normalWorldPosition.x<=1.0&&normalWorldPosition.x>=-1.0 && normalWorldPosition.y<=1.0&&normalWorldPosition.y>=-1.0 &&normalWorldPosition.z<=1.0&&normalWorldPosition.z>=-1.0 )
//		color = vec4(0.0,1.0,0.0,1.0);
//	else
//		color = vec4(1.0,0.0,0.0,1.0);

}