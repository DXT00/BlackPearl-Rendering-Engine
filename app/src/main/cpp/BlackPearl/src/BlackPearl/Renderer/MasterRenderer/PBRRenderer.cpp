#include "pch.h"
#include "PBRRenderer.h"

namespace BlackPearl {


	void PBRRenderer::Render(const std::vector<Object*>& objs)
	{
//		m_PbrShader->Bind();
//		DrawObjects(objs, m_PbrShader);
	}
    void PBRRenderer::Init()
    {
        std::vector<ShaderMacro> Macros;
        // Macros.push_back(ShaderMacro("COMPILE_SHADER", "1"));
        ShaderHandle vertexShader = g_shaderFactory->CreateShader("hlsl/test/forward_test_vs.hlsl", "main", &Macros, ShaderType::Vertex);
        ShaderHandle pixelShader = g_shaderFactory->CreateShader("hlsl/test/forward_test_vs.hlsl", "main", &Macros, ShaderType::Pixel);

        std::vector<VertexBufferBinding> vertexBuffers;
        IndexBufferBinding indexBuffer;
        const VertexAttributeDesc inputDescs[] =
                {
                        GetVertexAttributeDesc(VertexAttribute::Position, "POS", 0),
                        GetVertexAttributeDesc(VertexAttribute::PrevPosition, "PREV_POS", 1),
                        GetVertexAttributeDesc(VertexAttribute::TexCoord1, "TEXCOORD", 2),
                        GetVertexAttributeDesc(VertexAttribute::Normal, "NORMAL", 3),
                        //TODO::
                        // GetVertexAttributeDesc(VertexAttribute::Tangent, "TANGENT", 4),
                        GetVertexAttributeDesc(VertexAttribute::Transform, "TRANSFORM", 4),
                };

        InputLayoutHandle inputLayout = m_Device->createInputLayout(inputDescs, uint32_t(std::size(inputDescs)));


        RHIBindingLayoutDesc viewLayoutDesc;
        viewLayoutDesc.visibility = ShaderType::All;
        viewLayoutDesc.bindings = {
                RHIBindingLayoutItem::RT_VolatileConstantBuffer(0),
                RHIBindingLayoutItem::RT_Sampler(1)
        };

        BindingSetDesc viewBindingSetDesc;
        viewBindingSetDesc.bindings = {
                BindingSetItem::ConstantBuffer(0, m_ForwardViewCB),
                BindingSetItem::Sampler(1, m_ShadowSampler)
        };
        BindingLayoutHandle viewBindinglayout = m_Device->createBindingLayout(viewLayoutDesc);
        BindingSetHandle viewBindingset = m_Device->createBindingSet(viewBindingSetDesc, viewBindinglayout);

        m_ShaderParameters.bindingLayouts.push_back(viewBindinglayout);
        m_ShaderParameters.bindingSets.push_back(viewBindingset);
    }

	void PBRRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
	{
		commandList->beginMarker("BasePass");
		SceneData* view = Renderer::GetSceneData();
		SceneData* preView = Renderer::GetPreSceneData();

		m_DrawStrategy->PrepareForView(scene, *view);
		RenderPassTemplate(commandList, targetFramebuffer, view, m_DrawStrategy, m_ShaderParameters);

		commandList->endMarker();
	}

	void PBRRenderer::Render(Object * obj)
	{
//		m_PbrShader->Bind();
//		DrawObject(obj, m_PbrShader);
	}

	PBRRenderer::~PBRRenderer()
	{
	}

}
