## BlackPearl Engine
BlackPearl Engine is a dynamic GI rendering engine to simulate different dynamic GI algorithm,such as light probe based GI,voxel cone tracing GI, image based lighting, SDF DDGI.You can select the algorithm you want to simulate in SandBox.

## Supported Platforms
Supports Windows and Android platforms. Currently, the lighting system on the Android platform is still relatively simple, and GI is still under planning.




| Platform     | Graphics API   |
|--------------|--------------  |
| Windows      | OpenGL ,Vulkan |
| Android      |   GLES         |

## Environment 
Windows 
1) NVIDIA GeForce RTX 2060/PCIe/SSE2  
2) Windows 10  
3) OpenGL Version :4.5.0  

Android:
Test on two devices:
Android 10 Mali/ Android 12 Andreno

## Featrues
1) IBL
2) voxel cone tracing
3) light probe GI
4) SDF DDGI  
5) Gpu Driven
6) Batch rendering
7) Forward pipeline
8) Deffered pipeline
9) PBR material
10) Simple Shadowmap, PCSS
11) Terrain
12) SSR



## Documentation
Dynamic Global illumination PPT-DXT00 Master thesis

https://github.com/DXT00/BlackPearl-Rendering-Engine/blob/master/Dynamic%20Global%20illumination%20PPT-DXT00%20Master%20thesis.pdf

## CubeMap Based Light probe GI 

### sort diffuse probe based on Areas
![lightprobe_Map](/app/src/main/cpp/results/lightprobe_Map.png)

### cache object's nearyby diffuse probes
![lightprobe_diffuse+specular](/app/src/main/cpp/results/lightprobe_diffuse+specular.png)

### dynamic enviroment light (day and night shifting)
![lightprobe_update_one_diffuse_probe_per_frame](/app/src/main/cpp/results/lightprobe_update_one_diffuse_probe_per_frame.png)

##  Voxel Cone Tracing - 3D texture cone tracing
![3Dtexture_coneTracing](/app/src/main/cpp/results/3Dtexture_coneTracing1.png)

### indirect specular light
![3Dtexture_coneTracing](/app/src/main/cpp/results/3Dtexture_coneTracing2.png)

### indirect diffuse light
![3Dtexture_coneTracing](/app/src/main/cpp/results/3Dtexture_coneTracing3.png)

## SDF Based DDGI 




## DirectX Raytracing - RTX
### DXR pipline rendering
![DXR pipline rendering](/app/src/main/cpp/results/DXR.png)

## Mesh Shader
### Support meshlet rendering
![meshlet rendering](/app/src/main/cpp/results/meshShader.png)

## batch rendering
### per object rendering:
![per object rendering](/app/src/main/cpp/results/non-batchRendering.png)
### batch rendering:
batch rendering only need 9 drawcalls,and maintain 60 fps. improve (objs num/batch num) performance
![per object rendering](/app/src/main/cpp/results/batchRendering.png)
## Terrain rendering
![Terrain rendering](/app/src/main/cpp/results/TerrianTess.png)

## MultiIndirect DrawCall rendering
### rendering dynamic objs:  
since the transform matrix needs to be updated every frame ,can only reach 20 fps
![Terrain rendering](/app/src/main/cpp/results/muti_indirect_drawcall.png)

### rendering static objs:  
upload all data to gpu once, can reach 60 fps
![Terrain rendering](/app/src/main/cpp/results/muti_indirect_drawcall_without_matrixUpdate.png)

## GPU Driven hiz culling
![hiz culling](/app/src/main/cpp/results/hiz-frustum-culling.png)

## PCSS
![pcss](/app/src/main/cpp/results/PCSS.png)

## Cloud rendering
![cloud](/app/src/main/cpp/results/Cloud.png)

## SSR
![cloud](/app/src/main/cpp/results/SSR.png)

## Get start

Before starting, you need to download the third-party libraries and Assets.[ see BlackPearl vendor](#vendor-anchor)


### Windows
1) Open the directory: app/src/main/cpp
2) select render API, modify the RenderAPI Settings in GenerateProject.bat:
```
call vendor\bin\premake\premake5.exe vs2022 --RenderAPI=opengl
PAUSE
```
You can choose RenderAPI from the following three options:  
	   { "opengl",    "OpenGL" },  
	   { "vulkan",    "Vulkan (Windows only)" }  

3) run `GenerateProject.bat` to set up visual studio project
   
4)  set `Sandbox` or `SandboxVK` as the startup project.


### Android

1) compile `BlackPearl` and `SandBoxAndroid` using Clion.
   
| ANDROID_NDK     | ANDROID_PLATFORM  |
|--------------|--------------  |
| 25.1.8937393    | android-26 Or any of the above versions |

camke settings:
```
cmake -DCMAKE_BUILD_TYPE=Debug -G Ninja -DCMAKE_SYSTEM_NAME=Android -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_SYSTEM_VERSION=26 -DANDROID_PLATFORM=android-26 -DANDROID_ABI=arm64-v8a -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a -DANDROID_NDK=${Path-to-AndroidSDK}\ndk\25.1.8937393 -DCMAKE_ANDROID_NDK=${Path-to-AndroidSDK}\ndk\25.1.8937393 -DCMAKE_TOOLCHAIN_FILE=${Path-to-AndroidSDK}\ndk\25.1.8937393\build\cmake\android.toolchain.cmake -DCMAKE_CXX_FLAGS=-std=c++11 
```
2) open blackpearl app by Android Studio

3) run the app

<a id="vendor-anchor"></a>
## BlackPearl vendor


### vendor library:

Please download the vendor library at the following link:
url：https://pan.baidu.com/s/1JtBzMzp5xZo1HRNRfrIkHQ?pwd=1234
Extract code：1234

copy to app\src\main\cpp\BlackPearl\vendor


### assets library:
Please download the assets library at the following link:
https://pan.baidu.com/s/1qclUPOsQ73bas0TnDH_gNA?pwd=1234
Extract code：1234


copy to app\src\main\cpp\assets