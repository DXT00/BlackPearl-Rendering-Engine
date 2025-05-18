#pragma once
#include "Object/Object.h"
#include "Math/vector.h"
#include "Map/MapManager.h"
namespace BlackPearl {

	class LightProbeManager {

	public:
		LightProbeManager* GetInstance();
		std::vector<Object*> FindKnearProbes(math::float3 objPos, std::vector<Object*> probes, unsigned int k);
		std::vector<unsigned int> FindKnearAreaProbes(math::float3 objPos, std::vector<Object*> probes, unsigned int k, MapManager* mapManager);
	private:
		LightProbeManager();
	};
}