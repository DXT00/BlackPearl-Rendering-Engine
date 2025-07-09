#pragma once
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Renderer/GbufferInfo.h"
#include "Renderer/SDF/GlobalDistanceField.h"
#include "Renderer/Voxel/Voxel.h"

namespace BlackPearl {
    class SystemTexture {
 
    public:

       void Init(DeviceHandle device);
       static SystemTexture& Get();
       bool isValid();

       //Default textures
       TextureHandle blackTexture;
       TextureHandle grayTexture;
       TextureHandle whiteTexture;
       TextureHandle blackTexture2DArray;
       TextureHandle whiteTexture2DArray;
       TextureHandle blackCubeMapArray;
       TextureHandle blackCubeMap;

       //Default back buffer rendertarget
       TextureHandle DefaultRT;

       //Gbuffer textures
       TextureHandle SceneColor;
       TextureHandle SceneColorResolve; //for mass, todo

       TextureHandle SceneDepth;
       TextureHandle SceneDepthResolve;  //for mass, todo

       TextureHandle GBufferA;
       TextureHandle GBufferB;
       TextureHandle GBufferC;
       TextureHandle GBufferD;
       
       // Global SDF, initial in GlobalDFRenderer
       GlobalDistanceField SceneGlobalDF;

       // Scene Voxels, initial in VoxelConeTracingRenderer
       std::vector<Voxel> SceneVoxels;

       // DDGI output texture
       TextureHandle DDGIIndirectLight;
       TextureHandle DDGIReflection;

       //Shadow textures
       TextureHandle ShadowCubeMap;

       //Skybox textures
       TextureHandle SkyboxTexture0;
       TextureHandle SkyboxTexture1;
       TextureHandle SkyboxTexture2;

       SystemTexture();

       TextureHandle GetBackBuffer();
    private:
        PlatformGbufferConfig m_GbufferConfig;

        bool m_isValid;

        void InitDefaultRT(DeviceHandle device);
        void InitDefaultTextures(DeviceHandle device);
        void InitGbufferTextures(DeviceHandle device);
        void InitSceneTextures(DeviceHandle device);
        void InitShadowTextures(DeviceHandle device);
        void InitSkyboxTextures(DeviceHandle device);


    };
}