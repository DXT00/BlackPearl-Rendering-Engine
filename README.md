###  Voxel Cone Tracing - Sparse voxel octree indirect light tracing

paper reference:
https://developer.download.nvidia.com/GTC/PDF/GTC2012/PresentationPDF/SB134-Voxel-Cone-Tracing-Octree-Real-Time-Illumination.pdf

"Efficient Sparse Voxel Octrees"
Samuli Laine Tero Karras
NVIDIA Research
https://research.nvidia.com/sites/default/files/pubs/2010-02_Efficient-Sparse-Voxel/laine2010i3d_paper.pdf


SVO path tracing - only one bounce cubeSize = 40
![SVO path tracing](/results/svo_pathTracing2.png)
![SVO path tracing](/results/svo_pathTracing3.png)
![SVO path tracing](/results/svo_pathTracing5.png)
![SVO path tracing](/results/svo_pathTracing4.png)

### Light probe GI --- separate diffuse probe and reflection probe
![lightprobe_diffuse+specular](/results/lightprobe_diffuse+specular.png)

### -- sort diffuse probe based on Areas
![lightprobe_Map](/results/lightprobe_Map.png)

### -- cache object's nearyby diffuse probes, FPS:20frame/s
![lightprobe_Map_FPS](/results/lightprobe_Map_FPS.png)

### -- cache object's nearyby diffuse probes,update shadowMap only when pointlight pos changes,4x4x4 probe grid FPS:8frame/s
![lightprobe_Map_FPS2_4_4_4](/results/lightprobe_Map_FPS2_4_4_4.png)



###  Voxel Cone Tracing - 3D texture cone tracing

![3Dtexture_coneTracing](/results/3Dtexture_coneTracing3.png)
![3Dtexture_coneTracing](/results/3Dtexture_coneTracing4.png)
![3Dtexture_coneTracing](/results/3Dtexture_coneTracing5.png)


