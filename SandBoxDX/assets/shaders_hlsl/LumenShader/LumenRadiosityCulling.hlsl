#pragma once

StructuredBuffer<uint> CardPageIndexAllocator;
StructuredBuffer<uint> CardPageIndexData;

RWStructuredBuffer<uint> RWCardTileAllocator;
RWStructuredBuffer<uint> RWCardTileData;

/**
 * Build a list of radiosity tiles
 */
[numthreads(THREADGROUP_SIZE, THREADGROUP_SIZE, 1)]
void BuildRadiosityTilesCS(
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

				uint NextTileIndex = 0;
				InterlockedAdd(RWCardTileAllocator[0], 1, NextTileIndex);
				RWCardTileData[NextTileIndex] = PackCardTileData(CardTile);
			}
		}
	}
}
