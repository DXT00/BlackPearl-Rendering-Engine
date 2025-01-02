#include "pch.h"
#include "CullingManager.h"

namespace BlackPearl {

	std::vector<Object*> CullingManager::CullOctree(Scene* Scene, const IView& View, std::vector<bool>& OutVisibleNodes)
	{

		// Two bits per octree node, 1st bit is Inside Frustum, 2nd bit is Outside Frustum
		OutVisibleNodes.assign(Scene->PrimitiveOctree->GetNumNodes() * 2, false);

		Scene->PrimitiveOctree->FindNodesWithPredicate(
			[&View, &OutVisibleNodes](FNodeIndex ParentNodeIndex, FNodeIndex NodeIndex, const AABB& NodeBounds)
			{
				// If the parent node is completely contained there is no need to test containment
				if (ParentNodeIndex != INDEX_NONE && !OutVisibleNodes[(ParentNodeIndex * 2) + 1])
				{
					OutVisibleNodes[NodeIndex * 2] = true;
					OutVisibleNodes[NodeIndex * 2 + 1] = false;
					return true;
				}

				//const FPlane* PermutedPlanePtr = View.ViewFrustum.PermutedPlanes.GetData();
				bool bIntersects = false;

				/*if (Flags.bUseFastIntersect)
				{
					bIntersects = IntersectBox8Plane(NodeBounds.Center, NodeBounds.Extent, PermutedPlanePtr);
				}
				else*/
				{
					bIntersects = View.GetViewFrustum().intersectsWith(NodeBounds);
				}

				if (bIntersects)
				{
					OutVisibleNodes[NodeIndex * 2] = true;
					OutVisibleNodes[NodeIndex * 2 + 1] = true;// View.GetViewFrustum().GetBoxIntersectionOutcode(NodeBounds.GetCenter(), NodeBounds.GetExtent()).GetOutside();
				}

				return bIntersects;
			},
			[](FNodeIndex /*ParentNodeIndex*/, FNodeIndex /*NodeIndex*/, const AABB& /*NodeBounds*/)
			{

			});

		return _GetVisibleObjs(OutVisibleNodes, Scene);
	}

	std::vector<Object*> CullingManager::_GetVisibleObjs(std::vector<bool>& OutVisibleNodes, Scene* Scene)
	{
		std::vector<Object*> visibleObjs;
		// ∂‡œﬂ≥Ãcull
		std::vector<Object*> objs = Scene->GetObjects();
		for (size_t i = 0; i < objs.size(); i++)
		{
			uint32_t OctreeNodeIndex = Scene->PrimitiveOctreeIndex[objs[i]->GetId().id];

			bool isVisible = OutVisibleNodes[OctreeNodeIndex * 2];
			if (isVisible) {
				visibleObjs.push_back(objs[i]);
			}
		}

		return visibleObjs;
	}

}