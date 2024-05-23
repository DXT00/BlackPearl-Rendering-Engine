#include "pch.h"
#include "RHIState.h"
namespace BlackPearl {
    void CommandListResourceStateTracker::setEnableUavBarriersForTexture(TextureStateExtension* texture, bool enableBarriers)
    {
    }
    void CommandListResourceStateTracker::setEnableUavBarriersForBuffer(BufferStateExtension* buffer, bool enableBarriers)
    {
    }
    void CommandListResourceStateTracker::beginTrackingTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStates stateBits)
    {
    }
    void CommandListResourceStateTracker::beginTrackingBufferState(BufferStateExtension* buffer, ResourceStates stateBits)
    {
    }
    void CommandListResourceStateTracker::endTrackingTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStates stateBits, bool permanent)
    {
    }
    void CommandListResourceStateTracker::endTrackingBufferState(BufferStateExtension* buffer, ResourceStates stateBits, bool permanent)
    {
    }
    ResourceStates CommandListResourceStateTracker::getTextureSubresourceState(TextureStateExtension* texture, uint32_t arraySlice, uint32_t mipLevel)
    {
        return ResourceStates();
    }
    ResourceStates CommandListResourceStateTracker::getBufferState(BufferStateExtension* buffer)
    {
        return ResourceStates();
    }
    void CommandListResourceStateTracker::requireTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStates state)
    {
    }
    void CommandListResourceStateTracker::requireBufferState(BufferStateExtension* buffer, ResourceStates state)
    {
    }
    void CommandListResourceStateTracker::keepBufferInitialStates()
    {
    }
    void CommandListResourceStateTracker::keepTextureInitialStates()
    {
    }
    void CommandListResourceStateTracker::commandListSubmitted()
    {
    }
    TextureState* CommandListResourceStateTracker::getTextureStateTracking(TextureStateExtension* texture, bool allowCreate)
    {
        return nullptr;
    }
    BufferState* CommandListResourceStateTracker::getBufferStateTracking(BufferStateExtension* buffer, bool allowCreate)
    {
        return nullptr;
    }
    bool verifyPermanentResourceState(ResourceStates permanentState, ResourceStates requiredState, bool isTexture, const std::string& debugName, IMessageCallback* messageCallback)
    {
        return false;
    }
}