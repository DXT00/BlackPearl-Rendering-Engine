###  Voxel Cone Tracing 改进--在相机周围（立方体）体素化

#### 1.根据相机的 CubeSize 规定体素化范围，只有在立方体内的才体素化

#### 2.由于不再是对单位立方体voxelization了，因此需要坐标变化，把

CubeSize * CubeSize *CubeSize的场景，体素化到 1*1*1的voxel内，

体素追踪时，再从 1*1*1 voxel放大还原为原场景。


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
#### 3.存在的bug --> 1）随着 CubeSize增大体素化分辨率越低
2）tracing还有问题。后续解决。。



![voxelization](/results/voxelization2.png)

voxelization-CubeSize=1

![voxelization-CubeSize=1](/results/voxelization-CubeSize=1.png)
![voxelization-CubeSize=1_tracing](/results/voxelization-CubeSize=1_tracing.png)

voxelization-CubeSize=2
![voxelization-CubeSize=2](/results/voxelization-CubeSize=2.png)
![voxelization-CubeSize=2_tracing](/results/voxelization-CubeSize=2_tracing.png)

voxelization-CubeSize=3
![voxelization-CubeSize=3](/results/voxelization-CubeSize=3.png)

