#pragma once
#include "glm/glm.hpp"
#include "BlackPearl/AABB/AABB.h"
#include "LumenCard.h"

namespace BlackPearl {

	namespace Lumen
	{
		constexpr uint32_t NumAxisAlignedDirections = 6;

		//void UpdateCardSceneBuffer(FRDGBuilder& GraphBuilder, const FSceneViewFamily& ViewFamily, FScene* Scene);
	};

	//°üº¬¶à¸öLumenCard 
	class LumenMeshCards
	{
	public:
		void Initialize(
			const glm::mat4& InLocalToWorld,
			const AABB& InLocalBounds,
			int32_t InPrimitiveGroupIndex,
			uint32_t InFirstCardIndex,
			uint32_t InNumCards,
			bool InFarField,
			bool InHeightfield,
			bool InEmissiveLightSource)
		{
			PrimitiveGroupIndex = InPrimitiveGroupIndex;

			LocalBounds = InLocalBounds;
			SetTransform(InLocalToWorld);
			FirstCardIndex = InFirstCardIndex;
			NumCards = InNumCards;
			bFarField = InFarField;
			bHeightfield = InHeightfield;
			bEmissiveLightSource = InEmissiveLightSource;
		}

		void UpdateLookup(const std::vector<LumenCard>& Cards);

		void SetTransform(const glm::mat4& InLocalToWorld);

		AABB GetWorldSpaceBounds() const
		{
			AABB worldSpaceBounds = LocalBounds;
			worldSpaceBounds.UpdateTransform(LocalToWorld);
			return worldSpaceBounds;
		}

		glm::mat4 LocalToWorld;
		glm::vec3 LocalToWorldScale;
		glm::mat4 WorldToLocalRotation;
		AABB LocalBounds;

		int32_t PrimitiveGroupIndex = -1;
		bool bFarField = false;
		bool bHeightfield = false;
		bool bEmissiveLightSource = false;

		uint32_t FirstCardIndex = 0;
		uint32_t NumCards = 0;
		uint32_t CardLookup[Lumen::NumAxisAlignedDirections];
	};

}

