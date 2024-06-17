#pragma once
// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	GlobalDistanceFieldUtils.ush
=============================================================================*/

#define TWO_SIDED_BAND_SIZE 4.0f
#define DEFAULT_BAND_SIZE 4.0f

#ifndef GLOBALSDF_USE_COVERAGE_BASED_EXPAND
#define GLOBALSDF_USE_COVERAGE_BASED_EXPAND 1
#endif

uint ComputeGlobalDistanceFieldClipmapIndex(float3 WorldPosition)
{
	uint FoundClipmapIndex = 0;

	for (uint ClipmapIndex = 0; ClipmapIndex < NumGlobalSDFClipmaps; ClipmapIndex++)
	{
		float DistanceFromClipmap = ComputeDistanceFromBoxToPointInside(GlobalVolumeCenterAndExtent[ClipmapIndex].xyz, GlobalVolumeCenterAndExtent[ClipmapIndex].www, WorldPosition);

		if (DistanceFromClipmap > GlobalVolumeCenterAndExtent[ClipmapIndex].w * GlobalVolumeTexelSize)
		{
			FoundClipmapIndex = ClipmapIndex;
			break;
		}
	}

	return FoundClipmapIndex;
}

struct FGlobalSDFTraceInput
{
	float3 WorldRayStart;
	float3 WorldRayDirection;
	float MinTraceDistance;
	float MaxTraceDistance;
	float StepFactor;
	float MinStepFactor;
	// The SDF surface is expanded to reduce leaking through thin surfaces, especially foliage meshes with bGenerateDistanceFieldAsIfTwoSided
	// Expanding as RayTime increases errors on the side of over-occlusion, especially at grazing angles, which can be desirable for diffuse GI.
	// Expanding as MaxDistance increases has less incorrect self-intersection which is desirable for reflections rays.
	bool bExpandSurfaceUsingRayTimeInsteadOfMaxDistance;
	float InitialMaxDistance;

	// Bias relative to the clipmap's voxel size
	float VoxelSizeRelativeBias;
	// Ray length bias relative to the clipmap's voxel size
	float VoxelSizeRelativeRayEndBias;
};

FGlobalSDFTraceInput SetupGlobalSDFTraceInput(float3 InWorldRayStart, float3 InWorldRayDirection, float InMinTraceDistance, float InMaxTraceDistance, float InStepFactor, float InMinStepFactor)
{
	FGlobalSDFTraceInput TraceInput;
	TraceInput.WorldRayStart = InWorldRayStart;
	TraceInput.WorldRayDirection = InWorldRayDirection;
	TraceInput.MinTraceDistance = InMinTraceDistance;
	TraceInput.MaxTraceDistance = InMaxTraceDistance;
	TraceInput.StepFactor = InStepFactor;
	TraceInput.MinStepFactor = InMinStepFactor;
	TraceInput.bExpandSurfaceUsingRayTimeInsteadOfMaxDistance = true;
	TraceInput.InitialMaxDistance = 0;
	TraceInput.VoxelSizeRelativeBias = 0.0f;
	TraceInput.VoxelSizeRelativeRayEndBias = 0.0f;
	return TraceInput;
}

struct FGlobalSDFTraceResult
{
	float HitTime;
	uint HitClipmapIndex;
	uint TotalStepsTaken;

	// Amount the surface was expanded at the hit
	float ExpandSurfaceAmount;
};

bool GlobalSDFTraceResultIsHit(FGlobalSDFTraceResult TraceResult)
{
	return TraceResult.HitTime >= 0.0f;
}

FGlobalSDFTraceResult RayTraceGlobalDistanceField(FGlobalSDFTraceInput TraceInput)
{
	FGlobalSDFTraceResult TraceResult;
	TraceResult.HitTime = -1.0f;
	TraceResult.HitClipmapIndex = 0;
	TraceResult.TotalStepsTaken = 0;
	TraceResult.ExpandSurfaceAmount = 0;

	uint MinClipmapIndex = ComputeGlobalDistanceFieldClipmapIndex(TraceInput.WorldRayStart + TraceInput.MinTraceDistance * TraceInput.WorldRayDirection);
	float MaxDistance = TraceInput.InitialMaxDistance;
	float MinRayTime = TraceInput.MinTraceDistance;

	LOOP
		for (uint ClipmapIndex = MinClipmapIndex; ClipmapIndex < NumGlobalSDFClipmaps && TraceResult.HitTime < 0.0f; ++ClipmapIndex)
		{
			float ClipmapVoxelExtent = GlobalVolumeCenterAndExtent[ClipmapIndex].w * GlobalVolumeTexelSize;
			float MinStepSize = TraceInput.MinStepFactor * ClipmapVoxelExtent;
			float ExpandSurfaceDistance = ClipmapVoxelExtent;
			float ClipmapRayBias = ClipmapVoxelExtent * TraceInput.VoxelSizeRelativeBias;
			float ClipmapRayLength = TraceInput.MaxTraceDistance - ClipmapVoxelExtent * TraceInput.VoxelSizeRelativeRayEndBias;

			float3 GlobalVolumeCenter = GlobalVolumeCenterAndExtent[ClipmapIndex].xyz;
			float GlobalVolumeExtent = GlobalVolumeCenterAndExtent[ClipmapIndex].w - ClipmapVoxelExtent;
			float3 WorldRayEnd = TraceInput.WorldRayStart + TraceInput.WorldRayDirection * ClipmapRayLength;
			float2 IntersectionTimes = LineBoxIntersect(TraceInput.WorldRayStart, WorldRayEnd, GlobalVolumeCenter - GlobalVolumeExtent.xxx, GlobalVolumeCenter + GlobalVolumeExtent.xxx);
			IntersectionTimes.xy *= ClipmapRayLength;
			IntersectionTimes.x = max(IntersectionTimes.x, MinRayTime);
			IntersectionTimes.x = max(IntersectionTimes.x, ClipmapRayBias);

			if (IntersectionTimes.x < IntersectionTimes.y)
			{
				// Update the trace start for the next clipmap
				MinRayTime = IntersectionTimes.y;

				float SampleRayTime = IntersectionTimes.x;
				const float ClipmapInfluenceRange = GLOBAL_DISTANCE_FIELD_INFLUENCE_RANGE_IN_VOXELS * 2.0f * GlobalVolumeCenterAndExtent[ClipmapIndex].w * GlobalVolumeTexelSize;

				uint StepIndex = 0;
				const uint MaxSteps = 256;

				LOOP
					for (; StepIndex < MaxSteps; ++StepIndex)
					{
						float3 SampleWorldPosition = TraceInput.WorldRayStart + TraceInput.WorldRayDirection * SampleRayTime;

						float3 ClipmapVolumeUV = ComputeGlobalUV(SampleWorldPosition, ClipmapIndex);
						float3 MipUV = ComputeGlobalMipUV(SampleWorldPosition, ClipmapIndex);

						float DistanceFieldMipValue = Texture3DSampleLevel(GlobalDistanceFieldMipTexture, GlobalTrilinearClampedSampler, MipUV, 0).x;
						float DistanceField = DecodeGlobalDistanceFieldPageDistance(DistanceFieldMipValue, GlobalDistanceFieldMipFactor * ClipmapInfluenceRange);
						float Coverage = 1;
						uint PageIndex = GetGlobalDistanceFieldPage(ClipmapVolumeUV, ClipmapIndex);

						if (PageIndex < GLOBAL_DISTANCE_FIELD_INVALID_PAGE_ID && DistanceFieldMipValue < GlobalDistanceFieldMipTransition)
						{
#if GLOBALSDF_USE_COVERAGE_BASED_EXPAND
							float3 PageUV;
							float3 CoveragePageUV;
							ComputeGlobalDistanceFieldPageUV(ClipmapVolumeUV, PageIndex, PageUV, CoveragePageUV);
							Coverage = Texture3DSampleLevel(GlobalDistanceFieldCoverageAtlasTexture, GlobalTrilinearWrappedSampler, CoveragePageUV, 0).x;
#else
							float3 PageUV = ComputeGlobalDistanceFieldPageUV(ClipmapVolumeUV, PageIndex);
#endif
							float DistanceFieldValue = Texture3DSampleLevel(GlobalDistanceFieldPageAtlasTexture, GlobalTrilinearWrappedSampler, PageUV, 0).x;
							DistanceField = DecodeGlobalDistanceFieldPageDistance(DistanceFieldValue, ClipmapInfluenceRange);
						}

						MaxDistance = max(DistanceField, MaxDistance);

						float ExpandSurfaceTime = TraceInput.bExpandSurfaceUsingRayTimeInsteadOfMaxDistance ? SampleRayTime - ClipmapRayBias : MaxDistance;
						float ExpandSurfaceScale = lerp(UncoveredExpandSurfaceScale, FullyCoveredExpandSurfaceScale, Coverage);

						const float ExpandSurfaceFalloff = 2.0f * ExpandSurfaceDistance;
						const float ExpandSurfaceAmount = ExpandSurfaceDistance * saturate(ExpandSurfaceTime / ExpandSurfaceFalloff) * ExpandSurfaceScale;

						if (DistanceField < ExpandSurfaceAmount)
						{
							TraceResult.HitTime = max(SampleRayTime + DistanceField - ExpandSurfaceAmount, 0.0f);
							TraceResult.HitClipmapIndex = ClipmapIndex;
							TraceResult.ExpandSurfaceAmount = ExpandSurfaceAmount;
							break;
						}

						float LocalMinStepSize = MinStepSize * lerp(UncoveredMinStepScale, 1.0f, Coverage);
						float StepDistance = max(DistanceField * TraceInput.StepFactor, LocalMinStepSize);
						SampleRayTime += StepDistance;

						// Terminate the trace if we went past the end of the ray or achieved enough occlusion
						if (SampleRayTime > IntersectionTimes.y || TraceResult.HitTime >= 0.0f)
						{
							break;
						}
					}

				TraceResult.TotalStepsTaken += StepIndex;
			}
		}

	return TraceResult;
}