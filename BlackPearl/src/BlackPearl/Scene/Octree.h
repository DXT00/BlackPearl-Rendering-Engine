#pragma once
#include "BlackPearl/Math/Math.h"
#include "BlackPearl/Math/basics.h"
#include "BlackPearl/AABB/AABB.h"
namespace BlackPearl {
	using FNodeIndex = uint32_t;
	enum :uint32_t
	{
		INDEX_NONE = 0xffffffff
	};
	class FOctreeChildNodeRef
	{
	public:
		int8_t Index;

		/** Initialization constructor. */
		FOctreeChildNodeRef(int8_t InX, int8_t InY, int8_t InZ)
		{
			assert(InX >= 0 && InX <= 1);
			assert(InY >= 0 && InY <= 1);
			assert(InZ >= 0 && InZ <= 1);
			Index = int8_t(InX << 0) | int8_t(InY << 1) | int8_t(InZ << 2);
		}

		/** Initialized the reference with a child index. */
		FOctreeChildNodeRef(int8_t InIndex = 0)
			: Index(InIndex)
		{
			assert(Index < 8);
		}

		/** Advances the reference to the next child node.  If this was the last node remain, Index will be 8 which represents null. */
		inline void Advance()
		{
			++Index;
		}

		/** @return true if the reference isn't set. */
		inline bool IsNULL() const
		{
			return Index >= 8;
		}

		inline void SetNULL()
		{
			Index = 8;
		}

		inline int32_t X() const
		{
			return (Index >> 0) & 1;
		}

		inline int32_t Y() const
		{
			return (Index >> 1) & 1;
		}

		inline int32_t Z() const
		{
			return (Index >> 2) & 1;
		}
	};

	/** A subset of an octree node's children that intersect a bounding box. */
	class FOctreeChildNodeSubset
	{
	public:

		union
		{
			struct
			{
				uint32_t bPositiveX : 1;
				uint32_t bPositiveY : 1;
				uint32_t bPositiveZ : 1;
				uint32_t bNegativeX : 1;
				uint32_t bNegativeY : 1;
				uint32_t bNegativeZ : 1;
			};

			struct
			{
				/** Only the bits for the children on the positive side of the splits. */
				uint32_t PositiveChildBits : 3;

				/** Only the bits for the children on the negative side of the splits. */
				uint32_t NegativeChildBits : 3;
			};

			/** All the bits corresponding to the child bits. */
			uint32_t ChildBits : 6;

			/** All the bits used to store the subset. */
			uint32_t AllBits;
		};

		/** Initializes the subset to be empty. */
		FOctreeChildNodeSubset()
			: AllBits(0)
		{}

		/** Initializes the subset to contain a single node. */
		FOctreeChildNodeSubset(FOctreeChildNodeRef ChildRef)
			: AllBits(0)
		{
			// The positive child bits correspond to the child index, and the negative to the NOT of the child index.
			PositiveChildBits = ChildRef.Index;
			NegativeChildBits = ~ChildRef.Index;
		}

		/** Determines whether the subset contains a specific node. */
		bool Contains(FOctreeChildNodeRef ChildRef) const;
	};

	/**
 *	An identifier for an element in the octree.
 */
	class FOctreeElementId2
	{
	public:

		template<typename, typename>
		friend class Octree;

		/** Default constructor. */
		FOctreeElementId2()
			: NodeIndex(INDEX_NONE)
			, ElementIndex(INDEX_NONE)
		{}

		/** @return a boolean value representing whether the id is NULL. */
		bool IsValidId() const
		{
			return NodeIndex != INDEX_NONE;
		}

		uint32_t GetNodeIndex() const { return NodeIndex; }

	private:

		/** The node the element is in. */
		uint32_t NodeIndex;

		/** The index of the element in the node's element array. */
		int32_t ElementIndex;

		/** Initialization constructor. */
		FOctreeElementId2(uint32_t InNodeIndex, int32_t InElementIndex)
			: NodeIndex(InNodeIndex)
			, ElementIndex(InElementIndex)
		{}

		/** Implicit conversion to the element index. */
		operator int32_t() const
		{
			return ElementIndex;
		}
	};

	/** The context of an octree node, derived from the traversal of the tree. */
	class FOctreeNodeContext {
	public:

		/** The node bounds are expanded by
		their extent divided by LoosenessDenominator. */
		enum { LoosenessDenominator = 16 };
		/** The bounds of the node. */
		AABB Bounds;

		/** The extent of the node's children. */
		double ChildExtent;

		/** The offset of the childrens' centers from the center of this node. */
		double ChildCenterOffset;

		/** Bits used for culling, semantics left up to the caller (except that it is always set to zero at the root). This does not consume storage because it is leftover in the padding.*/
		uint32_t InCullBits;

		/** Bits used for culling, semantics left up to the caller (except that it is always set to zero at the root). This does not consume storage because it is leftover in the padding.*/
		uint32_t OutCullBits;

		/** Default constructor. */
		FOctreeNodeContext() {}
		/** Initialization constructor. */
		FOctreeNodeContext(const AABB& InBounds)
			: Bounds(InBounds)
		{
			// A child node's tight extents are half its parent's extents, and its loose extents are expanded by 1/LoosenessDenominator.
			const double TightChildExtent = Bounds.GetExtent().x * 0.5f;
			const double LooseChildExtent = TightChildExtent * (1.0f + 1.0f / (double)LoosenessDenominator);

			ChildExtent = LooseChildExtent;
			ChildCenterOffset = Bounds.GetExtent().x - LooseChildExtent;
		}

		/** Initialization constructor. */
		FOctreeNodeContext(const AABB& InBounds, uint32_t InInCullBits, uint32_t InOutCullBits)
			: Bounds(InBounds)
			, InCullBits(InInCullBits)
			, OutCullBits(InOutCullBits)
		{
			// A child node's tight extents are half its parent's extents, and its loose extents are expanded by 1/LoosenessDenominator.
			const double TightChildExtent = Bounds.GetExtent().x * 0.5f;
			const double LooseChildExtent = TightChildExtent * (1.0f + 1.0f / (double)LoosenessDenominator);

			ChildExtent = LooseChildExtent;
			ChildCenterOffset = Bounds.GetExtent().x - LooseChildExtent;
		}


		inline math::float4 GetChildOffsetVec(int i) const
		{
			// LWC_TODO: not sure this is correct for VectorRegister = VectorRegister4Double
			union MaskType {
				MaskType() {}
				math::float4 v;
				math::int4 i;
			};
			MaskType Mask;
			Mask.v = math::float4(1u, 2u, 4u, 8u);//0001,0010,0100,1000
			math::int4 X = math::int4(i);
			math::int4 A = Math::VectorIntAnd(X, Mask.i);
			Mask.i = Math::VectorIntCompareEQ(Mask.i, A);
			return Math::VectorSelect(Mask.v, math::float4(ChildCenterOffset), math::float4(-ChildCenterOffset));
		}

		/** Child node initialization constructor. */
		inline FOctreeNodeContext GetChildContext(FOctreeChildNodeRef ChildRef) const
		{
			AABB LocalBounds;
			math::float4 ZeroW = math::float4(1.0f, 1.0f, 1.0f, 0.0f);
			math::float4 offset = GetChildOffsetVec(ChildRef.Index);
			LocalBounds.m_Center = ZeroW.xyz() * (Bounds.m_Center + offset.xyz());
			LocalBounds.m_Extent = (ZeroW.xyz() * math::float3(ChildExtent));
			return FOctreeNodeContext(LocalBounds);
		}

		/** Construct a child context given the child ref. Optimized to remove all LHS. */
		inline void GetChildContext(FOctreeChildNodeRef ChildRef, FOctreeNodeContext* ChildContext) const
		{
			math::float4 ZeroW = math::float4(1.0f, 1.0f, 1.0f, 0.0f);
			math::float4 offset = GetChildOffsetVec(ChildRef.Index);

			ChildContext->Bounds.m_Center = ZeroW.xyz() * (Bounds.m_Center + offset.xyz());
			ChildContext->Bounds.m_Extent = ZeroW.xyz() * math::float3(ChildExtent);

			const double TightChildExtent = ChildExtent * 0.5f;
			const double LooseChildExtent = TightChildExtent * (1.0f + 1.0f / (double)LoosenessDenominator);
			ChildContext->ChildExtent = LooseChildExtent;
			ChildContext->ChildCenterOffset = ChildExtent - LooseChildExtent;
		}

		/** Child node initialization constructor. */
		inline FOctreeNodeContext GetChildContext(FOctreeChildNodeRef ChildRef, uint32_t InInCullBits, uint32_t InOutCullBits) const
		{
			AABB LocalBounds;
			math::float4 ZeroW = math::float4(1.0f, 1.0f, 1.0f, 0.0f);
			math::float4 offset = GetChildOffsetVec(ChildRef.Index);

			LocalBounds.m_Center = ZeroW.xyz() * (Bounds.m_Center + offset.xyz());
			LocalBounds.m_Extent = (ZeroW.xyz() * math::float3(ChildExtent));

			return FOctreeNodeContext(LocalBounds, InInCullBits, InOutCullBits);
		}
		/**
		 * Determines which of the octree node's children intersect with a bounding box.
		 * @param BoundingBox - The bounding box to check for intersection with.
		 * @return A subset of the children's nodes that intersect the bounding box.
		 */
		FOctreeChildNodeSubset GetIntersectingChildren(const AABB& BoundingBox) const;

		/**
		 * Determines which of the octree node's children contain the whole bounding box, if any.
		 * @param BoundingBox - The bounding box to check for intersection with.
		 * @return The octree's node that the bounding box is farthest from the outside edge of, or an invalid node ref if it's not contained
		 *			by any of the children.
		 */

		inline FOctreeChildNodeRef FOctreeNodeContext::GetContainingChild(const AABB& QueryBounds) const
		{
			FOctreeChildNodeRef Result;

			// Load the query bounding box values as VectorRegisters.
			const math::float3 QueryBoundsCenter = QueryBounds.GetCenter();
			const math::float3 QueryBoundsExtent = QueryBounds.GetExtent();

			// Compute the bounds of the node's children.
			const math::float3 BoundsCenter = Bounds.GetCenter();
			const math::float3 ChildCenterOffsetVector = math::float3(ChildCenterOffset);
			const math::float3 NegativeCenterDifference = QueryBoundsCenter - (BoundsCenter - ChildCenterOffsetVector);
			const math::float3 PositiveCenterDifference = ((BoundsCenter + ChildCenterOffsetVector) - QueryBoundsCenter);

			// If the query bounds isn't entirely inside the bounding box of the child it's closest to, it's not contained by any of the child nodes.
			const math::float3 MinDifference = math::min(PositiveCenterDifference, NegativeCenterDifference);
			if (Math::VectorAnyGreaterThan((QueryBoundsExtent + MinDifference), math::float3(ChildExtent)))
			{
				Result.SetNULL();
			}
			else
			{
				// Return the child node that the query is closest to as the containing child.
				Result.Index = Math::VectorMaskBits(Math::VectorCompareGT(QueryBoundsCenter, BoundsCenter)) & 0x7;
			}

			return Result;
		}
	};


	/** An octree. */
	template<typename ElementType, typename OctreeSemantics>
	class Octree
	{
		using ElementArrayType = std::vector<ElementType>;
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
		std::vector<std::vector<ElementType>> TreeElements;


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
					TreeElements.push_back(std::vector<ElementType>());
				}

				assert(Index == ElementIndex);
			}
			return Index;
		}

		void FreeEightNodes(FNodeIndex Index)
		{
			assert(Index != INDEX_NONE && Index != 0);
			for (int8_t i = 0; i < 8; i++)
			{
				TreeNodes[Index + i] = FNode();
				assert(TreeElements[Index + i].size() == 0);
			}
			ParentLinks[(Index - 1) / 8] = INDEX_NONE;
			//TODO shrink the TreeNodes as well as the TreeElements and ParentLinks to reduce high watermark memory footprint.
			FreeList.Push((Index - 1) / 8);
		}

		void AddElementInternal(FNodeIndex CurrentNodeIndex, const FOctreeNodeContext& NodeContext, const AABB& ElementBounds, ElementType& Element, std::vector<ElementType>& TempElementStorage)
		{
			assert(CurrentNodeIndex != INDEX_NONE);
			TreeNodes[CurrentNodeIndex].InclusiveNumElements++;
			if (TreeNodes[CurrentNodeIndex].IsLeaf())
			{
				if (TreeElements[CurrentNodeIndex].size() + 1 > OctreeSemantics::MaxElementsPerLeaf && NodeContext.Bounds.GetExtent().x > MinLeafExtent)
				{
					TempElementStorage = std::move(TreeElements[CurrentNodeIndex]);

					FNodeIndex ChildStartIndex = AllocateEightNodes();
					ParentLinks[(ChildStartIndex - 1) / 8] = CurrentNodeIndex;
					TreeNodes[CurrentNodeIndex].ChildNodes = ChildStartIndex;
					TreeNodes[CurrentNodeIndex].InclusiveNumElements = 0;

					for (auto ChildElement : TempElementStorage)
					{
						const AABB ChildElementBounds(OctreeSemantics::GetBoundingBox(ChildElement));
						AddElementInternal(CurrentNodeIndex, NodeContext, ChildElementBounds, ChildElement, TempElementStorage);
					}

					//TempElementStorage.Reset();
					TempElementStorage.clear();

					AddElementInternal(CurrentNodeIndex, NodeContext, ElementBounds, Element, TempElementStorage);
					return;
				}
				else
				{
					int ElementIndex = TreeElements[CurrentNodeIndex].size();
					TreeElements[CurrentNodeIndex].push_back(Element);

					SetElementId(Element, FOctreeElementId2(CurrentNodeIndex, ElementIndex));
					return;
				}
			}
			else
			{
				const FOctreeChildNodeRef ChildRef = NodeContext.GetContainingChild(ElementBounds);
				if (ChildRef.IsNULL())
				{
					int ElementIndex = TreeElements[CurrentNodeIndex].size();
						TreeElements[CurrentNodeIndex].push_back(Element);
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
			//CollapsedNodeElements.Append(std::move(TreeElements[CurrentNodeIndex]));

			for (size_t i = 0; i < TreeElements[CurrentNodeIndex].size(); i++)
			{
				CollapsedNodeElements.push_back(TreeElements[CurrentNodeIndex][i]);
			}
			TreeElements[CurrentNodeIndex].Reset();

			if (!TreeNodes[CurrentNodeIndex].IsLeaf())
			{
				FNodeIndex ChildStartIndex = TreeNodes[CurrentNodeIndex].ChildNodes;
				for (int8_t i = 0; i < 8; i++)
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
						for (int8_t i = 0; i < 8; i++)
						{
							FindNodesWithPredicateInternal(CurrentNodeIndex, ChildStartIndex + i, NodeContext.GetChildContext(FOctreeChildNodeRef(i)), Predicate, Func);
						}
					}
				}
			}
		}

		template<typename IterateFunc>
		void FindElementsWithBoundsTestInternal(FNodeIndex CurrentNodeIndex, const FOctreeNodeContext& NodeContext, const AABB& BoxBounds, const IterateFunc& Func) const
		{
			if (TreeNodes[CurrentNodeIndex].InclusiveNumElements > 0)
			{
				for (const ElementType& Element : TreeElements[CurrentNodeIndex])
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
					for (int8_t i = 0; i < 8; i++)
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
		void FindFirstElementWithBoundsTestInternal(FNodeIndex CurrentNodeIndex, const FOctreeNodeContext& NodeContext, const AABB& BoxBounds, const IterateFunc& Func, bool& ContinueTraversal) const
		{
			if (ContinueTraversal && TreeNodes[CurrentNodeIndex].InclusiveNumElements > 0)
			{
				for (int Index = 0; ContinueTraversal && Index < TreeElements[CurrentNodeIndex].size(); Index++)
				{
					const ElementType& Element = TreeElements[CurrentNodeIndex][Index];
					if (Intersect(OctreeSemantics::GetBoundingBox(Element), BoxBounds))
					{
						ContinueTraversal &= Func(Element);
					}
				}

				if (ContinueTraversal && !TreeNodes[CurrentNodeIndex].IsLeaf())
				{
					const FOctreeChildNodeSubset IntersectingChildSubset = NodeContext.GetIntersectingChildren(BoxBounds);
					FNodeIndex ChildStartIndex = TreeNodes[CurrentNodeIndex].ChildNodes;
					for (int8_t i = 0; i < 8; i++)
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
		void FindNearbyElementsInternal(FNodeIndex CurrentNodeIndex, const FOctreeNodeContext& NodeContext, const AABB& BoxBounds, const IterateFunc& Func) const
		{
			if (TreeNodes[CurrentNodeIndex].InclusiveNumElements > 0)
			{
				for (int Index = 0; Index < TreeElements[CurrentNodeIndex].size(); Index++)
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
							for (int8_t i = 0; i < 8; i++)
							{
								FindNearbyElementsInternal(ChildStartIndex + i, NodeContext.GetChildContext(FOctreeChildNodeRef(i)), BoxBounds, Func);
							}
						}
					}
				}
			}
		}
	public:

		int32_t GetNumNodes() const { return TreeNodes.size(); }

		/**
		 * this function will call the passed in function for all elements in the Octree in node by node in no specified order.
		 * @param Func - Function to call with each Element.
		 */
		template<typename IterateAllElementsFunc>
		inline void FindAllElements(const IterateAllElementsFunc& Func) const
		{
			for (const ElementArrayType& Elements : TreeElements)
			{
				for (const ElementType& Element : Elements)
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
			FindNodesWithPredicateInternal(INDEX_NONE, 0, RootNodeContext, Predicate, [&Func, this](FNodeIndex /*ParentNodeIndex*/, FNodeIndex NodeIndex, const AABB& /*NodeBounds*/)
				{
					for (const ElementType& Element : TreeElements[NodeIndex])
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
		inline void FindElementsWithBoundsTest(const AABB& BoxBounds, const IterateBoundsFunc& Func) const
		{
			FindElementsWithBoundsTestInternal(0, RootNodeContext, BoxBounds, Func);
		}

		/**
		 * this function will traverse the Octree using a fast box-box intersection and aborts traversal as soon as the Element function returns false.
		 * @param BoxBounds - the bounds to test if a node is traversed or skipped.
		 * @param Func - Function to call with each Element for nodes that passed bounds test.
		 */
		template<typename IterateBoundsFunc>
		inline void FindFirstElementWithBoundsTest(const AABB& BoxBounds, const IterateBoundsFunc& Func) const
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
		inline void FindNearbyElements(const math::float3& Position, const IterateBoundsFunc& Func) const
		{
			//(FNodeIndex CurrentNodeIndex, const FOctreeNodeContext& NodeContext, const AABB& BoxBounds, const IterateFunc& Func)
			FindNearbyElementsInternal(0, RootNodeContext, AABB(Position, math::float3(0),false), Func);
		}


		/**
		 * Adds an element to the octree.
		 * @param Element - The element to add.
		 */
		inline void AddElement(ElementType& Element)
		{
			ElementArrayType TempElementStorage;
			const AABB ElementBounds(OctreeSemantics::GetBoundingBox(Element));
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

			if (ElementId.ElementIndex < TreeElements[ElementId.NodeIndex].size())
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
				if (TreeElements[CollapseNodeIndex].size() < (int32_t)TreeNodes[CollapseNodeIndex].InclusiveNumElements)
				{
					ElementArrayType TempElementStorage;
					TempElementStorage.Reserve(TreeNodes[CollapseNodeIndex].InclusiveNumElements);
					// Gather the elements contained in this node and its children.
					CollapseNodesInternal(CollapseNodeIndex, TempElementStorage);
					TreeElements[CollapseNodeIndex] = MoveTemp(TempElementStorage);

					for (int ElementIndex = 0; ElementIndex < TreeElements[CollapseNodeIndex].size(); ElementIndex++)
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
			TreeNodes.resize(1);
			TreeElements.resize(1);
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
		std::vector<ElementType> GetElementsForNode(FNodeIndex NodeIndex) const
		{
			return TreeElements[NodeIndex];
		}

		/** Writes stats for the octree to the log. */
		void DumpStats() const
		{
			//int32_t NumNodes = 0;
			//int32_t NumLeaves = 0;
			//int32_t NumElements = 0;
			//int32_t MaxElementsPerNode = 0;
			//std::vector<int32_t> NodeElementDistribution;

			//FindNodesWithPredicateInternal(INDEX_NONE, 0, RootNodeContext, [](FNodeIndex /*ParentNodeIndex*/, FNodeIndex /*NodeIndex*/, const AABB&) { return true; }, [&, this](FNodeIndex /*ParentNodeIndex*/, FNodeIndex NodeIndex, const AABB&)
			//	{
			//		const int32_t CurrentNodeElementCount = GetElementsForNode(NodeIndex).Num();

			//		NumNodes++;
			//		if (TreeNodes[NodeIndex].IsLeaf())
			//		{
			//			NumLeaves++;
			//		}

			//		NumElements += CurrentNodeElementCount;
			//		MaxElementsPerNode = Math::Max(MaxElementsPerNode, CurrentNodeElementCount);

			//		if (CurrentNodeElementCount >= NodeElementDistribution.Num())
			//		{
			//			for (size_t i = 0; i < CurrentNodeElementCount - NodeElementDistribution.size() + 1); i++)
			//			{
			//			NodeElementDistribution.push_back(0);
			//			}
			//			//NodeElementDistribution.AddZeroed(CurrentNodeElementCount - NodeElementDistribution.Num() + 1);
			//		}
			//		NodeElementDistribution[CurrentNodeElementCount]++;
			//	});

			///*		GE_CORE_INFO("Octree overview:");
			//		GE_CORE_INFO("\t%i nodes"), NumNodes);
			//		GE_CORE_INFO("\t%i leaves", NumLeaves);
			//		GE_CORE_INFO("\t%i elements", NumElements);
			//		GE_CORE_INFO("\t%i >= elements per node", MaxElementsPerNode);
			//		GE_CORE_INFO("Octree node element distribution:");*/
			//for (int32_t i = 0; i < NodeElementDistribution.size(); i++)
			//{
			//	if (NodeElementDistribution[i] > 0)
			//	{
			//		GE_CORE_INFO("\tElements: %3i, Nodes: %3i", i, NodeElementDistribution[i]);
			//	}
			//}
		}

		SIZE_T GetSizeBytes() const
		{
			/*SIZE_T TotalSizeBytes = TreeNodes.GetAllocatedSize();
			TotalSizeBytes += TreeElements.GetAllocatedSize();
			TotalSizeBytes += TreeNodes[0].InclusiveNumElements * sizeof(ElementType);
			return TotalSizeBytes;*/
			return 0;
		}

		double GetNodeLevelExtent(int32_t Level) const
		{
			const int32_t ClampedLevel = math::clamp<int32_t>(Level, 0, OctreeSemantics::MaxNodeDepth);
			return RootNodeContext.Bounds.m_Extent.x * math::pow((1.0f + 1.0f / (double)FOctreeNodeContext::LoosenessDenominator) / 2.0f, double(ClampedLevel));
		}

		AABB GetRootBounds() const
		{
			return RootNodeContext.Bounds;
		}

		void ShrinkElements()
		{
			for (ElementArrayType& Elements : TreeElements)
			{
				//TODO
				//Elements.Shrink();
			}
		}

		/**
		 * Apply an arbitrary offset to all elements in the tree
		 * InOffset - offset to apply
		 * bGlobalOctree - hint that this octree is used as a boundless global volume,
		 *  so only content will be shifted but not origin of the octree
		 */
		void ApplyOffset(const math::float3& InOffset, bool bGlobalOctree = false)
		{
			ElementArrayType TempElementStorage;
			TempElementStorage.Reserve(TreeNodes[0].InclusiveNumElements);

			//collect all elements
			CollapseNodesInternal(0, TempElementStorage);
			assert(TreeNodes[0].IsLeaf());
			Destroy();

			if (!bGlobalOctree)
			{
				RootNodeContext.Bounds.m_Center += math::float3(InOffset, 0.0f);
			}

			// Offset & Add all elements from a saved nodes to a new empty octree
			for (ElementType& Element : TempElementStorage)
			{
				OctreeSemantics::ApplyOffset(Element, InOffset);
				AddElement(Element);
			}
		}

		/** Initialization constructor. */
		Octree(const AABB& boundingbox)
			: RootNodeContext(boundingbox, 0, 0)
			, MinLeafExtent(boundingbox.GetExtent().x * std::pow((1.0f + 1.0f / (double)FOctreeNodeContext::LoosenessDenominator) / 2.0f, double(OctreeSemantics::MaxNodeDepth)))
		{
			TreeNodes.push_back(FNode());
			TreeElements.push_back(std::vector<ElementType>());
		}

		/** DO NOT USE. This constructor is for internal usage only for hot-reload purposes. */
		Octree()
		{
			TreeNodes.push_back(FNode());
			TreeElements.push_back(std::vector<ElementType>());
			/*TreeNodes.AddDefaulted();
			TreeElements.AddDefaulted();*/
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
		void SetOctreeSemanticsElementId(ElementType& Element, FOctreeElementId2 Id)
		{
			Semantics::SetElementId(Element, Id);
			////TODO::
			//if constexpr (TModels_V<COctreeSemanticsV2, Semantics>)
			//{
			//	Semantics::SetElementId(static_cast<typename Semantics::FOctree&>(*this), Element, Id);
			//}
			//else
			//{
				
			//}
		}

	protected:
		// redirects SetElementId call to the proper implementation
		void SetElementId(ElementType& Element, FOctreeElementId2 Id)
		{
			SetOctreeSemanticsElementId<OctreeSemantics>(Element, Id);
		}
	};




}
