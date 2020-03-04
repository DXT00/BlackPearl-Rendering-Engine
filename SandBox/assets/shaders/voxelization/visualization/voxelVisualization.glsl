
// Author:	Fredrik Präntare <prantare@gmail.com> 
// Date:	11/26/2016
#type vertex
#version 430 core

//uniform mat4 V;

layout(location = 0) in vec3 aPos;
out vec2 textureCoordinateFrag; 

// Scales and bias a given vector (i.e. from [-1, 1] to [0, 1]).
vec2 scaleAndBias(vec2 p) { return 0.5f * p + vec2(0.5f); }

void main(){
	textureCoordinateFrag = aPos.xy;//scaleAndBias(aPos.xy);
	gl_Position = vec4(aPos, 1);
}


#type fragment
#version 430 core
// A simple fragment shader path tracer used to visualize 3D textures.
// Author:	Fredrik Präntare <prantare@gmail.com>
// Date:	11/26/2016

#define INV_STEP_LENGTH (1.0f/STEP_LENGTH)
#define STEP_LENGTH 0.005f
uniform sampler2D textureBack; // Unit cube back FBO.


uniform sampler3D texture3D; // Texture in which voxelization is stored.
uniform vec3 u_CameraViewPos; // World camera position.

uniform vec3 u_CameraFront;
uniform vec3 u_CameraUp;
uniform vec3 u_CameraRight;


uniform int u_State; // Decides mipmap sample level.
uniform vec3 u_CubeSize;

in vec2 textureCoordinateFrag; 
out vec4 color;

// Scales and bias a given vector (i.e. from [-1, 1] to [0, 1]).
vec3 scaleAndBias(vec3 p) { return 0.5f * p + vec3(0.5f); }

// Returns true if p is inside the unity cube (+ e) centered on (0, 0, 0).
//bool isInsideCube(vec3 p, float e) { return abs(p.x) < u_CubeSize.x + e && abs(p.y) <u_CubeSize.y + e && abs(p.z) < u_CubeSize.z + e; }
//bool isInsideCube(const vec3 p, float e) 
//{
//	return	p.x < u_CameraViewPos.x + u_CubeSize.x && p.x > u_CameraViewPos.x - u_CubeSize.x &&
//			p.y < u_CameraViewPos.y + u_CubeSize.y && p.y > u_CameraViewPos.y - u_CubeSize.y &&
//			 p.z < u_CameraViewPos.z + u_CubeSize.z && p.z > u_CameraViewPos.z - u_CubeSize.z ;
//
//}

void main() {
	const float mipmapLevel = u_State;

	// Initialize ray.
	 vec3 origin = u_CameraViewPos;

	vec3 stop = normalize(u_CameraFront)*u_CubeSize +
				normalize(u_CameraRight)*textureCoordinateFrag.x*u_CubeSize +
				normalize(u_CameraUp)*textureCoordinateFrag.y*u_CubeSize;

		vec3 direction =stop- origin;


	direction = direction/u_CubeSize;
	origin =(origin-u_CameraViewPos)/u_CubeSize;

	uint numberOfSteps = uint(INV_STEP_LENGTH * length(direction));///u_CubeSize.x
	direction = normalize(direction);

	// Trace.
	color = vec4(0.0f);
	for(uint step_ = 0; step_ < numberOfSteps && color.a < 0.99f; ++step_) {

		vec3 currentPoint = origin + STEP_LENGTH * step_ * direction;
		vec4 currentSample = textureLod(texture3D, scaleAndBias(currentPoint), mipmapLevel);///u_CubeSize.x
		if(currentSample.a!=-1)//if voxel is not empty
			color += (1.0f - color.a) * currentSample;
	} 
	color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
}