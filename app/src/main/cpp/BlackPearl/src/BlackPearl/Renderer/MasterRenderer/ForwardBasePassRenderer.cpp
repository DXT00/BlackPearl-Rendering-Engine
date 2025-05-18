#include "pch.h"
#include "Renderer/MasterRenderer/ForwardBasePassRenderer.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"
#include "hlsl/core/forward_cb.h"
#include "hlsl/core/transform_cb.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#include "BlackPearl/Core.h"

namespace BlackPearl {


    void ForwardBasePassRenderer::Render(const std::vector<Object*>& objs)
    {
        //		m_PbrShader->Bind();
        //		DrawObjects(objs, m_PbrShader);
    }
    void ForwardBasePassRenderer::Init()
    {
        
        m_DrawStrategy = DBG_NEW InstancedOpaqueDrawStrategy();
        ShaderDesc desc = ShaderDesc(ShaderType::All);
        desc.debugName = "PbrShader";
        m_PbrShader = DBG_NEW MaterialShader("assets/shaders/glsl/pbr/PbrTexture.glsl");



        /*auto samplerDesc = SamplerDesc()
            .setAllAddressModes(SamplerAddressMode::Border)
            .setBorderColor(1.0f);
        m_ShadowSampler = m_Device->createSampler(samplerDesc);

        m_ForwardViewCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(ForwardShadingViewConstants), "ForwardShadingViewConstants"));

        m_ObjectTransformCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(TransformConstants), "TransformConstants"));*/


        m_ShaderParameters[ShaderType::Pixel].bindingLayouts.push_back(m_ViewBindinglayout);
        m_ShaderParameters[ShaderType::Pixel].bindingSets.push_back(m_ViewBindingset);
        //m_ShaderParameters[ShaderType::VertexShader].inputLayout = inputLayout;
    }

    void ForwardBasePassRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        commandList->beginMarker("BasePass");
        GE_ERROR_JUDGE();

    

        SceneData* view = Renderer::GetSceneData();
        GE_ERROR_JUDGE();

        SceneData* preView = Renderer::GetPreSceneData();
        GE_ERROR_JUDGE();

        SetupView(commandList, view, preView);

        m_DrawStrategy->PrepareForView(scene, *view);
        GE_ERROR_JUDGE();

        RenderPassTemplate(commandList, targetFramebuffer, view, m_DrawStrategy, m_ShaderParameters);

        
        commandList->endMarker();
    }

    void ForwardBasePassRenderer::Render(Object* obj)
    {
        //		m_PbrShader->Bind();
        //		DrawObject(obj, m_PbrShader);
    }

    ForwardBasePassRenderer::~ForwardBasePassRenderer()
    {
    }

}
