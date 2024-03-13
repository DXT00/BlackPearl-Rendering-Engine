## BlackPearl  Engine
BlackPearl  Engine is a dynamic GI rendering engine to simulate different dynamic GI algorithm,such as light probe based GI,voxel cone tracing GI,image based lighting, sparse voxel octree cone tracing.You can select the algorithm you want to simulate in SandBox.

## Get start

1) run GenerateProject.bat to set up visual studio project,it is only for Windows now.
2) select the GI algorithom in SandBox.cpp(openGL supported algorithm) or SandBoxDX.cpp (directX supported GI algorithm.such as DXR)


SandBox.cpp
```
class SandBox :public BlackPearl::Application {
public:

	SandBox(HINSTANCE hInstance,int nShowCmd, BlackPearl::DynamicRHI::Type rhiType, const std::string &renderer)
	:Application(hInstance, nShowCmd, rhiType, renderer){
		
		BlackPearl::Layer* layer = NULL;
		const std::string layer_name = renderer+"Layer";
		if (renderer == "ShadowMapPointLight") {
			layer = DBG_NEW ShadowMapPointLightLayer(layer_name);
		}
		else if(renderer == "VoxelConeTracing"){
			layer = DBG_NEW VoxelConeTracingLayer(layer_name);
		}
		else if (renderer == "PbrRendering") {
			layer = DBG_NEW PbrRenderingLayer(layer_name);
		}
		else if (renderer == "IBLRendering") {
			layer = DBG_NEW IBLRenderingLayer(layer_name);
		}
		else if (renderer == "IBLProbesRendering") {
			layer = DBG_NEW IBLProbesRenderingLayer(layer_name);
		}
		else if (renderer == "VoxelConeTracingDeferred") {
			layer = DBG_NEW VoxelConeTracingDeferredLayer(layer_name);
		}
		else if (renderer == "VoxelConeTracingSVO") {
			layer = DBG_NEW VoxelConeTracingSVOLayer(layer_name);
		}
		else if (renderer == "RayTracing") {
			layer = DBG_NEW RayTracingLayer(layer_name);
		}

		GetLayerManager()->PushLayer(layer);
	}
	virtual ~SandBox() = default;

};

BlackPearl::Application* BlackPearl::CreateApplication(HINSTANCE hInstance, int nShowCmd) {

	return DBG_NEW SandBox(hInstance, nShowCmd, BlackPearl::DynamicRHI::Type::OpenGL, "IBLProbesRendering");

}
```

SandBoxDX.cpp

```
class SandBoxDX :public BlackPearl::Application {

public:

	SandBoxDX(HINSTANCE hInstance,int nShowCmd, BlackPearl::DynamicRHI::Type rhiType, const std::string &renderer)
	:Application(hInstance, nShowCmd, rhiType, renderer){
		
		BlackPearl::Layer* layer = NULL;
		const std::string layer_name = renderer+"Layer";

		if (renderer == "D3D12RayTracing") {
			layer = DBG_NEW D3D12RayTracingLayer(layer_name);
		}
		else if (renderer == "D3D12RayTracingCube") {
			layer = DBG_NEW D3D12RayTracingCubeLayer(layer_name);
		}
		else if (renderer == "D3D12RayTracingModelLayer") {
			layer = DBG_NEW D3D12RayTracingModelLayer(layer_name);
		}
		else if (renderer == "D3D12BasicRenderLayer") {
			layer = DBG_NEW D3D12BasicRenderLayer(layer_name);
		}

		GetLayerManager()->PushLayer(layer);
	}
	virtual ~SandBoxDX() = default;

};

BlackPearl::Application* BlackPearl::CreateApplication(HINSTANCE hInstance, int nShowCmd) {
	return DBG_NEW SandBoxDX(hInstance, nShowCmd, BlackPearl::DynamicRHI::Type::D3D12, "D3D12BasicRenderLayer");

}

```
3) set the SandBox or SandboxDX as the start porject in visual studio

4) play your games! ^-^


## Environment  
1) NVIDIA GeForce RTX 2060/PCIe/SSE2  
2) Windows 10  
3) OpenGL Version :4.5.0  

## Featrues
1) IBL
2) voxel cone tracing
3) light probe GI
4) DXR 
5) Mesh shader
6) Batch rendering
7) Indirect rendering
8) Deffered rendering
9) PBR material
10) Simple Shadowmap
11) Terrain
12) ECS

## Documentation
Dynamic Global illumination PPT-DXT00 Master thesis

https://github.com/DXT00/BlackPearl-Rendering-Engine/blob/master/Dynamic%20Global%20illumination%20PPT-DXT00%20Master%20thesis.pdf

## Light probe GI 
### separate diffuse probe and reflection probe
![lightprobe_diffuse+specular](/results/lightprobe_diffuse+specular.png)

### sort diffuse probe based on Areas
![lightprobe_Map](/results/lightprobe_Map.png)

### cache object's nearyby diffuse probes
![lightprobe_diffuse+specular](/results/lightprobe_diffuse+specular.png)


### set reflection probe for each specular object,update specular probe every frame+ update a diffuse probe per frame,probe grid 4x2x4 

dynamic enviroment light ---> day and night shifting
![lightprobe_update_one_diffuse_probe_per_frame](/results/lightprobe_update_one_diffuse_probe_per_frame.png)

##  Voxel Cone Tracing - 3D texture cone tracing
![3Dtexture_coneTracing](/results/3Dtexture_coneTracing1.png)

### indirect specular light
![3Dtexture_coneTracing](/results/3Dtexture_coneTracing2.png)

### indirect diffuse light
![3Dtexture_coneTracing](/results/3Dtexture_coneTracing3.png)

## GPU Raytracing

reference: ToyRTX
https://github.com/Ubpa/ToyRTX


Dielectric bunny
![gpu_raytracing_bvh_node_MATERAIL1](/results/gpu_raytracing_bvh_node_MATERAIL1.png)

## DirectX Raytracing - RTX
### DXR pipline rendering
![DXR pipline rendering](/results/DXR.png)

## Mesh Shader
### Support meshlet rendering
![meshlet rendering](/results/meshShader.png)

## batch rendering
### per object rendering:
![per object rendering](/results/non-batchRendering.png)
### batch rendering:
batch rendering only need 9 drawcalls,and maintain 60 fps. improve (objs num/batch num) performance
![per object rendering](/results/batchRendering.png)
## Terrain rendering
![Terrain rendering](/results/TerrianTess.png)

## MultiIndirect DrawCall rendering
### rendering dynamic objs:  
since the transform matrix needs to be updated every frame ,can only reach 20 fps
![Terrain rendering](/results/muti_indirect_drawcall.png)

### rendering static objs:  
upload all data to gpu once, can reach 60 fps
![Terrain rendering](/results/muti_indirect_drawcall_without_matrixUpdate.png)

## GPU Driven hiz culling
![hiz culling](/results/hiz-frustum-culling.png)

## PCSS
![pcss](/results/PCSS.png)

## Cloud rendering
![cloud](/results/Cloud.png)

## SSR
![cloud](/results/SSR.png)