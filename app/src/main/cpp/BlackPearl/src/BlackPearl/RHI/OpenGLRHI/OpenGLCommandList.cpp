#include "OpenGLCommandList.h"
namespace BlackPearl{
	CommandList::CommandList(Device* device, const OpenGLContext& context, const CommandListParameters& parameters)
	{
	}
	void CommandList::open()
	{
	}
	void CommandList::close()
	{
	}
	void CommandList::clearState()
	{
	}
	void CommandList::clearTextureFloat(ITexture* texture, TextureSubresourceSet subresources, const Color& clearColor)
	{
	}
	void CommandList::clearDepthStencilTexture(ITexture* texture, TextureSubresourceSet subresources, bool clearDepth, float depth, bool clearStencil, uint8_t stencil)
	{
	}
	void CommandList::clearTextureUInt(ITexture* texture, TextureSubresourceSet subresources, uint32_t clearColor)
	{
	}
	void CommandList::copyTexture(ITexture* dest, const TextureSlice& destSlice, ITexture* src, const TextureSlice& srcSlice)
	{
	}
	void CommandList::copyTexture(IStagingTexture* dest, const TextureSlice& dstSlice, ITexture* src, const TextureSlice& srcSlice)
	{
	}
	void CommandList::copyTexture(ITexture* dest, const TextureSlice& dstSlice, IStagingTexture* src, const TextureSlice& srcSlice)
	{
	}
	void CommandList::writeTexture(ITexture* dest, uint32_t arraySlice, uint32_t mipLevel, const void* data, size_t rowPitch, size_t depthPitch)
	{
	}
	void CommandList::resolveTexture(ITexture* dest, const TextureSubresourceSet& dstSubresources, ITexture* src, const TextureSubresourceSet& srcSubresources)
	{
	}
	void CommandList::writeBuffer(IBuffer* b, const void* data, size_t dataSize, uint64_t destOffsetBytes)
	{
	}
	void CommandList::clearBufferUInt(IBuffer* b, uint32_t clearValue)
	{
	}
	void CommandList::copyBuffer(IBuffer* dest, uint64_t destOffsetBytes, IBuffer* src, uint64_t srcOffsetBytes, uint64_t dataSizeBytes)
	{
	}
	void CommandList::setPushConstants(const void* data, size_t byteSize)
	{
	}
	void CommandList::setGraphicsState(const GraphicsState& state)
	{
	}
	void CommandList::draw(const DrawArguments& args)
	{
	}
	void CommandList::drawIndexed(const DrawArguments& args)
	{
	}
	void CommandList::drawIndirect(uint32_t offsetBytes, uint32_t drawCount)
	{
	}
	void CommandList::drawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount)
	{
	}
	void CommandList::setComputeState(const ComputeState& state)
	{
	}
	void CommandList::dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
	{
	}
	void CommandList::dispatchIndirect(uint32_t offsetBytes)
	{
	}
	void CommandList::setMeshletState(const MeshletState& state)
	{
	}
	void CommandList::dispatchMesh(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
	{
	}
	void CommandList::setRayTracingState(const RayTracingState& state)
	{
	}
	void CommandList::dispatchRays(const DispatchRaysArguments& args)
	{
	}
	void CommandList::buildOpacityMicromap(rt::IOpacityMicromap* omm, const rt::OpacityMicromapDesc& desc)
	{
	}
	void CommandList::buildBottomLevelAccelStruct(rt::IAccelStruct* as, const rt::GeometryDesc* pGeometries, size_t numGeometries, rt::AccelStructBuildFlags buildFlags)
	{
	}
	void CommandList::compactBottomLevelAccelStructs()
	{
	}
	void CommandList::buildTopLevelAccelStruct(rt::IAccelStruct* as, const rt::InstanceDesc* pInstances, size_t numInstances, rt::AccelStructBuildFlags buildFlags)
	{
	}
	void CommandList::buildTopLevelAccelStructFromBuffer(rt::IAccelStruct* as, IBuffer* instanceBuffer, uint64_t instanceBufferOffset, size_t numInstances, rt::AccelStructBuildFlags buildFlags)
	{
	}
	void CommandList::beginTimerQuery(ITimerQuery* query)
	{
	}
	void CommandList::endTimerQuery(ITimerQuery* query)
	{
	}
	void CommandList::beginMarker(const char* name)
	{
	}
	void CommandList::endMarker()
	{
	}
	void CommandList::setEnableAutomaticBarriers(bool enable)
	{
	}
	void CommandList::setResourceStatesForBindingSet(IBindingSet* bindingSet)
	{
	}
	void CommandList::setEnableUavBarriersForTexture(ITexture* texture, bool enableBarriers)
	{
	}
	void CommandList::setEnableUavBarriersForBuffer(IBuffer* buffer, bool enableBarriers)
	{
	}
	void CommandList::beginTrackingTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits)
	{
	}
	void CommandList::beginTrackingBufferState(IBuffer* buffer, ResourceStates stateBits)
	{
	}
	void CommandList::setTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits)
	{
	}
	void CommandList::setBufferState(IBuffer* buffer, ResourceStates stateBits)
	{
	}
	void CommandList::setPermanentTextureState(ITexture* texture, ResourceStates stateBits)
	{
	}
	void CommandList::setPermanentBufferState(IBuffer* buffer, ResourceStates stateBits)
	{
	}
	void CommandList::commitBarriers()
	{
	}
	ResourceStates CommandList::getTextureSubresourceState(ITexture* texture, uint32_t arraySlice, uint32_t mipLevel)
	{
		return ResourceStates();
	}
	ResourceStates CommandList::getBufferState(IBuffer* buffer)
	{
		return ResourceStates();
	}
}
