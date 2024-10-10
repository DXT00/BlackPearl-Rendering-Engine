#include "pch.h"
#include "RayTracingRenderGraph.h"
#include "BlackPearl/Renderer/CullingManager.h"
#include "BlackPearl/FileSystem/FileSystem.h"
#include "BlackPearl/Renderer/MasterRenderer/BasePassRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/RTXDI/RtxdiRenderer.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"
#include "BlackPearl/UI/UIManager.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
namespace BlackPearl {
    extern UIManager* g_uiManager;
    extern RootFileSystem* g_rootFileSystem;

    void RayTracingRenderGraph::Init(Scene* scene)
    {
        m_CommandList = GetDevice()->createCommandList();
        m_ShaderFactory = std::make_shared<ShaderFactory>(m_DeviceManager->GetDevice(), g_rootFileSystem, "assets/shaders/spv");
        m_CommonPasses = std::make_shared<CommonRenderPasses>(GetDevice(), m_ShaderFactory);
        m_Scene = scene;

        //create render pass
        m_RtxdiPass = std::make_unique<RtxdiRenderer>(GetDevice(), m_ShaderFactory, m_CommonPasses, m_BindlessLayout);


    }
    void RayTracingRenderGraph::Render(IFramebuffer* framebuffer, IView* View)
    {
        BuildOpacityMicromaps(m_CommandList, GetFrameIndex());
        BuildTLAS(m_CommandList, GetFrameIndex());
        PathTrace(framebuffer);
        Denoise(framebuffer);
        PostProcessAA(framebuffer);
    }

    void RayTracingRenderGraph::BuildOpacityMicromaps(ICommandList* commandList, uint32_t frameIndex)
    {
    }

    void RayTracingRenderGraph::BuildTLAS(ICommandList* commandList, uint32_t frameIndex) const
    {
        //commandList->beginMarker("Skinned BLAS Updates");

        //// Transition all the buffers to their necessary states before building the BLAS'es to allow BLAS batching
        //for (const auto& skinnedInstance : m_Scene->GetSceneGraph()->GetSkinnedMeshInstances())
        //{
        //    if (skinnedInstance->GetLastUpdateFrameIndex() < frameIndex)
        //        continue;

        //    commandList->setAccelStructState(skinnedInstance->GetMesh()->accelStruct, ResourceStates::AccelStructWrite);
        //    commandList->setBufferState(skinnedInstance->GetMesh()->buffers->vertexBuffer, ResourceStates::AccelStructBuildInput);
        //}
        //commandList->commitBarriers();

        //// Now build the BLAS'es
        //for (const auto& skinnedInstance : m_Scene->GetSceneGraph()->GetSkinnedMeshInstances())
        //{
        //    if (skinnedInstance->GetLastUpdateFrameIndex() < frameIndex)
        //        continue;

        //    bvh::Config cfg = { .excludeTransmissive = m_ui.AS.ExcludeTransmissive };

        //    rt::AccelStructDesc blasDesc = bvh::GetMeshBlasDesc(cfg, *skinnedInstance->GetMesh(), nullptr);

        //    RHIUtils::BuildBottomLevelAccelStruct(commandList, skinnedInstance->GetMesh()->accelStruct, blasDesc);
        //}
        //commandList->endMarker();

        std::vector<rt::InstanceDesc> instances; // TODO: make this a member, avoid allocs :)

        uint subInstanceCount = 0;
        //for (const auto& instance : m_Scene->GetObjects())
        //{
        //    const bool ommDebugViewEnabled = false;// m_ui.DebugView == DebugViewType::FirstHitOpacityMicroMapInWorld || m_ui.DebugView == DebugViewType::FirstHitOpacityMicroMapOverlay;
        //    // ommDebugViewEnabled must do two things: use a BLAS without OMMs and disable all alpha testing.
        //    // This may sound a bit counter intuitive, the goal is to intersect micro-triangles marked as transparent without them actually being treated as such.

        //    const bool forceOpaque = ommDebugViewEnabled || g_uiManager->m_UIData.AS.ForceOpaque;
        //    const bool hasAttachementOMM = instance->GetComponent<MeshRenderer>()->GetMeshes()->accelStructOMM.Get() != nullptr;
        //    const bool useOmmBLAS = g_uiManager->m_UIData.OpacityMicroMaps.Enable && hasAttachementOMM && !forceOpaque;

        //    rt::InstanceDesc instanceDesc;
        //    instanceDesc.bottomLevelAS = useOmmBLAS ? instance->GetMesh()->accelStructOMM.Get() : instance->GetMesh()->accelStruct.Get();
        //    instanceDesc.instanceMask = m_ui.OpacityMicroMaps.OnlyOMMs && !hasAttachementOMM ? 0 : 1;
        //    instanceDesc.instanceID = instance->GetGeometryInstanceIndex();
        //    instanceDesc.instanceContributionToHitGroupIndex = subInstanceCount;
        //    instanceDesc.flags = m_ui.OpacityMicroMaps.Force2State ? rt::InstanceFlags::ForceOMM2State : rt::InstanceFlags::None;
        //    if (forceOpaque)
        //        instanceDesc.flags = (rt::InstanceFlags)((uint32_t)instanceDesc.flags | (uint32_t)rt::InstanceFlags::ForceOpaque);

        //    assert(subInstanceCount == instance->GetGeometryInstanceIndex());
        //    subInstanceCount += (uint)instance->GetMesh()->geometries.size();

        //    auto node = instance->GetNode();
        //    assert(node);
        //    affineToColumnMajor(node->GetLocalToWorldTransformFloat(), instanceDesc.transform);

        //    instances.push_back(instanceDesc);
        //}
        //assert(m_SubInstanceCount == subInstanceCount);

        //// Compact acceleration structures that are tagged for compaction and have finished executing the original build
        //commandList->compactBottomLevelAccelStructs();

        //commandList->beginMarker("TLAS Update");
        //commandList->buildTopLevelAccelStruct(m_TopLevelAS, instances.data(), instances.size(), rt::AccelStructBuildFlags::AllowEmptyInstances);
        //commandList->endMarker();
    }

    void RayTracingRenderGraph::PathTrace(IFramebuffer* framebuffer)
    {
    }

    void RayTracingRenderGraph::Denoise(IFramebuffer* framebuffer)
    {
    }

    void RayTracingRenderGraph::PostProcessAA(IFramebuffer* framebuffer)
    {
    }
    
}
