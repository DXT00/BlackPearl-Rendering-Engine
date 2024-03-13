#include "pch.h"
#include "TileBasedLightRenderer.h"
namespace BlackPearl{


    TileBasedLightRenderer::~TileBasedLightRenderer()
    {
    }

    void ClusterAABB(const ClasterInfo& clusterInfo)
    {
     
        /* int stride = Marshal.SizeOf(typeof(AABB));
        myPipelineCamera.AABBBuffer = new ComputeBuffer(myPipelineCamera.clusterInfo.clusterNumXYZ, stride);
        myPipelineCamera.t_LightIndexList = new ComputeBuffer(myPipelineCamera.clusterInfo.clusterNumXYZ * myPipelineCamera.assumeMaxLightPerCluster, Marshal.SizeOf(typeof(uint)), ComputeBufferType.Counter);
        myPipelineCamera.ot_LightIndexListCounter = new ComputeBuffer(1, Marshal.SizeOf(typeof(uint)));
        myPipelineCamera.clusterGrid = new ComputeBuffer(myPipelineCamera.clusterInfo.clusterNumXYZ, 2 * Marshal.SizeOf(typeof(uint)));
        int kernel = ClusterAABBCS.FindKernel("ClusterAABBCore");
        commandBuffer.SetComputeBufferParam(ClusterAABBCS, kernel, "RWClusterAABBs", myPipelineCamera.AABBBuffer);
        commandBuffer.SetComputeFloatParam(ClusterAABBCS, "zNearVS", myPipelineCamera.clusterInfo.zNearVS);
        commandBuffer.SetComputeFloatParam(ClusterAABBCS, "ClusterCB_ViewNear", myPipelineCamera.clusterInfo.zDistanceNearVS);
        commandBuffer.SetComputeFloatParam(ClusterAABBCS, "ClusterCB_NearK", myPipelineCamera.clusterInfo.sD);
        commandBuffer.SetComputeIntParams(ClusterAABBCS, "ClusterCB_GridDim", new int[3]{ myPipelineCamera.clusterInfo.clusterNumX, myPipelineCamera.clusterInfo.clusterNumY, myPipelineCamera.clusterInfo.clusterNumZ });
        commandBuffer.SetComputeIntParams(ClusterAABBCS, "ClusterCB_Size", new int[2]{ LIGHTCULLBLOCKX, LIGHTCULLBLOCKY });
        commandBuffer.SetComputeVectorParam(ClusterAABBCS, "ClusterCB_ScreenDimensions", new Vector4((float)camera.pixelWidth, (float)camera.pixelHeight, 1.0f / (float)camera.pixelWidth, 1.0f / (float)camera.pixelHeight));
        commandBuffer.SetComputeMatrixParam(ClusterAABBCS, "inverseProjection", GL.GetGPUProjectionMatrix(camera.projectionMatrix, false).inverse);
        int threadGroup = Mathf.CeilToInt((float)myPipelineCamera.clusterInfo.clusterNumXYZ / 1024.0f);

        commandBuffer.DispatchCompute(ClusterAABBCS, kernel, threadGroup, 1, 1);
        context.ExecuteCommandBuffer(commandBuffer);
        commandBuffer.Clear();*/
    }
    void TileBasedLightRenderer::Init(Scene* scene, const ClasterInfo& clusterInfo)
    {
        mAABBs = std::make_shared<ShaderStorageBuffer>(clusterInfo.clusterNumXYZ * sizeof(AABB), GL_STREAM_DRAW, nullptr);
        mLightIndexList = std::make_shared<ShaderStorageBuffer>(clusterInfo.clusterNumXYZ * clusterInfo.maxLightPerCluster * sizeof(int), GL_STREAM_DRAW, nullptr);
        mLightIndexListCounter = std::make_shared<ShaderStorageBuffer>(1 * sizeof(int), GL_STREAM_DRAW, nullptr);
        mClusterGrid = std::make_shared<ShaderStorageBuffer>(clusterInfo.clusterNumXYZ * 2 * sizeof(int), GL_STREAM_DRAW, nullptr);
    }
    void TileBasedLightRenderer::Render(std::shared_ptr<Shader> indirectShader, MainCamera* cam)
    {
    }
}
