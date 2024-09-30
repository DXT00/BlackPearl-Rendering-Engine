#pragma once
#if GE_API_VULKAN

#include <array>
#include "../RefCountPtr.h"
#include "../RHIDevice.h"
#include "../RHIPipeline.h"
#include "../RHIFrameBuffer.h"
#include "../RHIDefinitions.h"
#include "../RHICommandList.h"
#include "../RHIQuery.h"
#include "../RHIDescriptorTable.h"
#include "VkAllocator.h"

namespace BlackPearl {

	class Texture;
	class StagingTexture;
	class InputLayout;
	class Buffer;
	class Shader;
	class Sampler;
	class Framebuffer;
	class GraphicsPipeline;
	class ComputePipeline;
	class BindingSet;
	class EvenetQuery;
	class TimerQuery;
	class Marker;
	class Device;
	class Queue;
	//class VulkanAllocator;
	struct DeviceDesc
	{
		IMessageCallback* errorCB = nullptr;

		VkInstance instance;
		VkPhysicalDevice physicalDevice;
		VkDevice device;

		// any of the queues can be null if this context doesn't intend to use them
		VkQueue graphicsQueue;
		int graphicsQueueIndex = -1;
		VkQueue transferQueue;
		int transferQueueIndex = -1;
		VkQueue computeQueue;
		int computeQueueIndex = -1;

		VkAllocationCallbacks* allocationCallbacks = nullptr;

		const char** instanceExtensions = nullptr;
		size_t numInstanceExtensions = 0;

		const char** deviceExtensions = nullptr;
		size_t numDeviceExtensions = 0;

		uint32_t maxTimerQueries = 256;

		// Indicates if VkPhysicalDeviceVulkan12Features::bufferDeviceAddress was set to 'true' at device creation time
		bool bufferDeviceAddressSupported = false;
	};

	class Device : public RefCounter<IDevice>
	{
	public:
		// Internal backend methods

		Device(const DeviceDesc& desc);
		~Device() override;

		Queue* getQueue(CommandQueue queue) const;
		TextureHandle createTexture(TextureDesc& d) override;

		TextureHandle createHandleForNativeTexture(uint32_t objectType, RHIObject texture, const TextureDesc& desc) override;

		BufferHandle createBuffer(const BufferDesc& d) override;
		FramebufferHandle createFramebuffer(const FramebufferDesc& desc) override;

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

		IMessageCallback* getMessageCallback() override { return m_Context.messageCallback; }

		SamplerHandle createSampler(const SamplerDesc& d) override;
		ShaderHandle createShader(const ShaderDesc& d, const void* binary, size_t binarySize) override;
		virtual ShaderLibraryHandle createShaderLibrary(const void* binary, size_t binarySize) override;

		InputLayoutHandle createInputLayout(const VertexAttributeDesc* d, uint32_t attributeCount, IShader* vertexShader);
		// vulkan::IDevice implementation
		VkSemaphore getQueueSemaphore(CommandQueue queueID) override;
		void queueWaitForSemaphore(CommandQueue waitQueue, VkSemaphore semaphore, uint64_t value) override;
		void queueSignalSemaphore(CommandQueue executionQueue, VkSemaphore semaphore, uint64_t value) override;
		uint64_t queueGetCompletedInstance(CommandQueue queue) override;

		bool queryFeatureSupport(Feature feature, void* pInfo = nullptr, size_t infoSize = 0) override;


		FramebufferHandle createHandleForNativeFramebuffer(VkRenderPass renderPass, VkFramebuffer framebuffer,
			const FramebufferDesc& desc, bool transferOwnership) override;

		void resizeDescriptorTable(IDescriptorTable* descriptorTable, uint32_t newSize, bool keepContents = true) override;
		bool writeDescriptorTable(IDescriptorTable* descriptorTable, const BindingSetItem& item) override;

		FormatSupport queryFormatSupport(Format format);
		// event queries
		EventQueryHandle createEventQuery() override;
		void setEventQuery(IEventQuery* query, CommandQueue queue) override;
		bool pollEventQuery(IEventQuery* query) override;
		void waitEventQuery(IEventQuery* query) override;
		void resetEventQuery(IEventQuery* query) override;

		static DeviceHandle createDevice(const DeviceDesc& desc);
		VkDevice getDevice() { return m_Context.device; }
	private:
		VulkanContext m_Context;
		VulkanAllocator m_Allocator;

		VkQueryPool m_TimerQueryPool = nullptr;
		// utils::BitSetAllocator m_TimerQueryAllocator;

		//std::mutex m_Mutex;

		// array of submission queues
		std::array<std::unique_ptr<Queue>, uint32_t(CommandQueue::Count)> m_Queues;

		void* mapBuffer(IBuffer* b, CpuAccessMode flags, uint64_t offset, size_t size) const;
	};



}
#endif
