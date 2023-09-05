#pragma once
#include "Lumen.h"
#include "BlackPearl/Scene/Scene.h"
namespace BlackPearl {
	class LumenSceneData;
	int32_t GLumenDirectLightingMaxLightsPerTile = 8;

	namespace Lumen {

		int32_t GLumenDirectLightingMaxLightsPerTile = 8;

		// Must match usf
		constexpr uint32_t PhysicalPageSize = 128;
		constexpr uint32_t VirtualPageSize = PhysicalPageSize - 1; // 0.5 texel border around page
		constexpr uint32_t MinCardResolution = 8;
		constexpr uint32_t MinResLevel = 3; // 2^3 = MinCardResolution
		constexpr uint32_t MaxResLevel = 11; // 2^11 = 2048 texels
		constexpr uint32_t SubAllocationResLevel = 7; // log2(PHYSICAL_PAGE_SIZE)
		constexpr uint32_t NumResLevels = MaxResLevel - MinResLevel + 1;
		constexpr uint32_t CardTileSize = 8;

		/*constexpr float MaxTracingEndDistanceFromCamera = 0.5f * UE_OLD_WORLD_MAX;
		constexpr float MaxTraceDistance = 0.5f * UE_OLD_WORLD_MAX;*/

		enum class ETracingPermutation
		{
			Cards,
			VoxelsAfterCards,
			Voxels,
			MAX
		};

		void DebugResetSurfaceCache();
		void DebugResetVoxelLighting();

		//bool UseMeshSDFTracing(const FSceneViewFamily& ViewFamily);
		//bool UseGlobalSDFTracing(const FSceneViewFamily& ViewFamily);
		//bool UseHeightfieldTracing(const FSceneViewFamily& ViewFamily, const LumenSceneData& LumenSceneData);
		bool UseHeightfieldTracingForVoxelLighting(const LumenSceneData& LumenSceneData);
		int32_t GetHeightfieldMaxTracingSteps();
		//float GetMaxTraceDistance(const FViewInfo& View);
		//bool AnyLumenHardwareRayTracingPassEnabled(const Scene* Scene, const FViewInfo& View);
		//bool AnyLumenHardwareInlineRayTracingPassEnabled(const Scene* Scene, const FViewInfo& View);
		bool IsSoftwareRayTracingSupported();
		//bool IsLumenFeatureAllowedForView(const Scene* Scene, const FSceneView& View, bool bSkipTracingDataCheck = false, bool bSkipProjectCheck = false);
		//bool ShouldVisualizeScene(const FSceneViewFamily& ViewFamily);
		//bool ShouldVisualizeHardwareRayTracing(const FSceneViewFamily& ViewFamily);
		//bool ShouldHandleSkyLight(const Scene* Scene, const FSceneViewFamily& ViewFamily);

		//void ExpandDistanceFieldUpdateTrackingBounds(const FSceneViewState* ViewState, DistanceField::FUpdateTrackingBounds& UpdateTrackingBounds);
		float GetDistanceSceneNaniteLODScaleFactor();

		bool ShouldUpdateLumenSceneViewOrigin();
		int32_t GetGlobalDFResolution();
		float GetGlobalDFClipmapExtent();
		float GetFirstClipmapWorldExtent();

		// Features
		//bool IsRadiosityEnabled(const FSceneViewFamily& ViewFamily);
		uint32_t GetRadiosityAtlasDownsampleFactor();

		// Surface cache
		bool IsSurfaceCacheFrozen();
		bool IsSurfaceCacheUpdateFrameFrozen();

		// Software ray tracing
		//bool UseVoxelLighting(const FSceneViewFamily& ViewFamily);

		// Hardware ray tracing
		bool UseHardwareRayTracing();
		//bool UseHardwareRayTracedSceneLighting(const FSceneViewFamily& ViewFamily);
		bool UseHardwareRayTracedDirectLighting();
		bool UseHardwareRayTracedReflections();
		bool UseHardwareRayTracedScreenProbeGather();
		bool UseHardwareRayTracedRadianceCache();
		//bool UseHardwareRayTracedRadiosity(const FSceneViewFamily& ViewFamily);
		//bool UseHardwareRayTracedVisualize(const FSceneViewFamily& ViewFamily);

		//bool ShouldRenderRadiosityHardwareRayTracing(const FSceneViewFamily& ViewFamily);
		//bool ShouldVisualizeHardwareRayTracing(const FSceneViewFamily& ViewFamily);

		int32_t GetMaxTranslucentSkipCount();

		bool UseHardwareInlineRayTracing();

		enum class EHardwareRayTracingLightingMode
		{
			LightingFromSurfaceCache = 0,
			EvaluateMaterial,
			EvaluateMaterialAndDirectLighting,
			EvaluateMaterialAndDirectLightingAndSkyLighting,
			MAX
		};
		//EHardwareRayTracingLightingMode GetHardwareRayTracingLightingMode(const FViewInfo& View);
		EHardwareRayTracingLightingMode GetRadianceCacheHardwareRayTracingLightingMode();

		enum class ESurfaceCacheSampling
		{
			AlwaysResidentPagesWithoutFeedback,
			AlwaysResidentPages,
			HighResPages,
		};

		const TCHAR* GetRayTracedLightingModeName(EHardwareRayTracingLightingMode LightingMode);
		const TCHAR* GetRayTracedNormalModeName(int NormalMode);
		float GetHardwareRayTracingPullbackBias();

		//bool UseFarField(const FSceneViewFamily& ViewFamily);
		float GetFarFieldMaxTraceDistance();
		float GetFarFieldDitheredStartDistanceFactor();
		//FVector GetFarFieldReferencePos();

		float GetHeightfieldReceiverBias();
		void Shutdown();
	}
}
