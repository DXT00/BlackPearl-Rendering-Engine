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
#include "RHIResources.h"
#include "RHITexture.h"
#include "RHIBuffer.h"
#include "RHIState.h"
#include "RHIQuery.h"
#include "RHIDefinitions.h"

namespace BlackPearl {
    class IDevice;
    struct CommandListParameters
    {
        // A command list with enableImmediateExecution = true maps to the immediate context on DX11.
        // Two immediate command lists cannot be open at the same time, which is checked by the validation layer.
        bool enableImmediateExecution = true;

        // Minimum size of memory chunks created to upload data to the device on DX12.
        size_t uploadChunkSize = 64 * 1024;

        // Minimum size of memory chunks created for AS build scratch buffers.
        size_t scratchChunkSize = 64 * 1024;

        // Maximum total memory size used for all AS build scratch buffers owned by this command list.
        size_t scratchMaxMemory = 1024 * 1024 * 1024;

        // Type of the queue that this command list is to be executed on.
        // COPY and COMPUTE queues have limited subsets of methods available.
        CommandQueue queueType = CommandQueue::Graphics;

        CommandListParameters& setEnableImmediateExecution(bool value) { enableImmediateExecution = value; return *this; }
        CommandListParameters& setUploadChunkSize(size_t value) { uploadChunkSize = value; return *this; }
        CommandListParameters& setScratchChunkSize(size_t value) { scratchChunkSize = value; return *this; }
        CommandListParameters& setScratchMaxMemory(size_t value) { scratchMaxMemory = value; return *this; }
        CommandListParameters& setQueueType(CommandQueue value) { queueType = value; return *this; }
    };

	class ICommandList : public IResource
	{
    public:
        virtual void open() = 0;
        virtual void close() = 0;

        // Clears the graphics state of the underlying command list object and resets the state cache.
        virtual void clearState() = 0;

        virtual void clearTextureFloat(ITexture* t, TextureSubresourceSet subresources, const Color& clearColor) = 0;
        virtual void clearDepthStencilTexture(ITexture* t, TextureSubresourceSet subresources, bool clearDepth, float depth, bool clearStencil, uint8_t stencil) = 0;
        virtual void clearTextureUInt(ITexture* t, TextureSubresourceSet subresources, uint32_t clearColor) = 0;

        virtual void copyTexture(ITexture* dest, const TextureSlice& destSlice, ITexture* src, const TextureSlice& srcSlice) = 0;
        virtual void copyTexture(IStagingTexture* dest, const TextureSlice& destSlice, ITexture* src, const TextureSlice& srcSlice) = 0;
        virtual void copyTexture(ITexture* dest, const TextureSlice& destSlice, IStagingTexture* src, const TextureSlice& srcSlice) = 0;
        virtual void writeTexture(ITexture* dest, uint32_t arraySlice, uint32_t mipLevel, const void* data, size_t rowPitch, size_t depthPitch = 0) = 0;
        virtual void resolveTexture(ITexture* dest, const TextureSubresourceSet& dstSubresources, ITexture* src, const TextureSubresourceSet& srcSubresources) = 0;

        virtual void writeBuffer(IBuffer* b, const void* data, size_t dataSize, uint64_t destOffsetBytes = 0) = 0;
        virtual void clearBufferUInt(IBuffer* b, uint32_t clearValue) = 0;
        virtual void copyBuffer(IBuffer* dest, uint64_t destOffsetBytes, IBuffer* src, uint64_t srcOffsetBytes, uint64_t dataSizeBytes) = 0;

        // Sets the push constants block on the command list, aka "root constants" on DX12.
        // Only valid after setGraphicsState or setComputeState etc.
        virtual void setPushConstants(const void* data, size_t byteSize) = 0;
        virtual void setBoundShaderState(BoundShaderState* state) = 0;

        virtual void setDepthStencilaState(DepthStencilState* state) {}
        virtual void setRasterizerState(RasterState* state) {}
        virtual void setBlendState(BlendState* state) {}


        virtual void setGraphicsState(const GraphicsState& state) = 0;
        virtual void draw(const DrawArguments& args) = 0;
        virtual void drawIndexed(const DrawArguments& args) = 0;
        virtual void drawIndirect(uint32_t offsetBytes, uint32_t drawCount = 1) = 0;
        virtual void drawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount = 1) = 0;

        virtual void setComputeState(const ComputeState& state) = 0;
        virtual void dispatch(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) = 0;
        virtual void dispatchIndirect(uint32_t offsetBytes) = 0;

        virtual void setMeshletState(const MeshletState& state) = 0;
        virtual void dispatchMesh(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) = 0;

        virtual void setRayTracingState(const RayTracingState& state) = 0;
        virtual void dispatchRays(const DispatchRaysArguments& args) = 0;

        virtual void buildOpacityMicromap(rt::IOpacityMicromap* omm, const rt::OpacityMicromapDesc& desc) = 0;

        virtual void buildBottomLevelAccelStruct(rt::IAccelStruct* as, const rt::GeometryDesc* pGeometries, size_t numGeometries,
            rt::AccelStructBuildFlags buildFlags = rt::AccelStructBuildFlags::None) = 0;
        virtual void compactBottomLevelAccelStructs() = 0;
        virtual void buildTopLevelAccelStruct(rt::IAccelStruct* as, const rt::InstanceDesc* pInstances, size_t numInstances,
            rt::AccelStructBuildFlags buildFlags = rt::AccelStructBuildFlags::None) = 0;

        // A version of buildTopLevelAccelStruct that takes the instance data from a buffer on the GPU.
        // The buffer must be pre-filled with rt::InstanceDesc structures using a copy operation or a shader.
        // No validation on the buffer contents is performed by NVRHI, and no state or liveness tracking for the referenced BLAS'es.
        virtual void buildTopLevelAccelStructFromBuffer(rt::IAccelStruct* as, IBuffer* instanceBuffer, uint64_t instanceBufferOffset, size_t numInstances,
            rt::AccelStructBuildFlags buildFlags = rt::AccelStructBuildFlags::None) = 0;

        virtual void beginTimerQuery(ITimerQuery* query) = 0;
        virtual void endTimerQuery(ITimerQuery* query) = 0;

        // Command list range markers
        virtual void beginMarker(const char* name) = 0;
        virtual void endMarker() = 0;

        // Enables or disables the automatic barrier placement on set[...]State, copy, write, and clear operations.
        // By default, automatic barriers are enabled, but can be optionally disabled to improve CPU performance and/or specific barrier placement.
        // When automatic barriers are disabled, it is application's responsibility to set correct states for all used resources.
        virtual void setEnableAutomaticBarriers(bool enable) = 0;

        // Sets the necessary resource states for all non-permanent resources used in the binding set.
        virtual void setResourceStatesForBindingSet(IBindingSet* bindingSet) = 0;

        // Sets the necessary resource states for all targets of the framebuffer.
        void setResourceStatesForFramebuffer(IFramebuffer* framebuffer);

        // Tells the D3D12/VK backend whether UAV barriers should be used for the given texture or buffer between draw calls.
        // A barrier should still be placed before the first draw call in the group and after the last one.
        virtual void setEnableUavBarriersForTexture(ITexture* texture, bool enableBarriers) = 0;
        virtual void setEnableUavBarriersForBuffer(IBuffer* buffer, bool enableBarriers) = 0;

        // Informs the command list of the state of a texture subresource or buffer prior to command list execution
        virtual void beginTrackingTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits) = 0;
        virtual void beginTrackingBufferState(IBuffer* buffer, ResourceStates stateBits) = 0;

        // Resource state transitions - these put barriers into the pending list. Call commitBarriers() after.
        virtual void setTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits) = 0;
        virtual void setBufferState(IBuffer* buffer, ResourceStates stateBits) = 0;
        //virtual void setAccelStructState(rt::IAccelStruct* as, ResourceStates stateBits) = 0;

        // Permanent resource state transitions - these make resource usage cheaper by excluding it from state tracking in the future.
        // Like setTexture/BufferState, these methods put barriers into the pending list. Call commitBarriers() after.
        virtual void setPermanentTextureState(ITexture* texture, ResourceStates stateBits) = 0;
        virtual void setPermanentBufferState(IBuffer* buffer, ResourceStates stateBits) = 0;

        // Flushes the barriers from the pending list into the GAPI command list.
        virtual void commitBarriers() = 0;

        // Returns the current tracked state of a texture subresource or a buffer.
        virtual ResourceStates getTextureSubresourceState(ITexture* texture, uint32_t arraySlice, uint32_t mipLevel) = 0;
        virtual ResourceStates getBufferState(IBuffer* buffer) = 0;

        // Returns the owning device, does NOT call AddRef on it
        virtual IDevice* getDevice() = 0;
        virtual const CommandListParameters& getDesc() = 0;

        virtual const GraphicsState& GetLastGraphicsState() const = 0;
        virtual void setViewport(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {}
        virtual void setScissorRect(bool bEnable, uint32_t minX, uint32_t minY, uint32_t maxX, uint32_t maxY) {}
    };

    typedef RefCountPtr<ICommandList> CommandListHandle;

}

