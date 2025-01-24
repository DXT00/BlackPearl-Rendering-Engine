#pragma once
#include "RHIPipeline.h"
#include "RHIBuffer.h"
#include "RHIBindingSet.h"
#include "RHIMessageCallback.h"
#include "RHIDefinitions.h"

namespace BlackPearl {


    struct GraphicsState
    {
        IGraphicsPipeline* pipeline = nullptr;
        IFramebuffer* framebuffer = nullptr;
        ViewportState viewport;
        Color blendConstantColor{};
        VariableRateShadingState shadingRateState;

        BindingSetVector bindings;

        std::vector<VertexBufferBinding> vertexBuffers;
        IndexBufferBinding indexBuffer;

        IBuffer* indirectParams = nullptr;


        //都在pipeline的desc里

       /* FBoundShaderStateInput			BoundShaderState;
        BlendState* BlendState;
        RasterState* RasterizerState;
        DepthStencilState* DepthStencilState;
        FImmutableSamplerState			ImmutableSamplerState;*/


        GraphicsState& setPipeline(IGraphicsPipeline* value) { pipeline = value; return *this; }
        GraphicsState& setFramebuffer(IFramebuffer* value) { framebuffer = value; return *this; }
        GraphicsState& setViewport(const ViewportState& value) { viewport = value; return *this; }
        GraphicsState& setBlendColor(const Color& value) { blendConstantColor = value; return *this; }
        GraphicsState& addBindingSet(IBindingSet* value) { bindings.push_back(value); return *this; }
        GraphicsState& addVertexBuffer(const VertexBufferBinding& value) { vertexBuffers.push_back(value); return *this; }
        GraphicsState& setIndexBuffer(const IndexBufferBinding& value) { indexBuffer = value; return *this; }
        GraphicsState& setIndirectParams(IBuffer* value) { indirectParams = value; return *this; }
    };

    struct ComputeState
    {
        IComputePipeline* pipeline = nullptr;

        BindingSetVector bindings;

        IBuffer* indirectParams = nullptr;

        ComputeState& setPipeline(IComputePipeline* value) { pipeline = value; return *this; }
        ComputeState& addBindingSet(IBindingSet* value) { bindings.push_back(value); return *this; }
        ComputeState& setIndirectParams(IBuffer* value) { indirectParams = value; return *this; }
    };

    struct MeshletState
    {
        IMeshletPipeline* pipeline = nullptr;
        IFramebuffer* framebuffer = nullptr;
        ViewportState viewport;
        Color blendConstantColor{};

        BindingSetVector bindings;

        IBuffer* indirectParams = nullptr;

        MeshletState& setPipeline(IMeshletPipeline* value) { pipeline = value; return *this; }
        MeshletState& setFramebuffer(IFramebuffer* value) { framebuffer = value; return *this; }
        MeshletState& setViewport(const ViewportState& value) { viewport = value; return *this; }
        MeshletState& setBlendColor(const Color& value) { blendConstantColor = value; return *this; }
        MeshletState& addBindingSet(IBindingSet* value) { bindings.push_back(value); return *this; }
        MeshletState& setIndirectParams(IBuffer* value) { indirectParams = value; return *this; }
    };


    struct RayTracingState
    {
        IShaderTable* shaderTable = nullptr;

        BindingSetVector bindings;

        RayTracingState& setShaderTable(IShaderTable* value) { shaderTable = value; return *this; }
        RayTracingState& addBindingSet(IBindingSet* value) { bindings.push_back(value); return *this; }
    };



    struct BufferStateExtension
    {
        const BufferDesc& descRef;
        ResourceStates permanentState = ResourceStates::Unknown;

        explicit BufferStateExtension(const BufferDesc& desc)
            : descRef(desc)
        { }
    };

    struct TextureStateExtension
    {
        const TextureDesc& descRef;
        ResourceStates permanentState = ResourceStates::Unknown;
        bool stateInitialized = false;

        explicit TextureStateExtension(const TextureDesc& desc)
            : descRef(desc)
        { }
    };

    struct TextureState
    {
        std::vector<ResourceStates> subresourceStates;
        ResourceStates state = ResourceStates::Unknown;
        bool enableUavBarriers = true;
        bool firstUavBarrierPlaced = false;
        bool permanentTransition = false;
    };

    struct BufferState
    {
        ResourceStates state = ResourceStates::Unknown;
        bool enableUavBarriers = true;
        bool firstUavBarrierPlaced = false;
        bool permanentTransition = false;
    };

    struct VolatileBufferState
    {
        int latestVersion = 0;
        int minVersion = 0;
        int maxVersion = 0;
        bool initialized = false;
    };


    struct TextureBarrier
    {
        TextureStateExtension* texture = nullptr;
        uint32_t mipLevel = 0;
        uint32_t arraySlice = 0;
        bool entireTexture = false;
        ResourceStates stateBefore = ResourceStates::Unknown;
        ResourceStates stateAfter = ResourceStates::Unknown;
    };

    struct BufferBarrier
    {
        BufferStateExtension* buffer = nullptr;
        ResourceStates stateBefore = ResourceStates::Unknown;
        ResourceStates stateAfter = ResourceStates::Unknown;
    };

    class CommandListResourceStateTracker
    {
    public:
        explicit CommandListResourceStateTracker(IMessageCallback* messageCallback)
            : m_MessageCallback(messageCallback)
        { }

        // ICommandList-like interface

        void setEnableUavBarriersForTexture(TextureStateExtension* texture, bool enableBarriers);
        void setEnableUavBarriersForBuffer(BufferStateExtension* buffer, bool enableBarriers);

        void beginTrackingTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStates stateBits);
        void beginTrackingBufferState(BufferStateExtension* buffer, ResourceStates stateBits);

        void endTrackingTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStates stateBits, bool permanent);
        void endTrackingBufferState(BufferStateExtension* buffer, ResourceStates stateBits, bool permanent);

        ResourceStates getTextureSubresourceState(TextureStateExtension* texture, uint32_t arraySlice, uint32_t mipLevel);
        ResourceStates getBufferState(BufferStateExtension* buffer);

        // Internal interface

        void requireTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStates state);
        void requireBufferState(BufferStateExtension* buffer, ResourceStates state);

        void keepBufferInitialStates();
        void keepTextureInitialStates();
        void commandListSubmitted();

        [[nodiscard]] const std::vector<TextureBarrier>& getTextureBarriers() const { return m_TextureBarriers; }
        [[nodiscard]] const std::vector<BufferBarrier>& getBufferBarriers() const { return m_BufferBarriers; }
        void clearBarriers() { m_TextureBarriers.clear(); m_BufferBarriers.clear(); }

    private:
        IMessageCallback* m_MessageCallback;

        std::unordered_map<TextureStateExtension*, std::unique_ptr<TextureState>> m_TextureStates;
        std::unordered_map<BufferStateExtension*, std::unique_ptr<BufferState>> m_BufferStates;

        // Deferred transitions of textures and buffers to permanent states.
        // They are executed only when the command list is executed, not when the app calls endTrackingTextureState.
        std::vector<std::pair<TextureStateExtension*, ResourceStates>> m_PermanentTextureStates;
        std::vector<std::pair<BufferStateExtension*, ResourceStates>> m_PermanentBufferStates;

        std::vector<TextureBarrier> m_TextureBarriers;
        std::vector<BufferBarrier> m_BufferBarriers;

        TextureState* getTextureStateTracking(TextureStateExtension* texture, bool allowCreate);
        BufferState* getBufferStateTracking(BufferStateExtension* buffer, bool allowCreate);
    };

    bool verifyPermanentResourceState(ResourceStates permanentState, ResourceStates requiredState, bool isTexture, const std::string& debugName, IMessageCallback* messageCallback);
}

