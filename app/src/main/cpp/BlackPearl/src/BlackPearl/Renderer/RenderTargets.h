#pragma once
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/RHI/RHIHeap.h"
#include "BlackPearl/RHI/RHICommandList.h"
#include "BlackPearl/Math/Math.h"
namespace BlackPearl {

	class RenderTargets// : public donut::render::GBufferRenderTargets
	{
	public:
	//	const uint32_t m_SampleCount = 1; // no MSAA supported in this sample
	//	bool m_UseReverseProjection = false;
	//	int m_BackbufferCount = 3;
	//	IDevice* m_Device;
	//public:
	//	TextureHandle AccumulatedRadiance;   // used only in non-realtime mode
	//	TextureHandle LdrColor;              // final, post-tonemapped color
		TextureHandle OutputColor;           // raw path tracing output goes here (in both realtime and non-realtime modes)
	//	TextureHandle ProcessedOutputColor;  // for when post-processing OutputColor (i.e. TAA) (previously ResolvedColor)
	//	TextureHandle TemporalFeedback1;     // used by TAA
	//	TextureHandle TemporalFeedback2;     // used by TAA
	//	TextureHandle PreUIColor;            // used DLSS-G

	//	TextureHandle DebugVizOutput;

	//	TextureHandle Throughput;            // when using PSR we need to remember throughput after perfect speculars with color for RTXDI to know how to do its thing correctly
	//	TextureHandle Depth;                 // exported by path tracer, used by TAA and others
	//	TextureHandle ScreenMotionVectors;   // screen space motion vectors, exported by path tracer, used by RTXDI, TAA and others

	//	TextureHandle DenoiserViewspaceZ;
	//	TextureHandle DenoiserMotionVectors;
	//	TextureHandle DenoiserNormalRoughness;

	//	TextureHandle StableRadiance;                    // radiance that doesn't require denoising; this is technically not needed as a separate buffer, but very useful for debug viz
	//	TextureHandle StablePlanesHeader;
	//	BufferHandle  StablePlanesBuffer;

	//	//BufferHandle  SurfaceDataBuffer;

	//	TextureHandle DenoiserDiffRadianceHitDist;       // input to denoiser
	//	TextureHandle DenoiserSpecRadianceHitDist;       // input to denoiser
	//	TextureHandle DenoiserDisocclusionThresholdMix;  // input to denoiser (see IN_DISOCCLUSION_THRESHOLD_MIX)

	//	TextureHandle CombinedHistoryClampRelax;         // all DenoiserDisocclusionThresholdMix combined together - used to tell TAA where to relax disocclusion test to minimize aliasing
	//	HeapHandle Heap;

	//	math::int2 m_RenderSize;// size of render targets pre-DLSS
	//	math::int2 m_DisplaySize; // size of render targets post-DLSS

	void Init(IDevice* device, math::int2 renderSize, math::int2 displaySize, bool enableMotionVectors, bool useReverseProjection, int backbufferCount);// override;
	[[nodiscard]] bool IsUpdateRequired(math::int2 renderSize, math::int2 displaySize, math::uint sampleCount = 1) const;
	void Clear(ICommandList* commandList); // override;
	};



}