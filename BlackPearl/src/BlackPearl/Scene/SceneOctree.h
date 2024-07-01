#pragma once
#include <cassert>
#include "BlackPearl/Math/Math.h"
#include "Octree.h"
#include "BlackPearl/AABB/AABB.h"
namespace BlackPearl {

	class PrimitiveOctreeNode {
	public:
		AABB Bound;
		uint32_t ObjectId;
	};
	struct PrimitiveOctreeSemantics
	{
		/** Note: this is coupled to shadow gather task granularity, see r.ParallelGatherShadowPrimitives. */
		enum { MaxElementsPerLeaf = 256 };
		enum { MinInclusiveElementsPerNode = 7 };
		enum { MaxNodeDepth = 12 };

		static AABB GetBoundingBox(PrimitiveOctreeNode element) {
			return element.Bound;
		}

		/*inline static bool AreElementsEqual(const FPrimitiveSceneInfoCompact& A, const FPrimitiveSceneInfoCompact& B)
		{
			return A.PrimitiveSceneInfo == B.PrimitiveSceneInfo;
		}

		inline static void SetElementId(const FPrimitiveSceneInfoCompact& Element, FOctreeElementId2 Id)
		{
			Element.PrimitiveSceneInfo->OctreeId = Id;
			SetOctreeNodeIndex(Element, Id);
		}

		inline static void ApplyOffset(FPrimitiveSceneInfoCompact& Element, FVector Offset)
		{
			Element.Bounds.Origin += Offset;
		}

		static void SetOctreeNodeIndex(const FPrimitiveSceneInfoCompact& Element, FOctreeElementId2 Id);*/
	};


	/** The type of the octree used by FScene to find primitives. */
	typedef Octree<PrimitiveOctreeNode, struct PrimitiveOctreeSemantics> ScenePrimitiveOctree;

}
