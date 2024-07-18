#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/RHI/RHIBindingLayoutDesc.h"
#include "BlackPearl/Math/vector.h"
#include "BlackPearl/Renderer/SceneType.h"
#include "BlackPearl/Renderer/DrawStrategy.h"
namespace BlackPearl {
   


	class GeometryRenderer : public BasicRenderer
	{
    public:
        //[[nodiscard]] virtual ViewType::Enum GetSupportedViewTypes() const = 0;
        virtual void SetupView(ICommandList* commandList, const IView* view, const IView* viewPrev) = 0;
        virtual bool SetupMaterial(const Material* material, RasterCullMode cullMode, GraphicsState& state) = 0;
        virtual void SetupInputBuffers(BufferGroup* buffers, GraphicsState& state) = 0;
        virtual void SetPushConstants(ICommandList* commandList, GraphicsState& state, DrawArguments& args) = 0;
        virtual ~GeometryRenderer() = default;
	};

    void RenderView(
        ICommandList* commandList,
        IFramebuffer* framebuffer,
        IView* view,
        IView* viewPrev,
        IDrawStrategy* drawStrategy,
        GeometryRenderer* pass,
        bool materialEvents = false);

  /*  void RenderCompositeView(
        ICommandList* commandList,
        const engine::ICompositeView* compositeView,
        const engine::ICompositeView* compositeViewPrev,
        engine::FramebufferFactory& framebufferFactory,
        const std::shared_ptr<engine::SceneGraphNode>& rootNode,
        IDrawStrategy& drawStrategy,
        IGeometryPass& pass,
        GeometryPassContext& passContext,
        const char* passEvent = nullptr,
        bool materialEvents = false);*/

}

