#pragma once
#include <cassert>
#include "BlackPearl/Math/Math.h"
#include "Octree.h"
#include "BlackPearl/AABB/AABB.h"
namespace BlackPearl {

	class PrimitiveOctreeNode {
		AABB Bound;
		uint32_t ObjectId;
	};
	struct PrimitiveOctreeSemantics
	{
		/** Note: this is coupled to shadow gather task granularity, see r.ParallelGatherShadowPrimitives. */
		enum { MaxElementsPerLeaf = 256 };
		enum { MinInclusiveElementsPerNode = 7 };
		enum { MaxNodeDepth = 12 };
	};


	/** The type of the octree used by FScene to find primitives. */
	typedef Octree<PrimitiveOctreeNode, struct PrimitiveOctreeSemantics> ScenePrimitiveOctree;

}
