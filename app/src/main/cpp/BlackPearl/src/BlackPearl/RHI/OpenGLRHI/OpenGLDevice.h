#pragma once
#include "../RHIDevice.h"
#include "../RefCountPtr.h"
#include "../RHIDevice.h"
#include "../RHIPipeline.h"
#include "../RHIFrameBuffer.h"
#include "../RHIDefinitions.h"
#include "../RHICommandList.h"
#include "../RHIQuery.h"
#include "../RHIDescriptorTable.h"

namespace BlackPearl {
	class Device :public RefCounter<IDevice>
	{
	public:
		Device(){}
		virtual ~Device() override {}
		virtual TextureHandle createTexture(TextureDesc& d) ;

		TextureHandle createHandleForNativeTexture(uint32_t objectType, RHIObject texture, const TextureDesc& desc) override;

		BufferHandle createBuffer(const BufferDesc& d) override;
		virtual FramebufferHandle createFramebuffer(const FramebufferDesc& desc);

		void* mapBuffer(IBuffer* b, CpuAccessMode mapFlags) override;
		void unmapBuffer(IBuffer* b) override;

		MemoryRequirements getBufferMemoryRequirements(IBuffer* buffer) override;
		GraphicsPipelineHandle createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb) override;
		ComputePipelineHandle createComputePipeline(const ComputePipelineDesc& desc) override;
		MeshletPipelineHandle createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb) override;
		RayTracingPipelineHandle createRayTracingPipeline(const RayTracingPipelineDesc& desc) override;


		BindingLayoutHandle createBindingLayout(const RHIBindingLayoutDesc& desc) override;
		BindingLayoutHandle createBindlessLayout(const RHIBindlessLayoutDesc& desc) override;
		BindingSetHandle createBindingSet(const BindingSetDesc& desc, IBindingLayout* layout) override;

		CommandListHandle createCommandList(const CommandListParameters& params = CommandListParameters()) override;
		uint64_t executeCommandLists(ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue = CommandQueue::Graphics) override;

		IMessageCallback* getMessageCallback() override;

		SamplerHandle createSampler(const SamplerDesc& d) override;
		ShaderHandle createShader(const ShaderDesc& d, const void* binary, size_t binarySize) override;
		virtual ShaderLibraryHandle createShaderLibrary(const void* binary, size_t binarySize) override;

		InputLayoutHandle createInputLayout(const VertexAttributeDesc* d, uint32_t attributeCount, IShader* vertexShader);
		bool queryFeatureSupport(Feature feature, void* pInfo = nullptr, size_t infoSize = 0) override;
		

		void resizeDescriptorTable(IDescriptorTable* descriptorTable, uint32_t newSize, bool keepContents = true) override;
		bool writeDescriptorTable(IDescriptorTable* descriptorTable, const BindingSetItem& item) override;

		FormatSupport queryFormatSupport(Format format);
		// event queries
		EventQueryHandle createEventQuery() override;
		void setEventQuery(IEventQuery* query, CommandQueue queue) override;
		bool pollEventQuery(IEventQuery* query) override;
		void waitEventQuery(IEventQuery* query) override;
		void resetEventQuery(IEventQuery* query) override;



		static DeviceHandle createDevice();
	public:

	private:
		/** Underlying platform-specific data */
	};

}

