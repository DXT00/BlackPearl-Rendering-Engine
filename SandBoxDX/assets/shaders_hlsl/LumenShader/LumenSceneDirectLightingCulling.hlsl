// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	LumenCardTiles.ush
=============================================================================*/

#pragma once

#define CARD_TILE_SIZE 8
#define NULL_PACKED_CARD_TILE 0xFFFFFFFF

struct FCardTileData
{
	uint CardPageIndex;
	uint2 TileCoord;
};

FCardTileData UnpackCardTileData(uint PackedTile)
{
	FCardTileData TileData;
	TileData.CardPageIndex = PackedTile & 0xFFFFFF;
	TileData.TileCoord.x = (PackedTile >> 24) & 0xF;
	TileData.TileCoord.y = (PackedTile >> 28) & 0xF;
	return TileData;
}

uint PackCardTileData(FCardTileData CardTile)
{
	uint PackedTile = CardTile.CardPageIndex;
	PackedTile |= (CardTile.TileCoord.x << 24);
	PackedTile |= (CardTile.TileCoord.y << 28);
	return PackedTile;
}

struct FLightTile
{
	uint LightIndex;
	uint CardPageIndex;
	uint2 TileCoord;
};

FLightTile UnpackLightTile(uint2 PackedTile)
{
	FLightTile Tile;
	Tile.LightIndex = PackedTile.x;
	Tile.CardPageIndex = PackedTile.y & 0xFFFFFF;
	Tile.TileCoord.x = (PackedTile.y >> 24) & 0xF;
	Tile.TileCoord.y = (PackedTile.y >> 28) & 0xF;
	return Tile;
}

uint2 PackLightTile(FLightTile Tile)
{
	uint2 PackedTile;
	PackedTile.x = Tile.LightIndex;
	PackedTile.y = Tile.CardPageIndex;
	PackedTile.y |= (Tile.TileCoord.x << 24);
	PackedTile.y |= (Tile.TileCoord.y << 28);
	return PackedTile;
}

StructuredBuffer<uint> CardPageIndexAllocator;
StructuredBuffer<uint> CardPageIndexData;
RWStructuredBuffer<uint> RWCardTileAllocator;
RWStructuredBuffer<uint> RWCardTiles;

/**
 * Splice card pages into N card tiles
 */
[numthreads(THREADGROUP_SIZE, THREADGROUP_SIZE, 1)]
void SpliceCardPagesIntoTilesCS(
	uint3 GroupId : SV_GroupID,
	uint3 DispatchThreadId : SV_DispatchThreadID,
	uint3 GroupThreadId : SV_GroupThreadID)
{
	// One thread per tile
	uint LinearLightTileOffset = (GroupId.x % 4);
	uint IndexInIndexBuffer = GroupId.x / 4;

	uint2 TileCoord;
	TileCoord.x = (LinearLightTileOffset % 2) * 8 + GroupThreadId.x;
	TileCoord.y = (LinearLightTileOffset / 2) * 8 + GroupThreadId.y;

	if (IndexInIndexBuffer < CardPageIndexAllocator[0])
	{
		uint CardPageIndex = CardPageIndexData[IndexInIndexBuffer];
		FLumenCardPageData CardPage = GetLumenCardPageData(CardPageIndex);
		if (CardPage.CardIndex >= 0)
		{
			FLumenCardData Card = GetLumenCardData(CardPage.CardIndex);

			const uint2 SizeInTiles = CardPage.SizeInTexels / CARD_TILE_SIZE;

			if (all(TileCoord < SizeInTiles))
			{
				FCardTileData CardTile;
				CardTile.CardPageIndex = CardPageIndex;
				CardTile.TileCoord = TileCoord;

				uint CardTileIndex = 0;
				InterlockedAdd(RWCardTileAllocator[0], 1, CardTileIndex);
				RWCardTiles[CardTileIndex] = PackCardTileData(CardTile);
			}
		}
	}
}

StructuredBuffer<uint> CardTileAllocator;
StructuredBuffer<uint> CardTiles;
RWBuffer<uint> RWDispatchCardTilesIndirectArgs;

[numthreads(THREADGROUP_SIZE, 1, 1)]
void InitializeCardTileIndirectArgsCS(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	if (DispatchThreadId.x == 0)
	{
		uint NumCardTiles = CardTileAllocator[0];

		// One thread per card tile
		RWDispatchCardTilesIndirectArgs[0] = (NumCardTiles + 63) / 64;
		RWDispatchCardTilesIndirectArgs[1] = 1;
		RWDispatchCardTilesIndirectArgs[2] = 1;
	}
}
uint MaxLightsPerTile;
uint NumLights;

/**
 * Pick N most important lights per tile in page selected to update to update this frame, and output a list of light tiles
 */
//对于每个CardTile，选择影响到它的N个光源
[numthreads(THREADGROUP_SIZE, 1, 1)]
void BuildLightTilesCS(
	uint3 GroupId : SV_GroupID,
	uint3 DispatchThreadId : SV_DispatchThreadID,
	uint3 GroupThreadId : SV_GroupThreadID)
{
	// One thread per tile
	uint CardTileIndex = DispatchThreadId.x;

	if (CardTileIndex < CardTileAllocator[0])
	{
		FCardTileData CardTile = UnpackCardTileData(CardTiles[CardTileIndex]);
		FLumenCardPageData CardPage = GetLumenCardPageData(CardTile.CardPageIndex);

		if (CardPage.CardIndex >= 0)
		{
			FLumenCardData Card = GetLumenCardData(CardPage.CardIndex);

			const uint2 SizeInTiles = CardPage.SizeInTexels / CARD_TILE_SIZE;
			float2 UVMin = float2(CardTile.TileCoord) / SizeInTiles;
			float2 UVMax = float2(CardTile.TileCoord + 1) / SizeInTiles;

			float SwapY = UVMin.y;
			UVMin.y = 1.0f - UVMax.y;
			UVMax.y = 1.0f - SwapY;

			// Loop over lights to select N most important lights
			uint NumCulledLightsPerTile = 0;
			for (uint LightIndex = 0; LightIndex < NumLights; ++LightIndex)
			{
				FLumenLight LumenLight = LoadLumenLight(LightIndex);

				bool bLightAffectsCard = DoesLightAffectCardPageUVRange(LumenLight, CardPage, Card, UVMin, UVMax);
				if (bLightAffectsCard)
				{
					// Write light tile
					FLightTile LightTile;
					LightTile.LightIndex = LightIndex;
					LightTile.CardPageIndex = CardTile.CardPageIndex;
					LightTile.TileCoord = CardTile.TileCoord;

					// Add to global light tile array
					uint CardTileIndex = 0;
					InterlockedAdd(RWLightTileAllocator[0], 1, CardTileIndex);
					RWLightTiles[CardTileIndex] = PackLightTile(LightTile);

					InterlockedAdd(RWLightTileAllocatorPerLight[LightIndex], 1);

					// Stop when we reached the limit of cards per current tile
					++NumCulledLightsPerTile;
					if (NumCulledLightsPerTile >= MaxLightsPerTile)
					{
						break;
					}
				}
			}
		}
	}
}


StructuredBuffer<uint> LightTileAllocatorPerLight;
RWStructuredBuffer<uint> RWLightTileOffsetsPerLight;

/**
 * Prefix sum for card tile array compaction
 */
[numthreads(THREADGROUP_SIZE, 1, 1)]
void ComputeLightTileOffsetsPerLightCS(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	if (DispatchThreadId.x == 0)
	{
		uint LightOffset = 0;
		for (uint LightIndex = 0; LightIndex < NumLights; ++LightIndex)
		{
			RWLightTileOffsetsPerLight[LightIndex] = LightOffset;
			LightOffset += LightTileAllocatorPerLight[LightIndex];
		}
	}
}
StructuredBuffer<uint> LightTileAllocatorPerLight;
RWStructuredBuffer<uint> RWLightTileOffsetsPerLight;

/**
 * Prefix sum for card tile array compaction
 */
[numthreads(THREADGROUP_SIZE, 1, 1)]
void ComputeLightTileOffsetsPerLightCS(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	if (DispatchThreadId.x == 0)
	{
		uint LightOffset = 0;
		for (uint LightIndex = 0; LightIndex < NumLights; ++LightIndex)
		{
			RWLightTileOffsetsPerLight[LightIndex] = LightOffset;
			LightOffset += LightTileAllocatorPerLight[LightIndex];
		}
	}
}

RWBuffer<uint> RWDispatchLightTilesIndirectArgs;
RWBuffer<uint> RWDrawTilesPerLightIndirectArgs;
RWBuffer<uint> RWDispatchTilesPerLightIndirectArgs;

uint VertexCountPerInstanceIndirect;

[numthreads(THREADGROUP_SIZE, 1, 1)]
void InitializeLightTileIndirectArgsCS(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	uint LightIndex = DispatchThreadId.x;

	// Global card tile array
	if (LightIndex == 0)
	{
		uint NumLightTiles = LightTileAllocator[0];

		// One group per tile
		RWDispatchLightTilesIndirectArgs[0] = NumLightTiles;
		RWDispatchLightTilesIndirectArgs[1] = 1;
		RWDispatchLightTilesIndirectArgs[2] = 1;

		// One thread per tile
		RWDispatchLightTilesIndirectArgs[3 + 0] = (NumLightTiles + 63) / 64;
		RWDispatchLightTilesIndirectArgs[3 + 1] = 1;
		RWDispatchLightTilesIndirectArgs[3 + 2] = 1;
	}

	// Per light card tile array
	if (LightIndex < NumLights)
	{
		uint NumLightTilesPerLight = LightTileAllocatorPerLight[LightIndex];

		// FRHIDispatchIndirectParameters
		RWDispatchTilesPerLightIndirectArgs[3 * LightIndex + 0] = NumLightTilesPerLight;
		RWDispatchTilesPerLightIndirectArgs[3 * LightIndex + 1] = 1;
		RWDispatchTilesPerLightIndirectArgs[3 * LightIndex + 2] = 1;

		// FRHIDrawIndirectParameters
		RWDrawTilesPerLightIndirectArgs[4 * LightIndex + 0] = VertexCountPerInstanceIndirect;
		RWDrawTilesPerLightIndirectArgs[4 * LightIndex + 1] = NumLightTilesPerLight;
		RWDrawTilesPerLightIndirectArgs[4 * LightIndex + 2] = 0;
		RWDrawTilesPerLightIndirectArgs[4 * LightIndex + 3] = 0;
	}
}

uint LightIndex;

void RasterizeToLightTilesVS(
	uint VertexId : SV_VertexID,
	uint InstanceId : SV_InstanceID,
	out FCardVSToPS CardInterpolants,
	out float4 OutPosition : SV_POSITION
)
{
	float2 TexCoord = float2(0.0f, 0.0f);
	TexCoord.x += VertexId == 1 || VertexId == 2 || VertexId == 4 ? 1.0f : 0.0f;
	TexCoord.y += VertexId == 2 || VertexId == 4 || VertexId == 5 ? 1.0f : 0.0f;

	uint LightTileIndex = InstanceId.x + LightTileOffsetsPerLight[LightIndex];
	uint NumLightTiles = LightTileAllocator[0];

	CardInterpolants = (FCardVSToPS)0;
	OutPosition = 0;

	if (LightTileIndex < NumLightTiles)
	{
		FLightTile LightTile = UnpackLightTile(LightTiles[LightTileIndex]);
		FLumenCardPageData CardPage = GetLumenCardPageData(LightTile.CardPageIndex);

		float2 AtlasUV = CardPage.PhysicalAtlasUVRect.xy + CardPage.PhysicalAtlasUVTexelScale * CARD_TILE_SIZE * (LightTile.TileCoord + TexCoord);
		float2 CardUV = CardPage.CardUVRect.xy + CardPage.CardUVTexelScale * CARD_TILE_SIZE * (LightTile.TileCoord + TexCoord);

		float2 ScreenPosition = float2(2.0f, -2.0f) * AtlasUV + float2(-1.0f, 1.0f);
		OutPosition = float4(ScreenPosition, 0, 1);

		CardInterpolants.AtlasUV = AtlasUV;
		CardInterpolants.IndirectLightingAtlasUV = float2(0.0f, 0.0f);
		CardInterpolants.CardUV = CardUV;
		CardInterpolants.CardTileIndex = LightTileIndex;
		CardInterpolants.CardPageIndex = LightTile.CardPageIndex;
	}
}
