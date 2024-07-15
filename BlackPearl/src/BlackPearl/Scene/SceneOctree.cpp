#include "pch.h"
#include "SceneOctree.h"
#include "BlackPearl/Scene/Scene.h"
namespace BlackPearl {
    void PrimitiveOctreeSemantics::SetOctreeNodeIndex(const PrimitiveOctreeNode& Element, FOctreeElementId2 Id)
    {
        // When a Primitive is removed from the renderer, it's index will be invalidated.  Only update if the primitive still
        // has a valid index.
        if (Element.Scene)
        {
            Element.Scene->PrimitiveOctreeIndex[Element.GetObj()->GetId()] = Id.GetNodeIndex();
        }

    }
  /*  FPrimitiveSceneInfoCompact::FPrimitiveSceneInfoCompact(FPrimitiveSceneInfo* InPrimitiveSceneInfo)
    {
      

    }*/
}