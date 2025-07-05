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
            
           
            
            std::vector<Object*>& objs = m_DrawStrategy->GetDrawObjects();// scene->GetObjects();
			for (size_t i = 0; i < objs.size(); i++)
			{
				DFObjectConstants objCnonst;
				objCnonst.extend = objs[i]->GetComponent<BoundingBox>()->Get().GetExtent();
				objCnonst.center = objs[i]->GetComponent<BoundingBox>()->Get().GetCenter();
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
            cmdList->dispatch(SystemTexture::Get().SceneGlobalDF.ClipDim / 4, SystemTexture::Get().SceneGlobalDF.ClipDim / 4, SystemTexture::Get().SceneGlobalDF.ClipDim / 4);


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

    void GlobalDFRenderer::VisualizeGDF()
    {
    }
}