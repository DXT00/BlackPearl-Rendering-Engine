#include "pch.h"
#include "Renderer/MasterRenderer/GlobalDFRenderer.h"
#include "hlsl/core/sdf_cb.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"
#include "hlsl/core/sdf_cb.h"
#include "BlackPearl/Component/BoundingBoxComponent/BoundingBox.h"
#include "Timestep/TimeCounter.h"
#include "Renderer/SystemTextures.h"
#ifdef GE_API_OPENGL
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
#endif
#include "Application.h"
namespace BlackPearl {


	GlobalDFRenderer::GlobalDFRenderer(IDevice* device)
        : BasicRenderer(device)
    {
    }

    //todo::  这里还要做 相机范围内的物体裁剪，根据 mesh df 更新， 中心是相机中心， 实时更新
    //https://dev.epicgames.com/documentation/zh-cn/unreal-engine/mesh-distance-fields-in-unreal-engine
    void GlobalDFRenderer::Init(Scene* scene) {
        m_GDFBakeShader = DBG_NEW MaterialShader("assets/shaders/glsl/sdf/sdfBake.glsl");
        SystemTexture::Get().SceneGlobalDF.Init(scene);
        m_GDFCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(GlobalSDFConstants), "GlobalSDFConstants"));
        m_DrawStrategy = DBG_NEW InstancedOpaqueDrawStrategy();

        BufferDesc sceneObjsDesc;
        sceneObjsDesc.byteSize = sizeof(DFObjectConstants) * scene->GetObjects().size();
        sceneObjsDesc.structStride = sizeof(DFObjectConstants);
        sceneObjsDesc.initialState = ResourceStates::UnorderedAccess;
        sceneObjsDesc.keepInitialState = true;
		sceneObjsDesc.canHaveUAVs = true;
        sceneObjsDesc.debugName = "SceneObjsInfoBuffer";
        m_SceneObjectsCB = m_Device->createBuffer(sceneObjsDesc);

  
        //GDF Material
        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(8),     // GlobalSDFConstants
            RHIBindingLayoutItem::RT_StructuredBuffer_UAV(9),		// DFObjectConstants
            RHIBindingLayoutItem::RT_Texture_UAV(0),

        };
        m_GDFBindingLayout = m_Device->createBindingLayout(layoutDesc);

        for (int i = 0; i < SystemTexture::Get().SceneGlobalDF.NumClipMapLevels; i++) {

            auto& level = SystemTexture::Get().SceneGlobalDF.Clipmaps[i];

            BindingSetDesc bindingSetDesc;
            bindingSetDesc.bindings = {
                BindingSetItem::ConstantBuffer(8, m_GDFCB),
                BindingSetItem::StructuredBuffer_SRV(9, m_SceneObjectsCB),
                BindingSetItem::Texture_UAV(0, SystemTexture::Get().SceneGlobalDF.Clipmaps[i].MipTexture, "MipTexture" + std::to_string(i))
            };
            m_GDFBindingSets.push_back(m_Device->createBindingSet(bindingSetDesc, m_GDFBindingLayout));

        }


        _InitGDFDebug();
    }


    void GlobalDFRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) {
        SCOPE_TIME_COUNTER(GlobalDF);
        cmdList->beginMarker("GlobalDF");
		SceneData* view = Renderer::GetSceneData();

        SystemTexture::Get().SceneGlobalDF.Update(Math::ToFloat3(view->CameraPosition));

		for (int i = 0; i < SystemTexture::Get().SceneGlobalDF.NumClipMapLevels; i++) {

			auto& level = SystemTexture::Get().SceneGlobalDF.Clipmaps[i];


            

			std::vector<DFObjectConstants> objsConstants;
			
            
            SceneData* view = Renderer::GetSceneData();
            GE_ERROR_JUDGE();

            SceneData* preView = Renderer::GetPreSceneData();
            GE_ERROR_JUDGE();

            SetupView(cmdList, view, preView);
            //获取 cpu 裁剪后的objects
            m_DrawStrategy->PrepareForView(scene, *view);
            
           
            //TODO:: Objects Culling!
            std::vector<Object*>& objs = scene->GetObjects();//  m_DrawStrategy->GetDrawObjects();// scene->GetObjects();
			for (size_t j = 0; j < objs.size(); j++)
			{
				DFObjectConstants objCnonst;
				objCnonst.extend = objs[j]->GetComponent<BoundingBox>()->Get().GetExtent();
				objCnonst.center = objs[j]->GetComponent<BoundingBox>()->Get().GetCenter();
				objsConstants.push_back(objCnonst);
			}
			int debug = sizeof(objsConstants);
			cmdList->writeBuffer(m_SceneObjectsCB, objsConstants.data(), sizeof(DFObjectConstants)* objsConstants.size());



            GlobalSDFConstants gdfConstants{};
            gdfConstants.voxelSize = std::pow(2, i);
            gdfConstants.clipmapCenter = SystemTexture::Get().SceneGlobalDF.Clipmaps[i].Center;
            gdfConstants.clipmapDimension = SystemTexture::Get().SceneGlobalDF.ClipDim;
            gdfConstants.objsCnt = m_DrawStrategy->GetDrawObjects().size();
            cmdList->writeBuffer(m_GDFCB, &gdfConstants, sizeof(GlobalSDFConstants));

            ComputePipelineDesc psoDesc;
       
            psoDesc.bindingLayouts.push_back(m_GDFBindingLayout);
            psoDesc.CS = m_GDFBakeShader->GetComputeShader();

            if (!m_GDFPso) {
                m_GDFPso = m_Device->createComputePipeline(psoDesc);
            }

            ComputeState computePSO;
            computePSO.pipeline = m_GDFPso;
            computePSO.bindings.push_back(m_GDFBindingSets[i]);


            cmdList->setComputeState(computePSO);
            cmdList->dispatch(SystemTexture::Get().SceneGlobalDF.ClipDim / 8, SystemTexture::Get().SceneGlobalDF.ClipDim / 8, SystemTexture::Get().SceneGlobalDF.ClipDim / 8);


			//// 绑定当前 Clipmap 的 3D 纹理为存储目标
			//glBindImageTexture(0, level.Texture3D, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);

			//// 传递参数到 Compute Shader
			//glUniform1f(glGetUniformLocation(ComputeShader, "u_VoxelSize"), level.VoxelSize);
			//glUniform3fv(glGetUniformLocation(ComputeShader, "u_ClipmapCenter"), 1, &level.Center[0]);

			//// 分派 Compute Shader（64x64x64 / 8x8x8 = 8x8x8 工作组）
			//glDispatchCompute(8, 8, 8);
			//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}
    
        SystemTexture::Get().SceneGlobalDF.Valid = true;
        cmdList->endMarker();


    }

    void GlobalDFRenderer::FillShaderParameters() {

    }

    void GlobalDFRenderer::ShowGDF(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {


        SceneData* view = Renderer::GetSceneData();
        GE_ERROR_JUDGE();

        SceneData* preView = Renderer::GetPreSceneData();
        GE_ERROR_JUDGE();

        SetupView(cmdList, view, preView);


        DrawItem drawItem = IDrawStrategy::ObjectToDrawItem(scene->GetFullScreenObj())[0];

        GraphicsState graphicsPSO;
        graphicsPSO.framebuffer = targetFramebuffer;
        graphicsPSO.viewport = view->GetViewportState();
        graphicsPSO.shadingRateState = view->GetVariableRateShadingState();

        GraphicsPipelineDesc psoDesc;

        //psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
        psoDesc.depthStencilState.disableDepthTest();
        psoDesc.depthStencilState.disableDepthWrite();
        psoDesc.depthStencilState.disableStencil();

        psoDesc.blendState.alphaToCoverageEnable = false;
        psoDesc.rasterState.frontCounterClockwise = true;
        psoDesc.rasterState.cullMode = RasterCullMode::None;
        psoDesc.primType = PrimitiveType::TriangleList;
        psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

        psoDesc.VS = m_GDFDebugShader->GetVertexShader();
        psoDesc.PS = m_GDFDebugShader->GetPixelShader();
        psoDesc.bFromPSOFileCache = false;
        psoDesc.bindingLayouts.push_back(m_GDFDebugBinding.layout);
        psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);
      


        if (!m_GDFDebugPso) {
            m_GDFDebugPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
        }
        graphicsPSO.pipeline = m_GDFDebugPso;
       
        graphicsPSO.bindings.push_back(m_GDFDebugBinding.set);
        graphicsPSO.bindings.push_back(m_ViewBindingset);
       
        graphicsPSO.inputLayout = psoDesc.inputLayout;
        /*for (int j = 0; j < shaderParms[ShaderType::Pixel].bindingLayouts.size(); ++j) {
            psoDesc.bindingLayouts.push_back(shaderParms[ShaderType::Pixel].bindingLayouts[j]);
        }

        for (int j = 0; j < shaderParms[ShaderType::Pixel].bindingSets.size(); ++j) {
            graphicsPSO.bindings.push_back(shaderParms[ShaderType::Pixel].bindingSets[j]);
        }*/

        /*GE_ERROR_JUDGE();
        SetupMaterial(drawItem.material, drawItem.cullMode, psoDesc, graphicsPSO);*/
        SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
        GE_ERROR_JUDGE();

        SystemTexture& systemTex = SystemTexture::Get();

        GlobalSDFTraceConstants gdfTraceConst{};
        GE_ASSERT(systemTex.SceneGlobalDF.NumClipMapLevels <= MAX_CLIP_NUM);
        for (int i = 0; i < MAX_CLIP_NUM; i++) {
            gdfTraceConst.clipPosDistance[i] = math::float4(0.0);

        }
        gdfTraceConst.clipVoxelSize = math::float4(0.0);

        GE_ASSERT(systemTex.SceneGlobalDF.NumClipMapLevels == 3, "Invalid clipmap ,should fix sdfVisualization shader");
        for (int i = 0; i < systemTex.SceneGlobalDF.NumClipMapLevels; i++) {
            gdfTraceConst.clipPosDistance[i] = math::float4(systemTex.SceneGlobalDF.Clipmaps[i].Center, systemTex.SceneGlobalDF.Clipmaps[i].Extent * 0.5f);
            gdfTraceConst.clipVoxelSize[i] = systemTex.SceneGlobalDF.Clipmaps[i].VoxelSize;
        }

        gdfTraceConst.dimension = systemTex.SceneGlobalDF.ClipDim;
        gdfTraceConst.clipCount = systemTex.SceneGlobalDF.NumClipMapLevels;

        cmdList->writeBuffer(m_GDFDebugBinding.gdfCb, &gdfTraceConst, sizeof(GlobalSDFTraceConstants));

        cmdList->setGraphicsState(graphicsPSO);

        Draw(cmdList, drawItem);


        //GraphicsPipelineDesc psoDesc;

        //psoDesc.bindingLayouts.push_back(m_GDFDebugBinding.layout);
        //psoDesc.CS = m_GDFDebugShader->GetComputeShader();


        //ComputeState computePSO;

        //if (!m_GDFDebugPso)
        //    m_GDFDebugPso = m_Device->createGraphicsPipeline(psoDesc);

        //computePSO.pipeline = m_GDFDebugPso;



        //GlobalSDFConstants gdfConstants{};
        //gdfConstants.voxelSize = std::pow(2, 0);
        //gdfConstants.clipmapCenter = SystemTexture::Get().SceneGlobalDF.Clipmaps[0].Center;
        //gdfConstants.clipmapDimension = SystemTexture::Get().SceneGlobalDF.ClipDim;
        //gdfConstants.objsCnt = m_DrawStrategy->GetDrawObjects().size();
        //cmdList->writeBuffer(m_GDFDebugBinding.gdfCb, &gdfConstants, sizeof(GlobalSDFConstants));

        //computePSO.bindings.push_back(m_GDFDebugBinding.set);
        //cmdList->setComputeState(computePSO);
        //cmdList->dispatch(SystemTexture::Get().SceneGlobalDF.ClipDim / 8, SystemTexture::Get().SceneGlobalDF.ClipDim / 8, SystemTexture::Get().SceneGlobalDF.ClipDim / 8);



   

    }
    void GlobalDFRenderer::_InitGDFDebug()
    {
        m_GDFDebugShader = DBG_NEW MaterialShader("assets/shaders/glsl/sdf/sdfVisualization.glsl");
        m_GDFDebugBinding.gdfCb = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(GlobalSDFTraceConstants), "GlobalSDFTraceConstants"));


        TextureDesc outputDesc;
        outputDesc.width = Application::Get().GetWindow().GetCurWindowSize().x;
        outputDesc.height = Application::Get().GetWindow().GetCurWindowSize().y;
        outputDesc.mipLevelsCnt = 1;
        outputDesc.format = Format::R16_FLOAT;
        outputDesc.access = TextureAccess::WriteOnly;
        //m_GDFDebugBinding.debugOutput = m_Device->createTexture(outputDesc);


        //GDF Material
        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Compute;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(8),     // GlobalSDFTraceConstants
            RHIBindingLayoutItem::RT_Texture_UAV(0),		
            RHIBindingLayoutItem::RT_Texture_UAV(1),
            RHIBindingLayoutItem::RT_Texture_UAV(2),

        };
        m_GDFDebugBinding.layout = m_Device->createBindingLayout(layoutDesc);

       

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::ConstantBuffer(8, m_GDFDebugBinding.gdfCb),
            BindingSetItem::Texture_UAV(0, SystemTexture::Get().SceneGlobalDF.Clipmaps[0].MipTexture),
            BindingSetItem::Texture_UAV(1, SystemTexture::Get().SceneGlobalDF.Clipmaps[1].MipTexture),
            BindingSetItem::Texture_UAV(2, SystemTexture::Get().SceneGlobalDF.Clipmaps[2].MipTexture)
        };
        m_GDFDebugBinding.set = (m_Device->createBindingSet(bindingSetDesc, m_GDFDebugBinding.layout));

        
    }
}