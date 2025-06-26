#pragma once
#include "Object/Object.h"
#include "Math/vector.h"
#include "Map/MapManager.h"
#include "Renderer/MasterRenderer/GI/ProbeRenderer.h"
namespace BlackPearl {

    enum ProbeGenerateMethod {
        DDGI, //hardware raytracing or sdf sw tracing
        RTXDI,  //hardware raytracing
        IBL,    // world space SH
        SSGI

    };
	class LightProbeManager {

	public:
		LightProbeManager* GetInstance();
		std::vector<Object*> FindKnearProbes(math::float3 objPos, std::vector<Object*> probes, unsigned int k);
		std::vector<unsigned int> FindKnearAreaProbes(math::float3 objPos, std::vector<Object*> probes, unsigned int k, MapManager* mapManager);
	

        static ProbeRenderer* CreateProbeRenderer(IDevice* device, ProbeGenerateMethod method);
    
    private:
		LightProbeManager();
	};
}