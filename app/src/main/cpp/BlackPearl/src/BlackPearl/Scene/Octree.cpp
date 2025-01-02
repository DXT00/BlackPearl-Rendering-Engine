#include "pch.h"
#include "Octree.h"

namespace BlackPearl {
	bool FOctreeChildNodeSubset::Contains(FOctreeChildNodeRef ChildRef) const
	{
		// This subset contains the child if it has all the bits set that are set for the subset containing only the child node.
		const FOctreeChildNodeSubset ChildSubset(ChildRef);
		return (ChildBits & ChildSubset.ChildBits) == ChildSubset.ChildBits;
	}

	FOctreeChildNodeSubset FOctreeNodeContext::GetIntersectingChildren(const AABB& BoundingBox) const
	{
		return FOctreeChildNodeSubset();
	}

}
