#pragma once
#include "BlackPearl/RHI/RHI.h"
#include "BlackPearl/RHI/RHICommandList.h"
#include "BlackPearl/RHI/RHIState.h"
#include "OpenGLDevice.h"
#include "OpenGLContext.h"
#include "OpenGLViewport.h"
namespace BlackPearl {
	class CommandList :public RefCounter<ICommandList> {
        // Internal backend methods
    public:
        CommandList(Device* device, const OpenGLContext& context, const CommandListParameters& parameters);

       // void executed(Queue& queue, uint64_t submissionID);

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

        void setGraphicsState(const GraphicsState& state) override;
        void draw(const DrawArguments& args) override;
        void drawIndexed(const DrawArguments& args) override;
        void drawIndirect(uint32_t offsetBytes, uint32_t drawCount) override;
        void drawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount) override;

        void setComputeState(const ComputeState& state) override;
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

      //  TrackedCommandBufferPtr getCurrentCmdBuf() const { return m_CurrentCmdBuf; }

        const GraphicsState& GetLastGraphicsState() const { return m_CurrentGraphicsState; }
    private:
        void _setViewport(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
    
    private:
        Device* m_Device;
        const OpenGLContext& m_Context;

        CommandListParameters m_CommandListParameters;

      
 

        ///** Query list. This is used to inform queries they're no longer valid when OpenGL context they're in gets released from another thread. */
        //std::vector<FOpenGLRenderQuery*> Queries;

        ///** A critical section to protect modifications and iteration over Queries list */
        //FCriticalSection QueriesListCriticalSection;

        //FOpenGLGPUProfiler GPUProfilingData;
        //friend FOpenGLGPUProfiler;

        //FCriticalSection CustomPresentSection;
        //TRefCountPtr<class FRHICustomPresent> CustomPresent;



        void _endRenderPass();

        void _trackResourcesAndBarriers(const GraphicsState& state);
        void _trackResourcesAndBarriers(const MeshletState& state);

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

       
	};
}