#pragma once
#include "RHIDefinitions.h"
#include "RHIPipeline.h"
#include "RHITexture.h"
#include "RHIFrameBuffer.h"
#include "RHIShaderLibrary.h"
#include "RHICommandList.h"
#include "RHIQuery.h"
#include "RHIMessageCallback.h"
#include "RHIDescriptorTable.h"
#ifdef GE_API_VULKAN
#include <vulkan/vulkan.h>
#endif
namespace BlackPearl {

	class IDevice : public IResource
	{
	public:
		//virtual HeapHandle createHeap(const HeapDesc& d) = 0;

		virtual TextureHandle createTexture(const TextureDesc& d) = 0;
		virtual SamplerHandle createSampler(const SamplerDesc& d) = 0;

		//virtual MemoryRequirements getTextureMemoryRequirements(ITexture* texture) = 0;
		//virtual bool bindTextureMemory(ITexture* texture, IHeap* heap, uint64_t offset) = 0;

		virtual TextureHandle createHandleForNativeTexture(uint32_t objectType, RHIObject texture, const TextureDesc& desc) = 0;

		//virtual StagingTextureHandle createStagingTexture(const TextureDesc& d, CpuAccessMode cpuAccess) = 0;
		//virtual void* mapStagingTexture(IStagingTexture* tex, const TextureSlice& slice, CpuAccessMode cpuAccess, size_t* outRowPitch) = 0;
		//virtual void unmapStagingTexture(IStagingTexture* tex) = 0;

		virtual BufferHandle createBuffer(const BufferDesc& d) = 0;
		virtual void* mapBuffer(IBuffer* buffer, CpuAccessMode cpuAccess) = 0;
		virtual void unmapBuffer(IBuffer* buffer) = 0;
		//virtual MemoryRequirements getBufferMemoryRequirements(IBuffer* buffer) = 0;
		//virtual bool bindBufferMemory(IBuffer* buffer, IHeap* heap, uint64_t offset) = 0;

		//virtual BufferHandle createHandleForNativeBuffer(ObjectType objectType, Object buffer, const BufferDesc& desc) = 0;

		virtual ShaderHandle createShader(const ShaderDesc& d, const void* binary, size_t binarySize) = 0;
		//virtual ShaderHandle createShaderSpecialization(IShader* baseShader, const ShaderSpecialization* constants, uint32_t numConstants) = 0;
		virtual ShaderLibraryHandle createShaderLibrary(const void* binary, size_t binarySize) = 0;


		//// Note: vertexShader is only necessary on D3D11, otherwise it may be null
		virtual InputLayoutHandle createInputLayout(const VertexAttributeDesc* d, uint32_t attributeCount, IShader* vertexShader) = 0;

		virtual bool writeDescriptorTable(IDescriptorTable* descriptorTable, const BindingSetItem& item) = 0;

		// Event queries
		virtual EventQueryHandle createEventQuery() = 0;
		virtual void setEventQuery(IEventQuery* query, CommandQueue queue) = 0;
		virtual bool pollEventQuery(IEventQuery* query) = 0;
		virtual void waitEventQuery(IEventQuery* query) = 0;
		virtual void resetEventQuery(IEventQuery* query) = 0;

		// Timer queries - see also begin/endTimerQuery in ICommandList
	   // virtual TimerQueryHandle createTimerQuery() = 0;
	   // virtual bool pollTimerQuery(ITimerQuery* query) = 0;
		// returns time in seconds
		//virtual float getTimerQueryTime(ITimerQuery* query) = 0;
	   // virtual void resetTimerQuery(ITimerQuery* query) = 0;

		// Returns the API kind that the RHI backend is running on top of.
		//virtual GraphicsAPI getGraphicsAPI() = 0;

		virtual FramebufferHandle createFramebuffer(const FramebufferDesc& desc) = 0;

		virtual GraphicsPipelineHandle createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb) = 0;
		virtual ComputePipelineHandle createComputePipeline(const ComputePipelineDesc& desc) = 0;
		virtual MeshletPipelineHandle createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb) = 0;

		//virtual RayTracingPipelineHandle createRayTracingPipeline(const RayTracingPipelineDesc& desc) = 0;

		virtual BindingLayoutHandle createBindingLayout(const RHIBindingLayoutDesc& desc) = 0;
		virtual BindingLayoutHandle createBindlessLayout(const RHIBindlessLayoutDesc& desc) = 0;
		virtual BindingSetHandle createBindingSet(const BindingSetDesc& desc, IBindingLayout* layout) = 0;
		// virtual DescriptorTableHandle createDescriptorTable(IBindingLayout* layout) = 0;

		virtual void resizeDescriptorTable(IDescriptorTable* descriptorTable, uint32_t newSize, bool keepContents = true) = 0;
		// virtual bool writeDescriptorTable(IDescriptorTable* descriptorTable, const BindingSetItem& item) = 0;

		// //virtual rt::OpacityMicromapHandle createOpacityMicromap(const rt::OpacityMicromapDesc& desc) = 0;
		// //virtual rt::AccelStructHandle createAccelStruct(const rt::AccelStructDesc& desc) = 0;
		// virtual MemoryRequirements getAccelStructMemoryRequirements(rt::IAccelStruct* as) = 0;
		//// virtual bool bindAccelStructMemory(rt::IAccelStruct* as, IHeap* heap, uint64_t offset) = 0;

		virtual CommandListHandle createCommandList(const CommandListParameters& params = CommandListParameters()) = 0;
		virtual uint64_t executeCommandLists(ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue = CommandQueue::Graphics) = 0;
		// virtual void queueWaitForCommandList(CommandQueue waitQueue, CommandQueue executionQueue, uint64_t instance) = 0;
		// virtual void waitForIdle() = 0;

		// // Releases the resources that were referenced in the command lists that have finished executing.
		// // IMPORTANT: Call this method at least once per frame.
		// virtual void runGarbageCollection() = 0;

		virtual bool queryFeatureSupport(Feature feature, void* pInfo = nullptr, size_t infoSize = 0) = 0;

		virtual FormatSupport queryFormatSupport(Format format) = 0;

		 // Front-end for executeCommandLists(..., 1) for compatibility and convenience
		uint64_t executeCommandList(ICommandList* commandList, CommandQueue executionQueue = CommandQueue::Graphics)
		{
			return executeCommandLists(&commandList, 1, executionQueue);
		}
		virtual IMessageCallback* getMessageCallback() = 0;

#ifdef GE_API_VULKAN
		//vulkan
		virtual VkSemaphore getQueueSemaphore(CommandQueue queueID) = 0;
		virtual void queueWaitForSemaphore(CommandQueue waitQueue, VkSemaphore semaphore, uint64_t value) = 0;
		virtual void queueSignalSemaphore(CommandQueue executionQueue, VkSemaphore semaphore, uint64_t value) = 0;
		virtual uint64_t queueGetCompletedInstance(CommandQueue queue) = 0;
		virtual FramebufferHandle createHandleForNativeFramebuffer(VkRenderPass renderPass,
			VkFramebuffer framebuffer, const FramebufferDesc& desc, bool transferOwnership) = 0;
#endif
	};
	typedef RefCountPtr<IDevice> DeviceHandle;

}


