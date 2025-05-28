#include "pch.h"
#include "Renderer/MasterRenderer/ForwardShadingRenderer.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"
#include "hlsl/core/forward_cb.h"
#include "hlsl/core/transform_cb.h"
#include "BlackPearl/Core.h"
#ifdef GE_API_OPENGL
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#endif
#include "Component/LightComponent/DirectionLight.h"
#include "Component/LightComponent/PointLight.h"
#include "Component/LightComponent/SpotLight.h"
namespace BlackPearl {


    void ForwardShadingRenderer::Render(const std::vector<Object*>& objs)
    {
        //		m_PbrShader->Bind();
        //		DrawObjects(objs, m_PbrShader);
    }
    void ForwardShadingRenderer::Init()
    {
        
        m_DrawStrategy = DBG_NEW InstancedOpaqueDrawStrategy();

        m_ForwardShadingShader = DBG_NEW MaterialShader("assets/shaders/glsl/forward_shading/forward_shading.glsl");


        //Deferred Shading Material
        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(8) //           ForwardShadingLightConstants
        };
        m_ForwardLightsBindingLayout = m_Device->createBindingLayout(layoutDesc);


        m_LightsCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(ForwardShadingLightConstants), "ForwardShadingLightConstants"));

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::ConstantBuffer(8, m_LightsCB),
        };
        m_ForwardLightsBindingSet = m_Device->createBindingSet(bindingSetDesc, m_ForwardLightsBindingLayout);


        /*auto samplerDesc = SamplerDesc()
            .setAllAddressModes(SamplerAddressMode::Border)
            .setBorderColor(1.0f);
        m_ShadowSampler = m_Device->createSampler(samplerDesc);

        m_ForwardViewCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(ForwardShadingViewConstants), "ForwardShadingViewConstants"));

        m_ObjectTransformCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(TransformConstants), "TransformConstants"));*/


        m_ShaderParameters[ShaderType::Pixel].bindingLayouts.push_back(m_ViewBindinglayout);
        m_ShaderParameters[ShaderType::Pixel].bindingSets.push_back(m_ViewBindingset);

        m_ShaderParameters[ShaderType::Pixel].bindingLayouts.push_back(m_ForwardLightsBindingLayout);
        m_ShaderParameters[ShaderType::Pixel].bindingSets.push_back(m_ForwardLightsBindingSet);


        m_ShaderParameters->VertexShader = m_ForwardShadingShader->GetVertexShader();
        m_ShaderParameters->PixelShader = m_ForwardShadingShader->GetPixelShader();

        //m_ShaderParameters[ShaderType::VertexShader].inputLayout = inputLayout;
    }

    void ForwardShadingRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
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

        ForwardShadingLightConstants lightConstants{};
        FillShaderParameters(scene->GetLightSources(), lightConstants);
        commandList->writeBuffer(m_LightsCB, &lightConstants, sizeof(ForwardShadingLightConstants));

        RenderPassTemplate(commandList, targetFramebuffer, view, m_DrawStrategy, m_ShaderParameters);

        
        commandList->endMarker();
    }

    void ForwardShadingRenderer::FillShaderParameters(LightSources* lightSource, ForwardShadingLightConstants& output)
    {
        if (lightSource->GetLightsNum() > FORWARD_MAX_LIGHTS) {
            GE_CORE_ERROR("light number %d exceed limit %d", lightSource->GetLightsNum(), FORWARD_MAX_LIGHTS);
        }
        output.numLights = lightSource->GetLightsNum();
        for (size_t i = 0; i < math::min(FORWARD_MAX_LIGHTS, (int)lightSource->GetLightsNum()); i++)
        {
            LightConstants lightConst;
            Light* light = lightSource->GetLights()[i];
            light->FillLightConstants(lightConst);
            output.lights[i] = lightConst;
        }
    }

    void ForwardShadingRenderer::Render(Object* obj)
    {
        //		m_PbrShader->Bind();
        //		DrawObject(obj, m_PbrShader);
    }

    ForwardShadingRenderer::~ForwardShadingRenderer()
    {
    }

}
