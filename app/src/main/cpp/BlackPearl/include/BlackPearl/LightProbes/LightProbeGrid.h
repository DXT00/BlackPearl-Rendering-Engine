#pragma once
#include "Math/vector.h"
#include "LightProbeCommon.h"

namespace BlackPearl {

    class Object;
	class LightProbeGrid {

	public:
		
        LightProbeGrid(ProbeType type, math::float3 probeNums, math::float3 offsets, float space);
        LightProbeGrid(ProbeType type, float extendX, float extendY, float extendZ, math::float3 offsets, float space);

        float Width;
        float Height;
        float Depth;
        float ProbeDistance;
        ProbeType ProbeType;
        math::float3 ProbeCounts;
        Object* GridObj = nullptr;

    private:
        void _CreateGridObj(math::float3 offsets);


	};
}