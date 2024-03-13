#pragma once
/**
 * Ray trace the Global Distance Field, compute hit point and hit normal, and fetch lighting from Voxel Lighting volume
 */
void ConeTraceVoxels(
	FConeTraceInput TraceInput,
	inout FConeTraceResult OutResult)
{
	FGlobalSDFTraceResult SDFTraceResult;

	// Trace SDF ray
	{
		FGlobalSDFTraceInput SDFTraceInput = SetupGlobalSDFTraceInput(TraceInput.ConeOrigin, TraceInput.ConeDirection, TraceInput.MinTraceDistance, TraceInput.MaxTraceDistance, TraceInput.SDFStepFactor, TraceInput.VoxelStepFactor);
		SDFTraceInput.bExpandSurfaceUsingRayTimeInsteadOfMaxDistance = TraceInput.bExpandSurfaceUsingRayTimeInsteadOfMaxDistance;
		SDFTraceInput.InitialMaxDistance = TraceInput.InitialMaxDistance;

		SDFTraceResult = RayTraceGlobalDistanceField(SDFTraceInput);
	}

	float4 LightingAndAlpha = float4(0, 0, 0, 1);

	if (GlobalSDFTraceResultIsHit(SDFTraceResult))
	{
		float3 SampleWorldPosition = TraceInput.ConeOrigin + TraceInput.ConeDirection * SDFTraceResult.HitTime;

		uint VoxelClipmapIndex = 0;
		float3 VoxelClipmapCenter = ClipmapWorldCenter[VoxelClipmapIndex].xyz;
		float3 VoxelClipmapExtent = ClipmapWorldSamplingExtent[VoxelClipmapIndex].xyz;

		bool bOutsideValidRegion = any(SampleWorldPosition > VoxelClipmapCenter + VoxelClipmapExtent || SampleWorldPosition < VoxelClipmapCenter - VoxelClipmapExtent);

		// Find the voxel clipmap that matches the cone width at the current step
		while (bOutsideValidRegion && VoxelClipmapIndex + 1 < NumClipmapLevels)
		{
			VoxelClipmapIndex++;
			VoxelClipmapCenter = ClipmapWorldCenter[VoxelClipmapIndex].xyz;
			VoxelClipmapExtent = ClipmapWorldSamplingExtent[VoxelClipmapIndex].xyz;
			bOutsideValidRegion = any(SampleWorldPosition > VoxelClipmapCenter + VoxelClipmapExtent || SampleWorldPosition < VoxelClipmapCenter - VoxelClipmapExtent);
		}

		LightingAndAlpha.xyzw = 0.0f;

		if (!bOutsideValidRegion)
		{
			float3 DistanceFieldGradient = -TraceInput.ConeDirection;

			float3 ClipmapVolumeUV = ComputeGlobalUV(SampleWorldPosition, SDFTraceResult.HitClipmapIndex);
			uint PageIndex = GetGlobalDistanceFieldPage(ClipmapVolumeUV, SDFTraceResult.HitClipmapIndex);

			if (PageIndex < GLOBAL_DISTANCE_FIELD_INVALID_PAGE_ID)
			{
				float3 PageUV = ComputeGlobalDistanceFieldPageUV(ClipmapVolumeUV, PageIndex);
				DistanceFieldGradient = GlobalDistanceFieldPageCentralDiff(PageUV);
			}

			float DistanceFieldGradientLength = length(DistanceFieldGradient);
			float3 SampleNormal = DistanceFieldGradientLength > 0.001 ? DistanceFieldGradient / DistanceFieldGradientLength : -TraceInput.ConeDirection;
			float4 StepLighting = SampleVoxelLighting(SampleWorldPosition, -SampleNormal, VoxelClipmapIndex);

			StepLighting.xyz = StepLighting.xyz * (1.0f / max(StepLighting.w, 0.1));

			float VoxelSelfLightingBias = 1.0f;
			if (TraceInput.bExpandSurfaceUsingRayTimeInsteadOfMaxDistance)
			{
				// For diffuse rays prefer to over occlude instead of leaking
				VoxelSelfLightingBias = smoothstep(1.5 * ClipmapVoxelSizeAndRadius[VoxelClipmapIndex].w, 2.0 * ClipmapVoxelSizeAndRadius[VoxelClipmapIndex].w, SDFTraceResult.HitTime);
			}

			LightingAndAlpha.xyz = StepLighting.xyz * VoxelSelfLightingBias;
		}
	}

	LightingAndAlpha = FadeOutVoxelConeTraceMinTransparency(LightingAndAlpha);

	OutResult = (FConeTraceResult)0;
#if !VISIBILITY_ONLY_TRACE
	OutResult.Lighting = LightingAndAlpha.rgb;
#endif
	OutResult.Transparency = LightingAndAlpha.a;
	OutResult.NumSteps = SDFTraceResult.TotalStepsTaken;
	OutResult.OpaqueHitDistance = GlobalSDFTraceResultIsHit(SDFTraceResult) ? SDFTraceResult.HitTime : TraceInput.MaxTraceDistance;
	OutResult.ExpandSurfaceAmount = SDFTraceResult.ExpandSurfaceAmount;
}