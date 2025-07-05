#include "pch.h"
#include "Object/Object.h"
#include "LightProbes/LightProbeGrid.h"
#include "ObjectManager/ObjectManager.h"
#include "Component/BoundingBoxComponent/BoundingBox.h"
#include "Map/Mapmanager.h"
namespace BlackPearl {
	
    extern ObjectManager* g_objectManager;
    extern MapManager* g_mapManager;

	LightProbeGrid::LightProbeGrid(BlackPearl::ProbeType type, math::float3 probeNums, math::float3 offsets, float space)
	{
        ProbeType = type;
        ProbeCounts = probeNums;
        Width  = (probeNums.x - 1) * space;
        Height = (probeNums.y - 1) * space;
        Depth  = (probeNums.z - 1) * space;

        ProbeDistance = space;

        _CreateGridObj(offsets);



	}

    LightProbeGrid::LightProbeGrid(BlackPearl::ProbeType type, float extendX, float extendY, float extendZ, math::float3 offsets, float space)
    {

        ProbeType = type;
        ProbeCounts = math::float3((int)(extendX / space) +1, (int)(extendY / space) + 1, (int)(extendZ / space) + 1);
        Width = extendX;
        Height = extendY;
        Depth = extendZ;

        ProbeDistance = space;
        _CreateGridObj(offsets);


    }


    /*
    
     (0,0,1)

   (0,0,0)
    
    
    */
    void LightProbeGrid::_CreateGridObj(math::float3 offsets)
    {
        std::string objName = (ProbeType == ProbeType::DIFFUSE_PROBE) ? "Kd ProbesGrid" : "Ks ProbeGrid";
        GridObj = g_objectManager->CreateEmpty(objName);
        unsigned int idx = 0;
        for (unsigned int z = 0; z < ProbeCounts.z; z++)
        {
            for (unsigned int y = 0; y < ProbeCounts.y; y++)
            {
                for (unsigned int x = 0; x < ProbeCounts.z; x++)
                {
                    Object* probe = g_objectManager->CreateLightProbe(ProbeType, "assets/shaders/glsl/lightProbes/lightProbe.glsl", "","LightProbe");
                  //  int xx = (x - ProbeCounts.x / 2) * ProbeDistance, yy = (y - ProbeCounts.y / 2) * ProbeDistance, zz = (z - ProbeCounts.z / 2) * ProbeDistance;
                    int xx = (x) * ProbeDistance, yy = (y) * ProbeDistance, zz = (z) * ProbeDistance;

                    glm::vec3 probePos = { offsets.x + xx,offsets.y + yy,offsets.z + zz };
                    probe->GetComponent<Transform>()->SetInitPosition(probePos);
                    if (ProbeType == ProbeType::DIFFUSE_PROBE) {
                        uint32_t areaId = g_mapManager->AddProbeIdToArea(probePos, idx);
                        probe->GetComponent<LightProbe>()->SetAreaId(areaId);
                    }
                    idx++;
                    GridObj->AddChildObj(probe);

                }

            }
        }
        math::float3 pos = math::float3(
            (ProbeCounts.x - 1) * ProbeDistance,
            (ProbeCounts.y - 1) * ProbeDistance,
            (ProbeCounts.z - 1) * ProbeDistance);
        GridObj->GetComponent<Transform>()->SetPosition(Math::ToVec3(pos));

        math::float3 extend = (ProbeCounts - math::float3(1.0)) * ProbeDistance;
        AABB box(pos, extend);
        GridObj->AddComponent<BoundingBox>(box);



    }

}