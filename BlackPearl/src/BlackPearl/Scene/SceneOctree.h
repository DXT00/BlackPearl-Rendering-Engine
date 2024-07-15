#pragma once
#include <cassert>
#include "BlackPearl/Math/Math.h"
#include "Octree.h"
#include "BlackPearl/AABB/AABB.h"
#include "BlackPearl/Object/Object.h"
namespace BlackPearl {
	class Scene;
	//class FPrimitiveSceneInfo {
	//public:
	//	/** The identifier for the primitive in Scene->PrimitiveOctree. */
	//	FOctreeElementId2 OctreeId;
	//};
	///** The information needed to determine whether a primitive is visible. */
	//class FPrimitiveSceneInfoCompact
	//{
	//public:
	//	FPrimitiveSceneInfo* PrimitiveSceneInfo;
	//	AABB Bound;
	//	//FPrimitiveSceneProxy* Proxy;
	//	//FCompactBoxSphereBounds Bounds;
	//	//float MinDrawDistance;
	//	//float MaxDrawDistance;
	//	///** Used for precomputed visibility */
	//	//int32 VisibilityId;
	//	//FPrimitiveFlagsCompact PrimitiveFlagsCompact;

	//	/** Initialization constructor. */
	//	FPrimitiveSceneInfoCompact(FPrimitiveSceneInfo* InPrimitiveSceneInfo);
	//};


	class PrimitiveOctreeNode {
	public:
		PrimitiveOctreeNode(Object* obj = nullptr) {
			m_Obj = obj;
			
		}
		bool IsLeaf() {
			return m_Obj != nullptr;
		}
		FOctreeElementId2 OctreeNodeId;
		Scene* Scene = nullptr;
		Object* GetObj() const { return m_Obj; };

		AABB Bound;

	private: 
		Object* m_Obj = nullptr;
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

		inline static bool AreElementsEqual(const PrimitiveOctreeNode& A, const PrimitiveOctreeNode& B)
		{
			return A.Scene == B.Scene;
		}

		inline static void SetElementId(PrimitiveOctreeNode& Element, FOctreeElementId2 Id)
		{
			Element.OctreeNodeId = Id;
			SetOctreeNodeIndex(Element, Id);
		}

		inline static void ApplyOffset(PrimitiveOctreeNode& Element, math::float3 Offset)
		{
			Element.Bound.m_Center += Offset;
		}

		static void SetOctreeNodeIndex(const PrimitiveOctreeNode& Element, FOctreeElementId2 Id);
	};


	/** The type of the octree used by FScene to find primitives. */
	typedef Octree<PrimitiveOctreeNode, struct PrimitiveOctreeSemantics> ScenePrimitiveOctree;

}
