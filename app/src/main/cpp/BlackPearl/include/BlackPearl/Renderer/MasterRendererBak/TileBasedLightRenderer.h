#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/MainCamera/MainCamera.h"
#include "BlackPearl/Config.h"
namespace BlackPearl {

	struct ClasterInfo
	{
        ClasterInfo(MainCamera* camera, int blockSizeX, int blockSizeY, int sliceNumZ, float distanceFar, float distanceNear) {
        
            FOV = camera->Fov() * 1.0 / 180.0f * 0.5f;
            zNearVS = camera->ZNear();
            zFarVS = camera->ZFar();
            clusterNumX = (int)(Configuration::WindowWidth / (float)blockSizeX);
            clusterNumY = (int)(Configuration::WindowHeight / (float)blockSizeY);
            zDistanceNearVS = distanceNear;
            zDistanceFarVS = distanceFar;


            logDepth = log(zDistanceFarVS / zDistanceNearVS);
            clusterNumZ = sliceNumZ;
            logDimY = logDepth / clusterNumZ;
            sD = exp(logDimY);
            clusterNumXYZ = clusterNumX * clusterNumY * clusterNumZ;
        }

        float FOV;
        float zNearVS;
        float zFarVS;
        float zDistanceNearVS;
        float zDistanceFarVS;
        float sD;
        float logDimY;
        float logDepth;
        int clusterNumX;
        int clusterNumY;
        int clusterNumZ;
        int clusterNumXYZ;
        int maxLightPerCluster;
	};



	class TileBasedLightRenderer : public BasicRenderer
	{
    public:
        TileBasedLightRenderer();

        ~TileBasedLightRenderer();
        void Init(Scene* scene, const ClasterInfo& clusterInfo);

        void Render(std::shared_ptr<Shader> indirectShader, MainCamera* cam);
    private:
        std::shared_ptr<ShaderStorageBuffer> mLightIndexList;
        std::shared_ptr<ShaderStorageBuffer> mLightIndexListCounter;
        std::shared_ptr<ShaderStorageBuffer> mClusterGrid;
        std::shared_ptr<ShaderStorageBuffer> allLightInfo;
        std::shared_ptr<ShaderStorageBuffer> mAABBs;


	};

}