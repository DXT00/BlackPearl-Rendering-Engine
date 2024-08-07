#include "pch.h"
#include "CommonRenderPass.h"

namespace BlackPearl {



	CommonRenderPasses::CommonRenderPasses(IDevice* device, std::shared_ptr<ShaderFactory> shaderFactory)
	{
		m_Device = device;
		unsigned int blackImage = 0xff000000;
		unsigned int grayImage = 0xff808080;
		unsigned int whiteImage = 0xffffffff;

		TextureDesc textureDesc;
		textureDesc.format = Format::RGBA8_UNORM;
		textureDesc.width = 1;
		textureDesc.height = 1;
		textureDesc.mipLevels = 1;

		textureDesc.debugName = "BlackTexture";
		m_BlackTexture = m_Device->createTexture(textureDesc);

		textureDesc.debugName = "GrayTexture";
		m_GrayTexture = m_Device->createTexture(textureDesc);

		textureDesc.debugName = "WhiteTexture";
		m_WhiteTexture = m_Device->createTexture(textureDesc);

		textureDesc.dimension = TextureDimension::TextureCubeArray;
		textureDesc.debugName = "BlackCubeMapArray";
		textureDesc.arraySize = 6;
		m_BlackCubeMapArray = m_Device->createTexture(textureDesc);

		textureDesc.dimension = TextureDimension::Texture2DArray;
		textureDesc.debugName = "BlackTexture2DArray";
		textureDesc.arraySize = 6;
		m_BlackTexture2DArray = m_Device->createTexture(textureDesc);
		textureDesc.debugName = "WhiteTexture2DArray";
		m_WhiteTexture2DArray = m_Device->createTexture(textureDesc);


		// Write the textures using a temporary CL

		CommandListHandle commandList = m_Device->createCommandList();
		commandList->open();

		commandList->beginTrackingTextureState(m_BlackTexture, AllSubresources, ResourceStates::Common);
		commandList->beginTrackingTextureState(m_GrayTexture, AllSubresources, ResourceStates::Common);
		commandList->beginTrackingTextureState(m_WhiteTexture, AllSubresources, ResourceStates::Common);
		commandList->beginTrackingTextureState(m_BlackCubeMapArray, AllSubresources, ResourceStates::Common);
		commandList->beginTrackingTextureState(m_BlackTexture2DArray, AllSubresources, ResourceStates::Common);
		commandList->beginTrackingTextureState(m_WhiteTexture2DArray, AllSubresources, ResourceStates::Common);

		commandList->writeTexture(m_BlackTexture, 0, 0, &blackImage, 0);
		commandList->writeTexture(m_GrayTexture, 0, 0, &grayImage, 0);
		commandList->writeTexture(m_WhiteTexture, 0, 0, &whiteImage, 0);

		for (uint32_t arraySlice = 0; arraySlice < 6; arraySlice += 1)
		{
			commandList->writeTexture(m_BlackTexture2DArray, arraySlice, 0, &blackImage, 0);
			commandList->writeTexture(m_WhiteTexture2DArray, arraySlice, 0, &whiteImage, 0);
			commandList->writeTexture(m_BlackCubeMapArray, arraySlice, 0, &blackImage, 0);
		}

		commandList->setPermanentTextureState(m_BlackTexture, ResourceStates::ShaderResource);
		commandList->setPermanentTextureState(m_GrayTexture, ResourceStates::ShaderResource);
		commandList->setPermanentTextureState(m_WhiteTexture, ResourceStates::ShaderResource);
		commandList->setPermanentTextureState(m_BlackCubeMapArray, ResourceStates::ShaderResource);
		commandList->setPermanentTextureState(m_BlackTexture2DArray, ResourceStates::ShaderResource);
		commandList->setPermanentTextureState(m_WhiteTexture2DArray, ResourceStates::ShaderResource);
		commandList->commitBarriers();

		commandList->close();
		m_Device->executeCommandList(commandList);

	}

	void CommonRenderPasses::BlitTexture(ICommandList* commandList, const BlitParameters& params, BindingCache* bindingCache)
	{
	}

	void CommonRenderPasses::BlitTexture(ICommandList* commandList, IFramebuffer* targetFramebuffer, ITexture* sourceTexture, BindingCache* bindingCache)
	{
	}

}