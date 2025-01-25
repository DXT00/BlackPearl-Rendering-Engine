/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#pragma once
#if GE_API_VULKAN

#include "BlackPearl/RHI/RHI.h"
#include "BlackPearl/RHI/RHICommandList.h"
#include "BlackPearl/RHI/RHIState.h"
#include "VkDevice.h"
#include "VkQueue.h"
#include "VkBuffer.h"
#include "VkCommandBuffer.h"
#include "VkUploadManager.h"
#include "VkRayTraceStruct.h"
#include <vulkan/vulkan_core.h>
namespace BlackPearl {
	class CommandList : public RefCounter<ICommandList>
	{
	public:
        // Internal backend methods

        CommandList(Device* device, const VulkanContext& context, const CommandListParameters& parameters);

        void executed(Queue& queue, uint64_t submissionID);

        // IResource implementation

        // ICommandList implementation

        void open() override;
        void close() override;
        void clearState() override;

        void clearTextureFloat(ITexture* texture, TextureSubresourceSet subresources, const Color& clearColor) override;
        void clearDepthStencilTexture(ITexture* texture, TextureSubresourceSet subresources, bool clearDepth, float depth, bool clearStencil, uint8_t stencil) override;
        void clearTextureUInt(ITexture* texture, TextureSubresourceSet subresources, uint32_t clearColor) override;

        void copyTexture(ITexture* dest, const TextureSlice& destSlice, ITexture* src, const TextureSlice& srcSlice) override;
        void copyTexture(IStagingTexture* dest, const TextureSlice& dstSlice, ITexture* src, const TextureSlice& srcSlice) override;
        void copyTexture(ITexture* dest, const TextureSlice& dstSlice, IStagingTexture* src, const TextureSlice& srcSlice) override;
        void writeTexture(ITexture* dest, uint32_t arraySlice, uint32_t mipLevel, const void* data, size_t rowPitch, size_t depthPitch) override;
        void resolveTexture(ITexture* dest, const TextureSubresourceSet& dstSubresources, ITexture* src, const TextureSubresourceSet& srcSubresources) override;

        void writeBuffer(IBuffer* b, const void* data, size_t dataSize, uint64_t destOffsetBytes = 0) override;
        void clearBufferUInt(IBuffer* b, uint32_t clearValue) override;
        void copyBuffer(IBuffer* dest, uint64_t destOffsetBytes, IBuffer* src, uint64_t srcOffsetBytes, uint64_t dataSizeBytes) override;

        void setPushConstants(const void* data, size_t byteSize) override;

        void setBoundShaderState(const BoundShaderState& state) override;
        void setGraphicsState(const GraphicsState& state) override;
        void setComputeState(const ComputeState& state) override;

        void draw(const DrawArguments& args) override;
        void drawIndexed(const DrawArguments& args) override;
        void drawIndirect(uint32_t offsetBytes, uint32_t drawCount) override;
        void drawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount) override;

        void dispatch(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) override;
        void dispatchIndirect(uint32_t offsetBytes)  override;

        void setMeshletState(const MeshletState& state) override;
        void dispatchMesh(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) override;

        void setRayTracingState(const RayTracingState& state) override;
        void dispatchRays(const DispatchRaysArguments& args) override;

        void buildOpacityMicromap(rt::IOpacityMicromap* omm, const rt::OpacityMicromapDesc& desc) override;
        void buildBottomLevelAccelStruct(rt::IAccelStruct* as, const rt::GeometryDesc* pGeometries, size_t numGeometries, rt::AccelStructBuildFlags buildFlags) override;
        void compactBottomLevelAccelStructs() override;
        void buildTopLevelAccelStruct(rt::IAccelStruct* as, const rt::InstanceDesc* pInstances, size_t numInstances, rt::AccelStructBuildFlags buildFlags) override;
        void buildTopLevelAccelStructFromBuffer(rt::IAccelStruct* as, IBuffer* instanceBuffer, uint64_t instanceBufferOffset, size_t numInstances,
            rt::AccelStructBuildFlags buildFlags = rt::AccelStructBuildFlags::None) override;

        void beginTimerQuery(ITimerQuery* query) override;
        void endTimerQuery(ITimerQuery* query) override;

        void beginMarker(const char* name) override;
        void endMarker() override;

        void setEnableAutomaticBarriers(bool enable) override;
        void setResourceStatesForBindingSet(IBindingSet* bindingSet) override;

        void setEnableUavBarriersForTexture(ITexture* texture, bool enableBarriers) override;
        void setEnableUavBarriersForBuffer(IBuffer* buffer, bool enableBarriers) override;

        void beginTrackingTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits) override;
        void beginTrackingBufferState(IBuffer* buffer, ResourceStates stateBits) override;

        void setTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits) override;
        void setBufferState(IBuffer* buffer, ResourceStates stateBits) override;
        //void setAccelStructState(rt::IAccelStruct* _as, ResourceStates stateBits) override;

        void setPermanentTextureState(ITexture* texture, ResourceStates stateBits) override;
        void setPermanentBufferState(IBuffer* buffer, ResourceStates stateBits) override;

        void commitBarriers() override;

        ResourceStates getTextureSubresourceState(ITexture* texture, uint32_t arraySlice, uint32_t mipLevel) override;
        ResourceStates getBufferState(IBuffer* buffer) override;

        IDevice* getDevice() override { return m_Device; }
        const CommandListParameters& getDesc() override { return m_CommandListParameters; }

        TrackedCommandBufferPtr getCurrentCmdBuf() const { return m_CurrentCmdBuf; }

        const GraphicsState& GetLastGraphicsState() const { return m_CurrentGraphicsState; }
    private:
        Device* m_Device;
        const VulkanContext& m_Context;

        CommandListParameters m_CommandListParameters;

        CommandListResourceStateTracker m_StateTracker;
        bool m_EnableAutomaticBarriers = true;

        // current internal command buffer
        TrackedCommandBufferPtr m_CurrentCmdBuf = nullptr;

        VkPipelineLayout m_CurrentPipelineLayout;
        VkShaderStageFlags m_CurrentPushConstantsVisibility;
        GraphicsState m_CurrentGraphicsState{};
        ComputeState m_CurrentComputeState{};
        MeshletState m_CurrentMeshletState{};
        RayTracingState m_CurrentRayTracingState;
        bool m_AnyVolatileBufferWrites = false;

        struct ShaderTableState
        {
            VkStridedDeviceAddressRegionKHR rayGen;
            VkStridedDeviceAddressRegionKHR miss;
            VkStridedDeviceAddressRegionKHR hitGroups;
            VkStridedDeviceAddressRegionKHR callable;
            uint32_t version = 0;
        } m_CurrentShaderTablePointers;

        std::unordered_map<Buffer*, VolatileBufferState> m_VolatileBufferStates;

        std::unique_ptr<UploadManager> m_UploadManager;
        std::unique_ptr<UploadManager> m_ScratchManager;

        void _clearTexture(ITexture* texture, TextureSubresourceSet subresources, const VkClearColorValue& clearValue);

        void _bindBindingSets(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, const BindingSetVector& bindings);

        void _endRenderPass();

        void _trackResourcesAndBarriers(const GraphicsState& state);
        void _trackResourcesAndBarriers(const MeshletState& state);

        void _writeVolatileBuffer(Buffer* buffer, const void* data, size_t dataSize);
        void _flushVolatileBufferWrites();
        void _submitVolatileBuffers(uint64_t recordingID, uint64_t submittedID);

        void _updateGraphicsVolatileBuffers();
        void _updateComputeVolatileBuffers();
        void _updateMeshletVolatileBuffers();
        void _updateRayTracingVolatileBuffers();

        void _requireTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates state);
        void _requireBufferState(IBuffer* buffer, ResourceStates state);
        bool _anyBarriers() const;

        //void buildTopLevelAccelStructInternal(AccelStruct* as, VkDeviceAddress instanceData, size_t numInstances, rt::AccelStructBuildFlags buildFlags, uint64_t currentVersion);

        void _commitBarriersInternal();
        void _commitBarriersInternal_synchronization2();

        void _buildTopLevelAccelStructInternal(AccelStruct* as, VkDeviceAddress instanceData, size_t numInstances, rt::AccelStructBuildFlags buildFlags, uint64_t currentVersion);

        // rayTracing
        void _convertBottomLevelGeometry(const rt::GeometryDesc& src, VkAccelerationStructureGeometryKHR& dst, VkAccelerationStructureTrianglesOpacityMicromapEXT& dstOmm,
            uint32_t& maxPrimitiveCount, VkAccelerationStructureBuildRangeInfoKHR* pRange, const VulkanContext& context);
	};

}

#endif
