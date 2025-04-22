#include "pch.h"
#include "Renderer/Material/MaterialManager.h"

namespace BlackPearl {
	MaterialManager::MaterialManager()
	{
	}
	MaterialManager::~MaterialManager()
	{
	}
    void MaterialManager::RegisterDeviceManager(DeviceManager* deviceManager)
    {
        m_Device = deviceManager->GetDevice();
    }
	void MaterialManager::Init()
	{
		_CreateSystemTextures(m_Device);
	}

	void MaterialManager::_CreateSystemTextures(DeviceHandle device)
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
        systemTextures.blackTexture = device->createTexture(textureDesc);

        textureDesc.debugName = "GrayTexture";
        systemTextures.grayTexture = device->createTexture(textureDesc);

        textureDesc.debugName = "WhiteTexture";
        systemTextures.whiteTexture = device->createTexture(textureDesc);

        textureDesc.dimension = TextureDimension::TextureCubeArray;
        textureDesc.debugName = "BlackCubeMapArray";
        textureDesc.arraySize = 6;
        systemTextures.blackCubeMapArray = device->createTexture(textureDesc);

        textureDesc.dimension =  TextureDimension::Texture2DArray;
        textureDesc.debugName = "BlackTexture2DArray";
        textureDesc.arraySize = 6;
        systemTextures.blackTexture2DArray = device->createTexture(textureDesc);
        textureDesc.debugName = "WhiteTexture2DArray";

        systemTextures.whiteTexture2DArray = device->createTexture(textureDesc);

        // Write the textures using a temporary CL

        CommandListHandle commandList = device->createCommandList();
        commandList->open();

        commandList->beginTrackingTextureState(systemTextures.blackTexture, AllSubresources, ResourceStates::Common);
        commandList->beginTrackingTextureState(systemTextures.grayTexture, AllSubresources, ResourceStates::Common);
        commandList->beginTrackingTextureState(systemTextures.whiteTexture, AllSubresources, ResourceStates::Common);
        commandList->beginTrackingTextureState(systemTextures.blackCubeMapArray, AllSubresources, ResourceStates::Common);
        commandList->beginTrackingTextureState(systemTextures.blackTexture2DArray, AllSubresources, ResourceStates::Common);
        commandList->beginTrackingTextureState(systemTextures.whiteTexture2DArray, AllSubresources, ResourceStates::Common);

        commandList->writeTexture(systemTextures.blackTexture, 0, 0, &blackImage, 0);
        commandList->writeTexture(systemTextures.grayTexture, 0, 0, &grayImage, 0);
        commandList->writeTexture(systemTextures.whiteTexture, 0, 0, &whiteImage, 0);

        for (uint32_t arraySlice = 0; arraySlice < 6; arraySlice += 1)
        {
            commandList->writeTexture(systemTextures.blackTexture2DArray, arraySlice, 0, &blackImage, 0);
            commandList->writeTexture(systemTextures.whiteTexture2DArray, arraySlice, 0, &whiteImage, 0);
            commandList->writeTexture(systemTextures.blackCubeMapArray, arraySlice, 0, &blackImage, 0);
        }

        commandList->setPermanentTextureState(systemTextures.blackTexture, ResourceStates::ShaderResource);
        commandList->setPermanentTextureState(systemTextures.grayTexture, ResourceStates::ShaderResource);
        commandList->setPermanentTextureState(systemTextures.whiteTexture, ResourceStates::ShaderResource);
        commandList->setPermanentTextureState(systemTextures.blackCubeMapArray, ResourceStates::ShaderResource);
        commandList->setPermanentTextureState(systemTextures.blackTexture2DArray, ResourceStates::ShaderResource);
        commandList->setPermanentTextureState(systemTextures.whiteTexture2DArray, ResourceStates::ShaderResource);
        commandList->commitBarriers();

        commandList->close();
        device->executeCommandList(commandList);

	}
}
