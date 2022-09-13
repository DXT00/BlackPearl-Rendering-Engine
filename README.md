## BlackPearl  Engine
BlackPearl  Engine is a dynamic GI rendering engine to simulate different dynamic GI algorithm,such as light probe based GI,voxel cone tracing GI,image based lighting, sparse voxel octree cone tracing.You can select the algorithm you want to simulate in SandBox.

## Get start

run GenerateProject.bat to set up visual studio project,it is only for Windows now.

## Documentation
Dynamic Global illumination PPT-DXT00 Master thesis


## Voxel Cone Tracing - Sparse voxel octree indirect light tracing

paper reference:
https://developer.download.nvidia.com/GTC/PDF/GTC2012/PresentationPDF/SB134-Voxel-Cone-Tracing-Octree-Real-Time-Illumination.pdf

"Efficient Sparse Voxel Octrees"
Samuli Laine Tero Karras
NVIDIA Research
https://research.nvidia.com/sites/default/files/pubs/2010-02_Efficient-Sparse-Voxel/laine2010i3d_paper.pdf


SVO path tracing - only one bounce cubeSize = 40
![SVO path tracing](/results/svo_pathTracing2.png)
![SVO path tracing](/results/svo_pathTracing3.png)
![SVO path tracing](/results/svo_pathTracing4.png)

## Light probe GI --- separate diffuse probe and reflection probe
![lightprobe_diffuse+specular](/results/lightprobe_diffuse+specular.png)

### -- sort diffuse probe based on Areas
![lightprobe_Map](/results/lightprobe_Map.png)

### -- cache object's nearyby diffuse probes, FPS:20frame/s
![lightprobe_diffuse+specular](/results/lightprobe_diffuse+specular.png)


### -- set reflection probe for each specular object,update specular probe every frame+ update a diffuse probe per frame,probe grid 4x2x4 
-- changing enviroment light ---> day and night shifting
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

### GPU Raytracing 01
![gpu_raytracing](/results/gpu_raytracing.png)

### GPU Raytracing 02 -- spheres
![gpu_raytracing_spheres](/results/gpu_raytracing_spheres.png)

### GPU Raytracing 03 -- material spheres
![gpu_raytracing_material_spheres](/results/gpu_raytracing_material_spheres.png)

### GPU Raytracing 04 -- gendata
![gpu_raytracing_group_rendering](/results/gpu_raytracing_group_rendering.png)

### GPU Raytracing 05 -- bvh_node
![gpu_raytracing_bvh_node_rendering](/results/gpu_raytracing_bvh_node.png)

### GPU Raytracing 06 -- material -Dielectric bunny
![gpu_raytracing_bvh_node_MATERAIL1](/results/gpu_raytracing_bvh_node_MATERAIL1.png)

## DirectX Raytracing
![DirectX_Raytracing](/results/DirectX_Raytracing.png)

## RTX
### DXR pipline rendering
![DXR pipline rendering](/results/DXR.png)

