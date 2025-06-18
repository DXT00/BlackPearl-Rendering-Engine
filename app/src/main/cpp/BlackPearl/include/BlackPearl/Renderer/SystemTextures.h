#pragma once
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Renderer/GbufferInfo.h"
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
       
       //Shadow textures
       TextureHandle ShadowCubeMap;

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

    };
}