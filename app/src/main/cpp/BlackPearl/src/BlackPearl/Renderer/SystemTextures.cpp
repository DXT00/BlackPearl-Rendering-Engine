#include "pch.h"
#include "Renderer/SystemTextures.h"
#include "Renderer/GbufferInfo.h"
#include "Application.h"
#include "Config.h"

namespace BlackPearl {
    SystemTexture& SystemTexture::Get() {
        static SystemTexture s_Inst;
        return s_Inst;
    }
    bool SystemTexture::isValid()
    {
        return m_isValid;
    }
    SystemTexture::SystemTexture()
        :m_isValid(false)
    {

    }

    void SystemTexture::InitDefaultTextures(DeviceHandle device)
    {
        unsigned int blackImage = 0xff000000;
        unsigned int grayImage = 0xff808080;
        unsigned int whiteImage = 0xffffffff;

        TextureDesc textureDesc;
        textureDesc.format = Format::RGBA8_UNORM;
        textureDesc.width = 1;
        textureDesc.height = 1;
        textureDesc.mipLevelsCnt = 1;

        textureDesc.debugName = "BlackTexture";
        blackTexture = device->createTexture(textureDesc);

        textureDesc.debugName = "GrayTexture";
        grayTexture = device->createTexture(textureDesc);

        textureDesc.debugName = "WhiteTexture";
        whiteTexture = device->createTexture(textureDesc);

        textureDesc.dimension = TextureDimension::TextureCube;
        textureDesc.debugName = "BlackCubeMap";
        blackCubeMap = device->createTexture(textureDesc);


        textureDesc.dimension = TextureDimension::TextureCubeArray;
        textureDesc.debugName = "BlackCubeMapArray";
        textureDesc.arraySize = 6;
        blackCubeMapArray = device->createTexture(textureDesc);





        textureDesc.dimension = TextureDimension::Texture2DArray;
        textureDesc.debugName = "BlackTexture2DArray";
        textureDesc.arraySize = 6;
        blackTexture2DArray = device->createTexture(textureDesc);
        textureDesc.debugName = "WhiteTexture2DArray";

        whiteTexture2DArray = device->createTexture(textureDesc);


        CommandListHandle commandList = device->createCommandList();
        commandList->open();

        commandList->beginTrackingTextureState(blackTexture, AllSubresources, ResourceStates::Common);
        commandList->beginTrackingTextureState(grayTexture, AllSubresources, ResourceStates::Common);
        commandList->beginTrackingTextureState(whiteTexture, AllSubresources, ResourceStates::Common);
        commandList->beginTrackingTextureState(blackCubeMapArray, AllSubresources, ResourceStates::Common);
        commandList->beginTrackingTextureState(blackTexture2DArray, AllSubresources, ResourceStates::Common);
        commandList->beginTrackingTextureState(whiteTexture2DArray, AllSubresources, ResourceStates::Common);

        commandList->writeTexture(blackTexture, 0, 0, &blackImage, 0);
        commandList->writeTexture(grayTexture, 0, 0, &grayImage, 0);
        commandList->writeTexture(whiteTexture, 0, 0, &whiteImage, 0);

        for (uint32_t arraySlice = 0; arraySlice < 6; arraySlice += 1)
        {
            commandList->writeTexture(blackTexture2DArray, arraySlice, 0, &blackImage, 0);
            commandList->writeTexture(whiteTexture2DArray, arraySlice, 0, &whiteImage, 0);
            commandList->writeTexture(blackCubeMapArray, arraySlice, 0, &blackImage, 0);
        }

        commandList->setPermanentTextureState(blackTexture, ResourceStates::ShaderResource);
        commandList->setPermanentTextureState(grayTexture, ResourceStates::ShaderResource);
        commandList->setPermanentTextureState(whiteTexture, ResourceStates::ShaderResource);
        commandList->setPermanentTextureState(blackCubeMapArray, ResourceStates::ShaderResource);
        commandList->setPermanentTextureState(blackTexture2DArray, ResourceStates::ShaderResource);
        commandList->setPermanentTextureState(whiteTexture2DArray, ResourceStates::ShaderResource);
        commandList->commitBarriers();

        commandList->close();
        device->executeCommandList(commandList);
    }

    void SystemTexture::InitGbufferTextures(DeviceHandle device)
    {
        GBufferA = blackTexture;
        GBufferB = blackTexture;
        GBufferC = blackTexture;
        GBufferD = blackTexture;



        GbufferInfo gBufferInfo = GetGbufferInfo(m_GbufferConfig);
        TextureDesc gBufferTextureDesc;
        gBufferTextureDesc.width = Application::Get().GetWindow().GetCurWindowSize().x;
        gBufferTextureDesc.height = Application::Get().GetWindow().GetCurWindowSize().y;
        gBufferTextureDesc.mipLevelsCnt = 1;

        for (size_t i = 0; i < gBufferInfo.numTargets; i++)
        {
            gBufferTextureDesc.debugName = gBufferInfo.targets[i].name;
            gBufferTextureDesc.format = gBufferInfo.targets[i].format;

            if (gBufferInfo.targets[i].name == "GBufferA") {

                GBufferA = device->createTexture(gBufferTextureDesc);
            }
            else if (gBufferInfo.targets[i].name == "GBufferB") {
                GBufferB = device->createTexture(gBufferTextureDesc);
            }
            else if (gBufferInfo.targets[i].name == "GBufferC") {

                GBufferC = device->createTexture(gBufferTextureDesc);

            }
            else if (gBufferInfo.targets[i].name == "GBufferD") {
                GBufferD = device->createTexture(gBufferTextureDesc);
            }
           
        }
        //TextureDesc depthTextureDesc;
        //depthTextureDesc.width = Application::Get().GetWindow().GetCurWindowSize().x;
        //depthTextureDesc.height = Application::Get().GetWindow().GetCurWindowSize().y;
        //depthTextureDesc.mipLevelsCnt = 1;

        //SceneDepth = device->createTexture(depthTextureDesc);
    }

    void SystemTexture::InitShadowTextures(DeviceHandle device)
    {
        ShadowCubeMap = blackCubeMap;
    }


    void SystemTexture::InitSceneTextures(DeviceHandle device) {

        TextureDesc colorDesc;
        colorDesc.width = Application::Get().GetWindow().GetCurWindowSize().x;
        colorDesc.height = Application::Get().GetWindow().GetCurWindowSize().y;
        colorDesc.mipLevelsCnt = 1;
        colorDesc.sampleCount = Configuration::MSAA_SAMPLES;
        if (Configuration::bDeferredShading) {
            GbufferInfo gBufferInfo = GetGbufferInfo(m_GbufferConfig);

            for (size_t i = 0; i < gBufferInfo.numTargets; i++) {
             
                colorDesc.debugName = gBufferInfo.targets[i].name;
                colorDesc.format = gBufferInfo.targets[i].format; //不需要alpha, R11G11B10_FLOAT, tonemapping后再写到alpha texture
                if (gBufferInfo.targets[i].name == "SceneColor") 
                    SceneColor = device->createTexture(colorDesc);
        
            }
        }
        else {
            colorDesc.debugName = "SceneColor";
            colorDesc.format = Format::R11G11B10_FLOAT; 
            SceneColor = device->createTexture(colorDesc);

        }
        if (colorDesc.sampleCount > 1) {
            colorDesc.sampleCount = 1;
            SceneColorResolve = device->createTexture(colorDesc);

        }


        TextureDesc depthDesc;
        depthDesc.width = Application::Get().GetWindow().GetCurWindowSize().x;
        depthDesc.height = Application::Get().GetWindow().GetCurWindowSize().y;
        depthDesc.mipLevelsCnt = 1;
        depthDesc.sampleCount = Configuration::MSAA_SAMPLES;
        depthDesc.format = Format::D32S8;
        SceneDepth = device->createTexture(depthDesc);

        if (depthDesc.sampleCount > 1) {
            depthDesc.sampleCount = 1;
            SceneDepthResolve = device->createTexture(depthDesc);

        }

    }
    void SystemTexture::Init(DeviceHandle device)
    {
        InitDefaultTextures(device);
        InitGbufferTextures(device);
        InitSceneTextures(device);
        InitShadowTextures(device);

        m_isValid = true;
       
    }



}
