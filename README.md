###  Voxel Cone Tracing 改进--在相机周围（立方体）体素化

#### 1.根据相机的 CubeSize 规定体素化范围，只有在立方体内的才体素化

#### 2.由于不再是对单位立方体voxelization了，因此需要坐标变化，把

CubeSize x CubeSize x CubeSize的场景，体素化到 1x1x1的voxel内，

体素追踪时，再从 1x1x1 voxel放大还原为原场景。


voxelization.glsl

```
void main(){
	vec3 viewDir = normalize(u_CameraViewPos-worldPositionFrag);

	vec3 color = vec3(0.0);
	if(!isInsideCube(worldPositionFrag, 0)) return;
	vec3 normalWorldPositionFrag =worldPositionFrag-u_CameraViewPos;

 	normalWorldPositionFrag = normalWorldPositionFrag/u_CubeSize;
	for(int i = 0; i < u_PointLightNums; ++i) 
	{
	color += CalcPointLight(u_PointLights[i], normalFrag,viewDir);

	}

	// Output lighting to 3D texture.
	vec3 voxel = scaleAndBias(normalWorldPositionFrag);
	ivec3 dim = imageSize(texture3D);// retrieve the dimensions of an image
	float alpha = pow(1 - 0, 4); // For soft shadows to work better with transparent materials.
	vec4 res = alpha * vec4(vec3(color), 1);
    imageStore(texture3D, ivec3(dim * voxel), res);//write a single texel into an image;
}

```
#### 3.修复的bug --> 1）判断是否在cube内出错了

#### 4.修复 visualVoxelization.glsl

不需要使用frameBuffer获取cube背面坐标了-->直接用 camera的 up,front,right计算即可

```
#type fragment
#version 430 core
// A simple fragment shader path tracer used to visualize 3D textures.
// Author:	Fredrik Pr鋘tare <prantare@gmail.com>
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
		color += (1.0f - color.a) * currentSample;
	} 
	color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
}

```

#### 4.修复 voxelization.glsl 的几何着色器 -->几何着色器的输出坐标必须在-1到1之间，再进行光栅化阶段

```

#type geometry
#version 430 core

layout(triangles) in;
layout(triangle_strip,max_vertices = 3) out;

in vec3 worldPositionGeom[];
in vec3 normalGeom[];

out vec3 worldPositionFrag;
out vec3 normalFrag;
uniform vec3 u_CameraViewPos;
uniform vec3 u_CubeSize;
out vec3 normal_worldPositionFrag;

/*几何着色器后会进入光栅化阶段，因此需要把坐标转换的 [-1,1]的裁剪平面！！*/

void main(){
	vec3 p1 = worldPositionGeom[1] - worldPositionGeom[0];
	vec3 p2 = worldPositionGeom[2] - worldPositionGeom[0];
	vec3 p = abs(cross(p1, p2)); 
	for(uint i = 0; i < 3; ++i){
		worldPositionFrag =worldPositionGeom[i];//
		normal_worldPositionFrag=(worldPositionGeom[i]-u_CameraViewPos)/u_CubeSize;
		normalFrag = normalGeom[i];
		if(p.z > p.x && p.z > p.y){
			gl_Position = vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.y, 0, 1);
		} else if (p.x > p.y && p.x > p.z){
			gl_Position = vec4(normal_worldPositionFrag.y, normal_worldPositionFrag.z, 0, 1);
		} else {
			gl_Position = vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.z, 0, 1);
		}
		EmitVertex();
	}
    EndPrimitive();
}

```
voxelization when CubeSize=5

![voxelization-CubeSize=5_3](/results/voxelization-CubeSize=5_3.png)
![voxelization-CubeSize=5_tracing3](/results/voxelization-CubeSize=5_tracing3.png)


