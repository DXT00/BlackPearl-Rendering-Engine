#pragma once

RWBuffer<uint> RWIndirectArgs;

void SetIndirectArgs(uint ArgIndex, uint NumThreads)
{
	uint BaseOffset = ArgIndex * 3;
	RWIndirectArgs[BaseOffset + 0] = (NumThreads + THREADGROUP_SIZE - 1) / THREADGROUP_SIZE;
	RWIndirectArgs[BaseOffset + 1] = 1;
	RWIndirectArgs[BaseOffset + 2] = 1;
}

void SetHardwareRayTracingIndirectArgs(uint ArgIndex, uint NumThreads)
{
	uint BaseOffset = ArgIndex * 3;
	int3 DispatchDimension = GetRayTracingThreadCountWrapped(NumThreads, THREADGROUP_SIZE);
	RWIndirectArgs[BaseOffset + 0] = DispatchDimension.x;
	RWIndirectArgs[BaseOffset + 1] = DispatchDimension.y;
	RWIndirectArgs[BaseOffset + 2] = DispatchDimension.z;
}

void LumenRadiosityIndirectArgsCS(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	if (DispatchThreadId.x == 0)
	{
		// ERadiosityIndirectArgs::ThreadPerTrace
		uint ThreadPerTrace = CardTileAllocator[0] * RadiosityTileSize * RadiosityTileSize * NumTracesPerProbe;
		SetIndirectArgs(0, ThreadPerTrace);

		// ERadiosityIndirectArgs::ThreadPerProbe
		SetIndirectArgs(1, CardTileAllocator[0] * RadiosityTileSize * RadiosityTileSize);

		// ERadiosityIndirectArgs::ThreadPerRadiosityTexel
		SetIndirectArgs(2, CardTileAllocator[0] * CARD_TILE_SIZE * CARD_TILE_SIZE);

		// ERadiosityIndirectArgs::HardwareRayTracingThreadPerTrace
		SetHardwareRayTracingIndirectArgs(3, ThreadPerTrace);
	}
}

[numthreads(THREADGROUP_SIZE, 1, 1)]
void LumenRadiosityDistanceFieldTracingCS(
	uint DispatchThreadId : SV_DispatchThreadID)
{
	uint CardTileIndex;
	uint2 CoordInCardTile;
	uint2 TraceTexelCoord;
	UnswizzleTexelTraceCoords(DispatchThreadId, CardTileIndex, CoordInCardTile, TraceTexelCoord);

	FRadiosityTexel RadiosityTexel = GetRadiosityTexelFromCardTile(CardTileIndex, CoordInCardTile);

	if (RadiosityTexel.bInsideAtlas)
	{
		float3 Radiance = 0.0f;
		float TraceHitDistance = MaxTraceDistance;

		if (RadiosityTexel.Opacity > 0.0f)
		{
			float3 WorldPosition = RadiosityTexel.WorldPosition;
			float3 WorldNormal = RadiosityTexel.WorldNormal;

			float3 WorldRayDirection;
			float ConeHalfAngle;
			float PDF;
			GetRadiosityRay(RadiosityTexel, RadiosityTexel.CardCoord >> ProbeSpacingInRadiosityTexelsDivideShift, TraceTexelCoord, WorldRayDirection, ConeHalfAngle, PDF);

			//#lumen_todo: derive bias from texel world size
			WorldPosition += WorldNormal * SurfaceBias;

			float VoxelTraceStartDistance = CalculateVoxelTraceStartDistance(MinTraceDistance, MaxTraceDistance, MaxMeshSDFTraceDistance, false);

			//#lumen_todo: derive bias from texel world size
			float3 SamplePosition = WorldPosition + SurfaceBias * WorldRayDirection;

			FConeTraceInput TraceInput;
			TraceInput.Setup(SamplePosition, WorldRayDirection, ConeHalfAngle, 0, MinTraceDistance, MaxTraceDistance, 1);
			TraceInput.VoxelStepFactor = VoxelStepFactor;
			TraceInput.VoxelTraceStartDistance = VoxelTraceStartDistance;
			TraceInput.SDFStepFactor = 1;

			FConeTraceResult TraceResult = (FConeTraceResult)0;
			TraceResult.Transparency = 1;

#if TRACE_GLOBAL_SDF
			{
				ConeTraceVoxels(TraceInput, TraceResult);
			}
#endif

			if (TraceResult.Transparency < 0.5f)
			{
				Radiance = TraceResult.Lighting;

				// Recalculate TraceHitDistance to incorporate biases
				//@todo - Self intersection from grazing angle traces breaks probe occlusion
				float3 HitPosition = SamplePosition + WorldRayDirection * (TraceResult.OpaqueHitDistance + TraceResult.ExpandSurfaceAmount);
				TraceHitDistance = length(RadiosityTexel.WorldPosition - HitPosition);
			}
			else
			{
				Radiance = EvaluateSkyRadiance(WorldRayDirection, tan(ConeHalfAngle));
			}

			float MaxLighting = max3(Radiance.x, Radiance.y, Radiance.z);

			if (MaxLighting > MaxRayIntensity * View.OneOverPreExposure)
			{
				Radiance *= MaxRayIntensity * View.OneOverPreExposure / MaxLighting;
			}
		}

		FCardTileData CardTile = UnpackCardTileData(CardTileData[CardTileIndex]);
		FLumenCardPageData CardPage = GetLumenCardPageData(CardTile.CardPageIndex);
		uint2 RadiosityProbeTracingAtlasCoord = GetRadiosityProbeAtlasCoord(CardPage, CardTile, CoordInCardTile) * HemisphereProbeResolution + TraceTexelCoord;
		RWTraceRadianceAtlas[RadiosityProbeTracingAtlasCoord] = Radiance;

		if (UseProbeOcclusion > 0)
		{
			RWTraceHitDistanceAtlas[RadiosityProbeTracingAtlasCoord] = TraceHitDistance;
		}
	}
}
