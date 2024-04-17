#pragma once
#include "RHIPipeline.h"
#include "RHIBuffer.h"
#include "RHIBindingSet.h"
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
}

