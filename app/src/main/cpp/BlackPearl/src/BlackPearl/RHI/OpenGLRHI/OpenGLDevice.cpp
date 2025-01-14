#include "pch.h"
#include "OpenGLDevice.h"
#include "OpenGLTexture.h"
#include "OpenGLCubeMapTexture.h"
#include "OpenGLImageTexture2D.h"
#include "OpenGLFrameBuffer.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
namespace BlackPearl 
{
	TextureHandle Device::createTexture(TextureDesc& d)
	{
		Texture* texture = nullptr;
		if (d.type == TextureType::CubeMap) {
			texture = DBG_NEW CubeMapTexture(d);
		}
		else if(d.type == TextureType::Image2DMap){
			
			texture = DBG_NEW ImageTexture2D(d, d.data);

		}
		else {
			texture = DBG_NEW Texture(d);
		}

		GE_ASSERT(texture, "texture is nullptr");

		texture->Init(d, d.data);

		return TextureHandle::Create(texture);
	}

	TextureHandle Device::createHandleForNativeTexture(uint32_t objectType, RHIObject texture, const TextureDesc& desc)
	{
		return TextureHandle();
	}


	BufferHandle Device::createBuffer(const BufferDesc& d)
	{
		return BufferHandle();
	}

	FramebufferHandle Device::createFramebuffer(const FramebufferDesc& desc)
	{
		return FramebufferHandle();
	}

	void* Device::mapBuffer(IBuffer* b, CpuAccessMode mapFlags)
	{
		return nullptr;
	}

	void Device::unmapBuffer(IBuffer* b)
	{
	}

	MemoryRequirements Device::getBufferMemoryRequirements(IBuffer* buffer)
	{
		return MemoryRequirements();
	}

	GraphicsPipelineHandle Device::createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb)
	{
		return GraphicsPipelineHandle();
	}

	ComputePipelineHandle Device::createComputePipeline(const ComputePipelineDesc& desc)
	{
		return ComputePipelineHandle();
	}

	MeshletPipelineHandle Device::createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb)
	{
		return MeshletPipelineHandle();
	}

	RayTracingPipelineHandle Device::createRayTracingPipeline(const RayTracingPipelineDesc& desc)
	{
		return RayTracingPipelineHandle();
	}

	BindingLayoutHandle Device::createBindingLayout(const RHIBindingLayoutDesc& desc)
	{
		return BindingLayoutHandle();
	}

	BindingLayoutHandle Device::createBindlessLayout(const RHIBindlessLayoutDesc& desc)
	{
		return BindingLayoutHandle();
	}

	BindingSetHandle Device::createBindingSet(const BindingSetDesc& desc, IBindingLayout* layout)
	{
		return BindingSetHandle();
	}

	CommandListHandle Device::createCommandList(const CommandListParameters& params)
	{
		return CommandListHandle();
	}

	uint64_t Device::executeCommandLists(ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue)
	{
		return 0;
	}

	IMessageCallback* Device::getMessageCallback()
	{
		return nullptr;
	}

	SamplerHandle Device::createSampler(const SamplerDesc& d)
	{
		return SamplerHandle();
	}

	ShaderHandle Device::createShader(const ShaderDesc& d, const void* binary, size_t binarySize)
	{
		return ShaderHandle();
	}

	ShaderLibraryHandle Device::createShaderLibrary(const void* binary, size_t binarySize)
	{
		return ShaderLibraryHandle();
	}

	InputLayoutHandle Device::createInputLayout(const VertexAttributeDesc* d, uint32_t attributeCount, IShader* vertexShader)
	{
		return InputLayoutHandle();
	}

	bool Device::queryFeatureSupport(Feature feature, void* pInfo, size_t infoSize)
	{
		return false;
	}

	void Device::resizeDescriptorTable(IDescriptorTable* descriptorTable, uint32_t newSize, bool keepContents)
	{
	}

	bool Device::writeDescriptorTable(IDescriptorTable* descriptorTable, const BindingSetItem& item)
	{
		return false;
	}

	FormatSupport Device::queryFormatSupport(Format format)
	{
		return FormatSupport();
	}

	EventQueryHandle Device::createEventQuery()
	{
		return EventQueryHandle();
	}

	void Device::setEventQuery(IEventQuery* query, CommandQueue queue)
	{
	}

	bool Device::pollEventQuery(IEventQuery* query)
	{
		return false;
	}

	void Device::waitEventQuery(IEventQuery* query)
	{
	}

	void Device::resetEventQuery(IEventQuery* query)
	{
	}


	DeviceHandle Device::createDevice()
	{
		Device* device = new Device();

		return DeviceHandle(device);
	}
}