#pragma once
namespace BlackPearl {
	using FNodeIndex = uint32_t;
	/** The context of an octree node, derived from the traversal of the tree. */
	class FOctreeNodeContext {

	};

	enum :uint32_t
	{
		INDEX_NONE = 0xffffffff
	};
	/** An octree. */
	template<typename ElementType, typename OctreeSemantics>
	class Octree
	{
	public:

	private:
		struct FNode
		{
			FNodeIndex ChildNodes = INDEX_NONE;
			uint32_t InclusiveNumElements = 0;

			bool IsLeaf() const
			{
				return ChildNodes == INDEX_NONE;
			}
		};

		FOctreeNodeContext RootNodeContext;
		std::vector<FNode> TreeNodes;
		std::vector<FNodeIndex> ParentLinks;
		std::vector<ElementType> TreeElements;


		class FFreeList
		{
			struct FSpan
			{
				FNodeIndex Start;
				FNodeIndex End;
			};

			std::vector<FSpan> FreeList;

		public:
			FFreeList()
			{
				Reset();
			}
			//0 , 1 , 2 , 3 , 4 ,
			void Push(FNodeIndex NodeIndex)
			{
				//find the index that points to our right side node
				int Index = 1; //exclude the dummy
				int Size = FreeList.size() - 1;

				//start with binary search for larger lists
				while (Size > 32)
				{
					const int LeftoverSize = Size % 2;
					Size = Size / 2;

					const int CheckIndex = Index + Size;
					const int IndexIfLess = CheckIndex + LeftoverSize;

					Index = FreeList[CheckIndex].Start > NodeIndex ? IndexIfLess : Index;
				}

				//small size array optimization
				int ArrayEnd = Math::Min(Index + Size + 1, (int)FreeList.size());
				while (Index < ArrayEnd)
				{
					if (FreeList[Index].Start < NodeIndex)
					{
						break;
					}
					Index++;
				}

				//can we merge with the right node
				if (Index < FreeList.size() && FreeList[Index].End + 1 == NodeIndex)
				{
					FreeList[Index].End = NodeIndex;

					//are we filling the gap between the left and right node
					if (FreeList[Index - 1].Start - 1 == NodeIndex)
					{
						FreeList[Index - 1].Start = FreeList[Index].Start;
						FreeList.erase(FreeList.begin() + Index);
					}
					return;
				}

				//can we merge with the left node
				if (FreeList[Index - 1].Start - 1 == NodeIndex)
				{
					FreeList[Index - 1].Start = NodeIndex;
					return;
				}

				//we are node that could not be merged
				FreeList.insert(FreeList.begin() + Index, FSpan{ NodeIndex , NodeIndex });
			}

			FNodeIndex Pop()
			{
				FSpan& Span = FreeList.back();
				FNodeIndex Index = Span.Start;
				assert(Index != INDEX_NONE);
				if (Span.Start == Span.End)
				{
					FreeList.pop_back();
					return Index;
				}
				else
				{
					Span.Start++;
					return Index;
				}
			}

			void Reset()
			{
				FreeList.resize(1);
				//push a dummy
				FreeList[0] = FSpan{ INDEX_NONE, INDEX_NONE };
			}

			int Num() const
			{
				//includes one dummy
				return FreeList.size() - 1;
			}
		};

		std::vector<FNodeIndex> FreeList;
		/** The extent of a leaf at the maximum allowed depth of the tree. */
		double MinLeafExtent;

		FNodeIndex AllocateEightNodes()
		{
			FNodeIndex Index = INDEX_NONE;
			if (FreeList.size())
			{
				Index = (FreeList.back() * 8) + 1;
				FreeList.pop_back();
			}
			else
			{
				Index = TreeNodes.size();
				for (size_t i = 0; i < 8; i++)
				{
					TreeNodes.push_back(FNode());
				}
				ParentLinks.push_back(INDEX_NONE);
				FNodeIndex ElementIndex = TreeElements.size();
				for (size_t i = 0; i < 8; i++)
				{
					TreeElements.push_back(ElementType);
				}

				assert(Index == ElementIndex);
			}
			return Index;
		}

		void FreeEightNodes(FNodeIndex Index)
		{
			assert(Index != INDEX_NONE && Index != 0);
			for (int8 i = 0; i < 8; i++)
			{
				TreeNodes[Index + i] = FNode();
				assert(TreeElements[Index + i].Num() == 0);
			}
			ParentLinks[(Index - 1) / 8] = INDEX_NONE;
			//TODO shrink the TreeNodes as well as the TreeElements and ParentLinks to reduce high watermark memory footprint.
			FreeList.Push((Index - 1) / 8);
		}

		void AddElementInternal(FNodeIndex CurrentNodeIndex, const FOctreeNodeContext& NodeContext, const FBoxCenterAndExtent& ElementBounds, typename TCallTraits<ElementType>::ConstReference Element, ElementArrayType& TempElementStorage)
		{
			assert(CurrentNodeIndex != INDEX_NONE);
			TreeNodes[CurrentNodeIndex].InclusiveNumElements++;
			if (TreeNodes[CurrentNodeIndex].IsLeaf())
			{
				if (TreeElements[CurrentNodeIndex].Num() + 1 > OctreeSemantics::MaxElementsPerLeaf && NodeContext.Bounds.Extent.X > MinLeafExtent)
				{
					TempElementStorage = MoveTemp(TreeElements[CurrentNodeIndex]);

					FNodeIndex ChildStartIndex = AllocateEightNodes();
					ParentLinks[(ChildStartIndex - 1) / 8] = CurrentNodeIndex;
					TreeNodes[CurrentNodeIndex].ChildNodes = ChildStartIndex;
					TreeNodes[CurrentNodeIndex].InclusiveNumElements = 0;

					for (typename TCallTraits<ElementType>::ConstReference ChildElement : TempElementStorage)
					{
						const FBoxCenterAndExtent ChildElementBounds(OctreeSemantics::GetBoundingBox(ChildElement));
						AddElementInternal(CurrentNodeIndex, NodeContext, ChildElementBounds, ChildElement, TempElementStorage);
					}

					TempElementStorage.Reset();
					AddElementInternal(CurrentNodeIndex, NodeContext, ElementBounds, Element, TempElementStorage);
					return;
				}
				else
				{
					int ElementIndex = TreeElements[CurrentNodeIndex].Emplace(Element);

					SetElementId(Element, FOctreeElementId2(CurrentNodeIndex, ElementIndex));
					return;
				}
			}
			else
			{
				const FOctreeChildNodeRef ChildRef = NodeContext.GetContainingChild(ElementBounds);
				if (ChildRef.IsNULL())
				{
					int ElementIndex = TreeElements[CurrentNodeIndex].Emplace(Element);
					SetElementId(Element, FOctreeElementId2(CurrentNodeIndex, ElementIndex));
					return;
				}
				else
				{
					FNodeIndex ChildNodeIndex = TreeNodes[CurrentNodeIndex].ChildNodes + ChildRef.Index;
					FOctreeNodeContext ChildNodeContext = NodeContext.GetChildContext(ChildRef);
					AddElementInternal(ChildNodeIndex, ChildNodeContext, ElementBounds, Element, TempElementStorage);
					return;
				}
			}
		}

		void CollapseNodesInternal(FNodeIndex CurrentNodeIndex, ElementArrayType& CollapsedNodeElements)
		{
			CollapsedNodeElements.Append(MoveTemp(TreeElements[CurrentNodeIndex]));
			TreeElements[CurrentNodeIndex].Reset();

			if (!TreeNodes[CurrentNodeIndex].IsLeaf())
			{
				FNodeIndex ChildStartIndex = TreeNodes[CurrentNodeIndex].ChildNodes;
				for (int8 i = 0; i < 8; i++)
				{
					CollapseNodesInternal(ChildStartIndex + i, CollapsedNodeElements);
				}
				FreeEightNodes(ChildStartIndex);
			}
		}

		template<typename PredicateFunc, typename IterateFunc>
		void FindNodesWithPredicateInternal(FNodeIndex ParentNodeIndex, FNodeIndex CurrentNodeIndex, const FOctreeNodeContext& NodeContext, const PredicateFunc& Predicate, const IterateFunc& Func) const
		{
			if (TreeNodes[CurrentNodeIndex].InclusiveNumElements > 0)
			{
				if (Predicate(ParentNodeIndex, CurrentNodeIndex, NodeContext.Bounds))
				{
					Func(ParentNodeIndex, CurrentNodeIndex, NodeContext.Bounds);

					if (!TreeNodes[CurrentNodeIndex].IsLeaf())
					{
						FNodeIndex ChildStartIndex = TreeNodes[CurrentNodeIndex].ChildNodes;
						for (int8 i = 0; i < 8; i++)
						{
							FindNodesWithPredicateInternal(CurrentNodeIndex, ChildStartIndex + i, NodeContext.GetChildContext(FOctreeChildNodeRef(i)), Predicate, Func);
						}
					}
				}
			}
		}

		template<typename IterateFunc>
		void FindElementsWithBoundsTestInternal(FNodeIndex CurrentNodeIndex, const FOctreeNodeContext& NodeContext, const FBoxCenterAndExtent& BoxBounds, const IterateFunc& Func) const
		{
			if (TreeNodes[CurrentNodeIndex].InclusiveNumElements > 0)
			{
				for (typename TCallTraits<ElementType>::ConstReference Element : TreeElements[CurrentNodeIndex])
				{
					if (Intersect(OctreeSemantics::GetBoundingBox(Element), BoxBounds))
					{
						Func(Element);
					}
				}

				if (!TreeNodes[CurrentNodeIndex].IsLeaf())
				{
					const FOctreeChildNodeSubset IntersectingChildSubset = NodeContext.GetIntersectingChildren(BoxBounds);
					FNodeIndex ChildStartIndex = TreeNodes[CurrentNodeIndex].ChildNodes;
					for (int8 i = 0; i < 8; i++)
					{
						if (IntersectingChildSubset.Contains(FOctreeChildNodeRef(i)))
						{
							FindElementsWithBoundsTestInternal(ChildStartIndex + i, NodeContext.GetChildContext(FOctreeChildNodeRef(i)), BoxBounds, Func);
						}
					}
				}
			}
		}

		template<typename IterateFunc>
		void FindFirstElementWithBoundsTestInternal(FNodeIndex CurrentNodeIndex, const FOctreeNodeContext& NodeContext, const FBoxCenterAndExtent& BoxBounds, const IterateFunc& Func, bool& ContinueTraversal) const
		{
			if (ContinueTraversal && TreeNodes[CurrentNodeIndex].InclusiveNumElements > 0)
			{
				for (int Index = 0; ContinueTraversal && Index < TreeElements[CurrentNodeIndex].Num(); Index++)
				{
					typename TCallTraits<ElementType>::ConstReference Element = TreeElements[CurrentNodeIndex][Index];
					if (Intersect(OctreeSemantics::GetBoundingBox(Element), BoxBounds))
					{
						ContinueTraversal &= Func(Element);
					}
				}

				if (ContinueTraversal && !TreeNodes[CurrentNodeIndex].IsLeaf())
				{
					const FOctreeChildNodeSubset IntersectingChildSubset = NodeContext.GetIntersectingChildren(BoxBounds);
					FNodeIndex ChildStartIndex = TreeNodes[CurrentNodeIndex].ChildNodes;
					for (int8 i = 0; i < 8; i++)
					{
						if (IntersectingChildSubset.Contains(FOctreeChildNodeRef(i)))
						{
							FindFirstElementWithBoundsTestInternal(ChildStartIndex + i, NodeContext.GetChildContext(FOctreeChildNodeRef(i)), BoxBounds, Func, ContinueTraversal);
						}
					}
				}
			}
		}

		template<typename IterateFunc>
		void FindNearbyElementsInternal(FNodeIndex CurrentNodeIndex, const FOctreeNodeContext& NodeContext, const FBoxCenterAndExtent& BoxBounds, const IterateFunc& Func) const
		{
			if (TreeNodes[CurrentNodeIndex].InclusiveNumElements > 0)
			{
				for (int Index = 0; Index < TreeElements[CurrentNodeIndex].Num(); Index++)
				{
					Func(TreeElements[CurrentNodeIndex][Index]);
				}

				if (!TreeNodes[CurrentNodeIndex].IsLeaf())
				{
					// Find the child of the current node, if any, that contains the current new point
					FOctreeChildNodeRef ChildRef = NodeContext.GetContainingChild(BoxBounds);
					if (!ChildRef.IsNULL())
					{
						FNodeIndex ChildStartIndex = TreeNodes[CurrentNodeIndex].ChildNodes;
						// If the specified child node exists and contains any match, push it than process it
						if (TreeNodes[ChildStartIndex + ChildRef.Index].InclusiveNumElements > 0)
						{
							FindNearbyElementsInternal(ChildStartIndex + ChildRef.Index, NodeContext.GetChildContext(ChildRef), BoxBounds, Func);
						}
						// If the child node doesn't is a match, it's not worth pursuing any further. In an attempt to find
						// anything to match vs. the new point, process all of the children of the current octree node
						else
						{
							for (int8 i = 0; i < 8; i++)
							{
								FindNearbyElementsInternal(ChildStartIndex + i, NodeContext.GetChildContext(FOctreeChildNodeRef(i)), BoxBounds, Func);
							}
						}
					}
				}
			}
		}
	public:

		int32 GetNumNodes() const { return TreeNodes.Num(); }

		/**
		 * this function will call the passed in function for all elements in the Octree in node by node in no specified order.
		 * @param Func - Function to call with each Element.
		 */
		template<typename IterateAllElementsFunc>
		inline void FindAllElements(const IterateAllElementsFunc& Func) const
		{
			for (const ElementArrayType& Elements : TreeElements)
			{
				for (typename TCallTraits<ElementType>::ConstReference Element : Elements)
				{
					Func(Element);
				}
			}
		}

		/**
		 * this function will traverse the Octree starting from the root in depth first order and the predicate can be used to implement custom culling for each node.
		 * @param Predicate - a Function when given the bounds of the currently traversed node that returns true if traversal should continue or false to skip that branch.
		 * @param Func - Function that will receive the node ID which can be stored and later used to get the elements using GetElementsForNode for all nodes that passed the predicate.
		 */
		template<typename PredicateFunc, typename IterateFunc>
		inline void FindNodesWithPredicate(const PredicateFunc& Predicate, const IterateFunc& Func) const
		{
			FindNodesWithPredicateInternal(INDEX_NONE, 0, RootNodeContext, Predicate, Func);
		}

		/**
		 * this function will traverse the Octree starting from the root in depth first order and the predicate can be used to implement custom culling for each node.
		 * @param Predicate - a Function when given the bounds of the currently traversed node that returns true if traversal should continue or false to skip that branch.
		 * @param Func - Function to call with each Element for nodes that passed the predicate.
		 */
		template<typename PredicateFunc, typename IterateFunc>
		inline void FindElementsWithPredicate(const PredicateFunc& Predicate, const IterateFunc& Func) const
		{
			FindNodesWithPredicateInternal(INDEX_NONE, 0, RootNodeContext, Predicate, [&Func, this](FNodeIndex /*ParentNodeIndex*/, FNodeIndex NodeIndex, const FBoxCenterAndExtent& /*NodeBounds*/)
				{
					for (typename TCallTraits<ElementType>::ConstReference Element : TreeElements[NodeIndex])
					{
						Func(NodeIndex, Element);
					}
				});
		}

		/**
		 * this function will traverse the Octree using a fast box-box intersection this should be the preferred way of traversing the tree.
		 * @param BoxBounds - the bounds to test if a node is traversed or skipped.
		 * @param Func - Function to call with each Element for nodes that passed bounds test.
		 */
		template<typename IterateBoundsFunc>
		inline void FindElementsWithBoundsTest(const FBoxCenterAndExtent& BoxBounds, const IterateBoundsFunc& Func) const
		{
			FindElementsWithBoundsTestInternal(0, RootNodeContext, BoxBounds, Func);
		}

		/**
		 * this function will traverse the Octree using a fast box-box intersection and aborts traversal as soon as the Element function returns false.
		 * @param BoxBounds - the bounds to test if a node is traversed or skipped.
		 * @param Func - Function to call with each Element for nodes that passed bounds test.
		 */
		template<typename IterateBoundsFunc>
		inline void FindFirstElementWithBoundsTest(const FBoxCenterAndExtent& BoxBounds, const IterateBoundsFunc& Func) const
		{
			bool ContinueTraversal = true;
			FindFirstElementWithBoundsTestInternal(0, RootNodeContext, BoxBounds, Func, ContinueTraversal);
		}

		/**
		* this function will traverse the Octree using a tryint to find nearby nodes that contain any elements.
		* @param Position - the position to look nearby.
		* @param Func - Function to call with each Element for nodes that passed bounds test.
		*/
		template<typename IterateBoundsFunc>
		inline void FindNearbyElements(const FVector& Position, const IterateBoundsFunc& Func) const
		{
			FindNearbyElementsInternal(0, RootNodeContext, FBoxCenterAndExtent(Position, FVector::ZeroVector), Func);
		}


		/**
		 * Adds an element to the octree.
		 * @param Element - The element to add.
		 */
		inline void AddElement(typename TCallTraits<ElementType>::ConstReference Element)
		{
			ElementArrayType TempElementStorage;
			const FBoxCenterAndExtent ElementBounds(OctreeSemantics::GetBoundingBox(Element));
			AddElementInternal(0, RootNodeContext, ElementBounds, Element, TempElementStorage);
		}

		/**
		 * Removes an element from the octree.
		 * @param ElementId - The element to remove from the octree.
		 */
		inline void RemoveElement(FOctreeElementId2 ElementId)
		{
			checkSlow(ElementId.IsValidId());

			// Remove the element from the node's element list.
			TreeElements[ElementId.NodeIndex].RemoveAtSwap(ElementId.ElementIndex, 1, false);

			if (ElementId.ElementIndex < TreeElements[ElementId.NodeIndex].Num())
			{
				// Update the external element id for the element that was swapped into the vacated element index.
				SetElementId(TreeElements[ElementId.NodeIndex][ElementId.ElementIndex], ElementId);
			}

			FNodeIndex CollapseNodeIndex = INDEX_NONE;
			{
				// Update the inclusive element counts for the nodes between the element and the root node,
				// and find the largest node that is small enough to collapse.
				FNodeIndex NodeIndex = ElementId.NodeIndex;
				while (true)
				{
					TreeNodes[NodeIndex].InclusiveNumElements--;
					if (TreeNodes[NodeIndex].InclusiveNumElements < OctreeSemantics::MinInclusiveElementsPerNode)
					{
						CollapseNodeIndex = NodeIndex;
					}

					if (NodeIndex == 0)
					{
						break;
					}

					NodeIndex = ParentLinks[(NodeIndex - 1) / 8];
				}
			}

			// Collapse the largest node that was pushed below the threshold for collapse by the removal.
			if (CollapseNodeIndex != INDEX_NONE && !TreeNodes[CollapseNodeIndex].IsLeaf())
			{
				if (TreeElements[CollapseNodeIndex].Num() < (int32)TreeNodes[CollapseNodeIndex].InclusiveNumElements)
				{
					ElementArrayType TempElementStorage;
					TempElementStorage.Reserve(TreeNodes[CollapseNodeIndex].InclusiveNumElements);
					// Gather the elements contained in this node and its children.
					CollapseNodesInternal(CollapseNodeIndex, TempElementStorage);
					TreeElements[CollapseNodeIndex] = MoveTemp(TempElementStorage);

					for (int ElementIndex = 0; ElementIndex < TreeElements[CollapseNodeIndex].Num(); ElementIndex++)
					{
						// Update the external element id for the element that's being collapsed.
						SetElementId(TreeElements[CollapseNodeIndex][ElementIndex], FOctreeElementId2(CollapseNodeIndex, ElementIndex));
					}

					// Mark the node as a leaf.
					TreeNodes[CollapseNodeIndex].ChildNodes = INDEX_NONE;
				}
			}
		}

		/**
		 * this function resets the octree to empty.
		 */
		void Destroy()
		{
			TreeNodes.Reset(1);
			TreeElements.Reset(1);
			FreeList.Reset();
			ParentLinks.Reset();
			TreeNodes.AddDefaulted();
			TreeElements.AddDefaulted();
		}

		/** Accesses an octree element by ID. */
		ElementType& GetElementById(FOctreeElementId2 ElementId)
		{
			return TreeElements[ElementId.NodeIndex][ElementId.ElementIndex];
		}

		/** Accesses an octree element by ID. */
		const ElementType& GetElementById(FOctreeElementId2 ElementId) const
		{
			return TreeElements[ElementId.NodeIndex][ElementId.ElementIndex];
		}

		/**
		 * check if a FOctreeElementId2 is valid.
		 * @param ElementId - The ElementId to check.
		 */
		bool IsValidElementId(FOctreeElementId2 ElementId) const
		{
			return ElementId.IsValidId() && ElementId.ElementIndex < TreeElements[ElementId.NodeIndex].Num();
		};

		/**
		 * return all elements for a given node.
		 * @param NodeIndex - The the index of the node can be obtained using FindNodesWithPredicate.
		 */
		TArrayView<const ElementType> GetElementsForNode(FNodeIndex NodeIndex) const
		{
			return TreeElements[NodeIndex];
		}

		/** Writes stats for the octree to the log. */
		void DumpStats() const
		{
			int32_t NumNodes = 0;
			int32_t NumLeaves = 0;
			int32_t NumElements = 0;
			int32_t MaxElementsPerNode = 0;
			TArray<int32_t> NodeElementDistribution;

			FindNodesWithPredicateInternal(INDEX_NONE, 0, RootNodeContext, [](FNodeIndex /*ParentNodeIndex*/, FNodeIndex /*NodeIndex*/, const FBoxCenterAndExtent&) { return true; }, [&, this](FNodeIndex /*ParentNodeIndex*/, FNodeIndex NodeIndex, const FBoxCenterAndExtent&)
				{
					const int32_t CurrentNodeElementCount = GetElementsForNode(NodeIndex).Num();

					NumNodes++;
					if (TreeNodes[NodeIndex].IsLeaf())
					{
						NumLeaves++;
					}

					NumElements += CurrentNodeElementCount;
					MaxElementsPerNode = Math::Max(MaxElementsPerNode, CurrentNodeElementCount);

					if (CurrentNodeElementCount >= NodeElementDistribution.Num())
					{
						NodeElementDistribution.AddZeroed(CurrentNodeElementCount - NodeElementDistribution.Num() + 1);
					}
					NodeElementDistribution[CurrentNodeElementCount]++;
				});

			UE_LOG(LogGenericOctree, Log, TEXT("Octree overview:"));
			UE_LOG(LogGenericOctree, Log, TEXT("\t%i nodes"), NumNodes);
			UE_LOG(LogGenericOctree, Log, TEXT("\t%i leaves"), NumLeaves);
			UE_LOG(LogGenericOctree, Log, TEXT("\t%i elements"), NumElements);
			UE_LOG(LogGenericOctree, Log, TEXT("\t%i >= elements per node"), MaxElementsPerNode);
			UE_LOG(LogGenericOctree, Log, TEXT("Octree node element distribution:"));
			for (int32 i = 0; i < NodeElementDistribution.Num(); i++)
			{
				if (NodeElementDistribution[i] > 0)
				{
					UE_LOG(LogGenericOctree, Log, TEXT("\tElements: %3i, Nodes: %3i"), i, NodeElementDistribution[i]);
				}
			}
		}

		SIZE_T GetSizeBytes() const
		{
			SIZE_T TotalSizeBytes = TreeNodes.GetAllocatedSize();
			TotalSizeBytes += TreeElements.GetAllocatedSize();
			TotalSizeBytes += TreeNodes[0].InclusiveNumElements * sizeof(ElementType);
			return TotalSizeBytes;
		}

		FReal GetNodeLevelExtent(int32 Level) const
		{
			const int32 ClampedLevel = FMath::Clamp<uint32>(Level, 0, OctreeSemantics::MaxNodeDepth);
			return RootNodeContext.Bounds.Extent.X * FMath::Pow((1.0f + 1.0f / (FReal)FOctreeNodeContext::LoosenessDenominator) / 2.0f, FReal(ClampedLevel));
		}

		FBoxCenterAndExtent GetRootBounds() const
		{
			return RootNodeContext.Bounds;
		}

		void ShrinkElements()
		{
			for (ElementArrayType& Elements : TreeElements)
			{
				Elements.Shrink();
			}
		}

		/**
		 * Apply an arbitrary offset to all elements in the tree
		 * InOffset - offset to apply
		 * bGlobalOctree - hint that this octree is used as a boundless global volume,
		 *  so only content will be shifted but not origin of the octree
		 */
		void ApplyOffset(const FVector& InOffset, bool bGlobalOctree = false)
		{
			ElementArrayType TempElementStorage;
			TempElementStorage.Reserve(TreeNodes[0].InclusiveNumElements);

			//collect all elements
			CollapseNodesInternal(0, TempElementStorage);
			checkSlow(TreeNodes[0].IsLeaf());
			Destroy();

			if (!bGlobalOctree)
			{
				RootNodeContext.Bounds.Center += FVector4(InOffset, 0.0f);
			}

			// Offset & Add all elements from a saved nodes to a new empty octree
			for (ElementType& Element : TempElementStorage)
			{
				OctreeSemantics::ApplyOffset(Element, InOffset);
				AddElement(Element);
			}
		}

		/** Initialization constructor. */
		TOctree(const FVector& InOrigin, FVector::FReal InExtent)
			: RootNodeContext(FBoxCenterAndExtent(InOrigin, FVector(InExtent, InExtent, InExtent)), 0, 0)
			, MinLeafExtent(InExtent* FMath::Pow((1.0f + 1.0f / (FReal)FOctreeNodeContext::LoosenessDenominator) / 2.0f, FReal(OctreeSemantics::MaxNodeDepth)))
		{
			TreeNodes.AddDefaulted();
			TreeElements.AddDefaulted();
		}

		/** DO NOT USE. This constructor is for internal usage only for hot-reload purposes. */
		TOctree()
		{
			TreeNodes.AddDefaulted();
			TreeElements.AddDefaulted();
		}

	private:


		// Concept definition for the new octree semantics, which adds a new TOctree parameter
		struct COctreeSemanticsV2
		{
			template<typename Semantics>
			auto Requires(typename Semantics::FOctree& OctreeInstance, const ElementType& Element, FOctreeElementId2 Id)
				-> decltype(Semantics::SetElementId(OctreeInstance, Element, Id));
		};

		// Function overload set which calls the V2 version if it's supported or the old version if it's not
		template <typename Semantics>
		void SetOctreeSemanticsElementId(const ElementType& Element, FOctreeElementId2 Id)
		{
			if constexpr (TModels_V<COctreeSemanticsV2, Semantics>)
			{
				Semantics::SetElementId(static_cast<typename Semantics::FOctree&>(*this), Element, Id);
			}
			else
			{
				Semantics::SetElementId(Element, Id);
			}
		}

	protected:
		// redirects SetElementId call to the proper implementation
		void SetElementId(const ElementType& Element, FOctreeElementId2 Id)
		{
			SetOctreeSemanticsElementId<OctreeSemantics>(Element, Id);
		}
	};




}
