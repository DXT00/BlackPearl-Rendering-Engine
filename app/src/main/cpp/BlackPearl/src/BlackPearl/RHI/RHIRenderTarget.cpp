#include "pch.h"
#include "RHI/RHIRenderTarget.h"
#include "RHI/RHITexture.h"
namespace BlackPearl {


	//common case
	FRHIRenderTargetView::FRHIRenderTargetView(ITexture* InTexture, ERenderTargetLoadAction InLoadAction) :
		Texture(InTexture),
		MipIndex(0),
		ArraySliceIndex(-1),
		LoadAction(InLoadAction),
		StoreAction(ERenderTargetStoreAction::EStore)
	{
	}

	//common case
	FRHIRenderTargetView::FRHIRenderTargetView(ITexture* InTexture, ERenderTargetLoadAction InLoadAction, uint32_t InMipIndex, uint32_t InArraySliceIndex) :
		Texture(InTexture),
		MipIndex(InMipIndex),
		ArraySliceIndex(InArraySliceIndex),
		LoadAction(InLoadAction),
		StoreAction(ERenderTargetStoreAction::EStore)
	{
	}

	FRHIRenderTargetView::FRHIRenderTargetView(ITexture* InTexture, uint32_t InMipIndex, uint32_t InArraySliceIndex, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction) :
		Texture(InTexture),
		MipIndex(InMipIndex),
		ArraySliceIndex(InArraySliceIndex),
		LoadAction(InLoadAction),
		StoreAction(InStoreAction)
	{
	}



	// Color, no depth, optional resolve, optional mip, optional array slice
	FRHIRenderPassInfo::FRHIRenderPassInfo(ITexture* ColorRT, ERenderTargetActions ColorAction, ITexture* ResolveRT, uint8_t InMipIndex, int32_t InArraySlice)
	{
		GE_ASSERT((!ResolveRT || ResolveRT->getDesc().isMultiSample()), "invalid resolve rt");
		GE_ASSERT(ColorRT, "invald color rt");
		auto target = ColorRenderTargets[0].RenderTarget;
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = ResolveRT;
		ColorRenderTargets[0].ArraySlice = InArraySlice;
		ColorRenderTargets[0].MipIndex = InMipIndex;
		ColorRenderTargets[0].Action = ColorAction;
	}

    FRHIRenderPassInfo::FRHIRenderPassInfo(ITexture* ColorRT, ERenderTargetActions ColorAction, ITexture* DepthRT, EDepthStencilTargetActions DepthActions, uint8_t InMipIndex, int32_t InArraySlice)
    {
        GE_ASSERT(ColorRT, "invald color rt");
        auto target = ColorRenderTargets[0].RenderTarget;
        ColorRenderTargets[0].RenderTarget = ColorRT;
        ColorRenderTargets[0].ResolveTarget = nullptr;
        ColorRenderTargets[0].ArraySlice = InArraySlice;
        ColorRenderTargets[0].MipIndex = InMipIndex;
        ColorRenderTargets[0].Action = ColorAction;


        DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
        DepthStencilRenderTarget.Action = DepthActions;
        DepthStencilRenderTarget.ExclusiveDepthStencil = FExclusiveDepthStencil::DepthWrite_StencilWrite;// FExclusiveDepthStencil::DepthNop_StencilNop;
        DepthStencilRenderTarget.ResolveTarget = nullptr;
    }


    // one Color , no depth, no resolve, multi mip, multi array slice, such as cubemap
    FRHIRenderPassInfo::FRHIRenderPassInfo(int32_t NumColorSlice, ITexture* ColorRTs, ERenderTargetActions ColorAction, uint8_t* InMipIndex, int32_t* InArraySlice, ITexture* DepthRT, EDepthStencilTargetActions DepthActions)
    {
        GE_ASSERT(NumColorSlice > 0, "invald color slcie num");
        //for (int32_t Index = 0; Index < 1; ++Index)
        {
            GE_ASSERT(ColorRTs, "invald color rt");
            ColorRenderTargets[0].RenderTarget = ColorRTs;
            ColorRenderTargets[0].ArraySlice = InArraySlice[0];
            ColorRenderTargets[0].Action = ColorAction;
            ColorRenderTargets[0].MipIndex = InMipIndex[0];
        }
        DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
        DepthStencilRenderTarget.Action = DepthActions;
        DepthStencilRenderTarget.ExclusiveDepthStencil = FExclusiveDepthStencil::DepthNop_StencilNop;
        DepthStencilRenderTarget.ResolveTarget = nullptr;
    }

	// Color MRTs, no depth
	FRHIRenderPassInfo::FRHIRenderPassInfo(int32_t NumColorRTs, ITexture* ColorRTs[], ERenderTargetActions ColorAction)
	{
		GE_ASSERT(NumColorRTs > 0, "invald color rt num");
		for (int32_t Index = 0; Index < NumColorRTs; ++Index)
		{
			GE_ASSERT(ColorRTs[Index], "invald color rt");
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		DepthStencilRenderTarget.DepthStencilTarget = nullptr;
		DepthStencilRenderTarget.Action = EDepthStencilTargetActions::DontLoad_DontStore;
		DepthStencilRenderTarget.ExclusiveDepthStencil = FExclusiveDepthStencil::DepthNop_StencilNop;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
	}

	// Color MRTs, no depth
	FRHIRenderPassInfo::FRHIRenderPassInfo(int32_t NumColorRTs, ITexture* ColorRTs[], ERenderTargetActions ColorAction, ITexture* ResolveTargets[])
	{
		GE_ASSERT(NumColorRTs > 0, "invald color rt num");
		for (int32_t Index = 0; Index < NumColorRTs; ++Index)
		{
			GE_ASSERT(ColorRTs[Index], "invald color rt");
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ResolveTarget = ResolveTargets[Index];
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].MipIndex = 0;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		DepthStencilRenderTarget.DepthStencilTarget = nullptr;
		DepthStencilRenderTarget.Action = EDepthStencilTargetActions::DontLoad_DontStore;
		DepthStencilRenderTarget.ExclusiveDepthStencil = FExclusiveDepthStencil::DepthNop_StencilNop;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
	}

	// Color MRTs and depth
	FRHIRenderPassInfo::FRHIRenderPassInfo(int32_t NumColorRTs, ITexture* ColorRTs[], ERenderTargetActions ColorAction, ITexture* DepthRT, EDepthStencilTargetActions DepthActions, FExclusiveDepthStencil InEDS)
	{
		GE_ASSERT(NumColorRTs > 0, "invalid num color rt");
		for (int32_t Index = 0; Index < NumColorRTs; ++Index)
		{
			GE_ASSERT(ColorRTs[Index], "invalid color rt");
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ResolveTarget = nullptr;
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].MipIndex = 0;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		GE_ASSERT(DepthRT, "invalid depth rt");
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Color MRTs and depth
	FRHIRenderPassInfo::FRHIRenderPassInfo(int32_t NumColorRTs, ITexture* ColorRTs[], ERenderTargetActions ColorAction, ITexture* ResolveRTs[], ITexture* DepthRT, EDepthStencilTargetActions DepthActions, ITexture* ResolveDepthRT, FExclusiveDepthStencil InEDS)
	{
		GE_ASSERT(NumColorRTs > 0, "invalid num color rt");
		for (int32_t Index = 0; Index < NumColorRTs; ++Index)
		{
			GE_ASSERT(!ResolveRTs[Index] || ResolveRTs[Index]->getDesc().isMultiSample(), "invalid resolve rt");
			GE_ASSERT(ColorRTs[Index], "invalid color rt");
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ResolveTarget = ResolveRTs[Index];
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].MipIndex = 0;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		GE_ASSERT(!ResolveDepthRT || ResolveDepthRT->getDesc().isMultiSample(), "ResolveDepthRT invalid");
		GE_ASSERT(DepthRT, "invalid depth rt");
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Depth, no color
	FRHIRenderPassInfo::FRHIRenderPassInfo(ITexture* DepthRT, EDepthStencilTargetActions DepthActions, ITexture* ResolveDepthRT, FExclusiveDepthStencil InEDS)
	{
		GE_ASSERT(!ResolveDepthRT || ResolveDepthRT->getDesc().isMultiSample(), "ResolveDepthRT invalid");
		GE_ASSERT(DepthRT, "invalid depth rt");
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Depth, no color, occlusion queries
	FRHIRenderPassInfo::FRHIRenderPassInfo(ITexture* DepthRT, uint32_t InNumOcclusionQueries, EDepthStencilTargetActions DepthActions, ITexture* ResolveDepthRT, FExclusiveDepthStencil InEDS)
		: NumOcclusionQueries(InNumOcclusionQueries)
	{
		GE_ASSERT(!ResolveDepthRT || ResolveDepthRT->getDesc().isMultiSample(), "ResolveDepthRT invalid");
		GE_ASSERT(DepthRT, "invalid depth rt");
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Color and depth
	FRHIRenderPassInfo::FRHIRenderPassInfo(ITexture* ColorRT, ERenderTargetActions ColorAction, ITexture* DepthRT, EDepthStencilTargetActions DepthActions, FExclusiveDepthStencil InEDS)
	{
		GE_ASSERT(ColorRT, "invalid color rt");
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = nullptr;
		ColorRenderTargets[0].ArraySlice = -1;
		ColorRenderTargets[0].MipIndex = 0;
		ColorRenderTargets[0].Action = ColorAction;
		GE_ASSERT(DepthRT, "invalid depth rt");
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
		FMemory::Memzero(&ColorRenderTargets[1], sizeof(FColorEntry) * (c_MaxRenderTargets - 1));


        //GE_ASSERT(NumColorSlice > 0, "invald color slcie num");
        ////for (int32_t Index = 0; Index < 1; ++Index)
        //{
        //    GE_ASSERT(ColorRTs, "invald color rt");
        //    ColorRenderTargets[0].RenderTarget = ColorRTs;
        //    ColorRenderTargets[0].ArraySlice = InArraySlice[0];
        //    ColorRenderTargets[0].Action = ColorAction;
        //    ColorRenderTargets[0].MipIndex = InMipIndex[0];
        //}
        //DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
        //DepthStencilRenderTarget.Action = DepthActions;
        //DepthStencilRenderTarget.ExclusiveDepthStencil = FExclusiveDepthStencil::DepthNop_StencilNop;
        //DepthStencilRenderTarget.ResolveTarget = nullptr;

	}

	// Color and depth with resolve
	FRHIRenderPassInfo::FRHIRenderPassInfo(ITexture* ColorRT, ERenderTargetActions ColorAction, ITexture* ResolveColorRT,
		ITexture* DepthRT, EDepthStencilTargetActions DepthActions, ITexture* ResolveDepthRT, FExclusiveDepthStencil InEDS)
	{
		GE_ASSERT(!ResolveColorRT || ResolveColorRT->getDesc().isMultiSample(), "ResolveColorRT invalid");
		GE_ASSERT(!ResolveDepthRT || ResolveDepthRT->getDesc().isMultiSample(), "ResolveDepthRT invalid");
		GE_ASSERT(ColorRT, "invalid color rt");
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = ResolveColorRT;
		ColorRenderTargets[0].ArraySlice = -1;
		ColorRenderTargets[0].MipIndex = 0;
		ColorRenderTargets[0].Action = ColorAction;
		GE_ASSERT(DepthRT, "invalid depth rt");
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
		FMemory::Memzero(&ColorRenderTargets[1], sizeof(FColorEntry) * (c_MaxRenderTargets - 1));
	}

	// Color and depth with resolve and optional sample density
	FRHIRenderPassInfo::FRHIRenderPassInfo(ITexture* ColorRT, ERenderTargetActions ColorAction, ITexture* ResolveColorRT,
		ITexture* DepthRT, EDepthStencilTargetActions DepthActions, ITexture* ResolveDepthRT,
		ITexture* InShadingRateTexture, EVRSRateCombiner InShadingRateTextureCombiner,
		FExclusiveDepthStencil InEDS)
	{
		GE_ASSERT(!ResolveColorRT || ResolveColorRT->getDesc().isMultiSample(), "ResolveColorRT invalid");
		GE_ASSERT(!ResolveDepthRT || ResolveDepthRT->getDesc().isMultiSample(), "ResolveDepthRT invalid");
		GE_ASSERT(ColorRT, "invalid color rt");
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = ResolveColorRT;
		ColorRenderTargets[0].ArraySlice = -1;
		ColorRenderTargets[0].MipIndex = 0;
		ColorRenderTargets[0].Action = ColorAction;
		GE_ASSERT(DepthRT, "invalid depth rt");
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
		ShadingRateTexture = InShadingRateTexture;
		ShadingRateTextureCombiner = InShadingRateTextureCombiner;
		FMemory::Memzero(&ColorRenderTargets[1], sizeof(FColorEntry) * (c_MaxRenderTargets - 1));
	}

	inline int32_t FRHIRenderPassInfo::GetNumColorRenderTargets() const
	{
		int32_t ColorIndex = 0;
		for (; ColorIndex < c_MaxRenderTargets; ++ColorIndex)
		{
			const FColorEntry& Entry = ColorRenderTargets[ColorIndex];
			if (!Entry.RenderTarget)
			{
				break;
			}
		}

		return ColorIndex;
	}

	//FGraphicsPipelineRenderTargetsInfo ExtractRenderTargetsInfo() const
	//{
	//    FGraphicsPipelineRenderTargetsInfo RenderTargetsInfo;

	//    RenderTargetsInfo.NumSamples = 1;
	//    int32_t RenderTargetIndex = 0;

	//    for (; RenderTargetIndex < c_MaxRenderTargets; ++RenderTargetIndex)
	//    {
	//        ITexture* RenderTarget = ColorRenderTargets[RenderTargetIndex].RenderTarget;
	//        if (!RenderTarget)
	//        {
	//            break;
	//        }

	//        RenderTargetsInfo.RenderTargetFormats[RenderTargetIndex] = (uint8_t)RenderTarget->GetFormat();
	//        RenderTargetsInfo.RenderTargetFlags[RenderTargetIndex] = RenderTarget->GetFlags();
	//        RenderTargetsInfo.NumSamples |= RenderTarget->getDesc().sampleCount;
	//    }

	//    RenderTargetsInfo.RenderTargetsEnabled = RenderTargetIndex;
	//    for (; RenderTargetIndex < MaxSimultaneousRenderTargets; ++RenderTargetIndex)
	//    {
	//        RenderTargetsInfo.RenderTargetFormats[RenderTargetIndex] = PF_Unknown;
	//    }

	//    if (DepthStencilRenderTarget.DepthStencilTarget)
	//    {
	//        RenderTargetsInfo.DepthStencilTargetFormat = DepthStencilRenderTarget.DepthStencilTarget->GetFormat();
	//        RenderTargetsInfo.DepthStencilTargetFlag = DepthStencilRenderTarget.DepthStencilTarget->GetFlags();
	//        RenderTargetsInfo.NumSamples |= DepthStencilRenderTarget.DepthStencilTarget->GetNumSamples();
	//    }
	//    else
	//    {
	//        RenderTargetsInfo.DepthStencilTargetFormat = PF_Unknown;
	//    }

	//    const ERenderTargetActions DepthActions = GetDepthActions(DepthStencilRenderTarget.Action);
	//    const ERenderTargetActions StencilActions = GetStencilActions(DepthStencilRenderTarget.Action);
	//    RenderTargetsInfo.DepthTargetLoadAction = GetLoadAction(DepthActions);
	//    RenderTargetsInfo.DepthTargetStoreAction = GetStoreAction(DepthActions);
	//    RenderTargetsInfo.StencilTargetLoadAction = GetLoadAction(StencilActions);
	//    RenderTargetsInfo.StencilTargetStoreAction = GetStoreAction(StencilActions);
	//    RenderTargetsInfo.DepthStencilAccess = DepthStencilRenderTarget.ExclusiveDepthStencil;

	//    RenderTargetsInfo.MultiViewCount = MultiViewCount;
	//    RenderTargetsInfo.bHasFragmentDensityAttachment = ShadingRateTexture != nullptr;

	//    return RenderTargetsInfo;
	//}

//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
//        RHI_API void Validate() const;
//#else
//        void Validate() const {}
//#endif
	void FRHIRenderPassInfo::ConvertToRenderTargetsInfo(FRHISetRenderTargetsInfo& OutRTInfo) const
	{
		for (int32_t Index = 0; Index < c_MaxRenderTargets; ++Index)
		{
			if (!ColorRenderTargets[Index].RenderTarget)
			{
				break;
			}

			OutRTInfo.ColorRenderTarget[Index].Texture = ColorRenderTargets[Index].RenderTarget;
			ERenderTargetLoadAction LoadAction = GetLoadAction(ColorRenderTargets[Index].Action);
			OutRTInfo.ColorRenderTarget[Index].LoadAction = LoadAction;
			OutRTInfo.ColorRenderTarget[Index].StoreAction = GetStoreAction(ColorRenderTargets[Index].Action);
			OutRTInfo.ColorRenderTarget[Index].ArraySliceIndex = ColorRenderTargets[Index].ArraySlice;
			OutRTInfo.ColorRenderTarget[Index].MipIndex = ColorRenderTargets[Index].MipIndex;
			++OutRTInfo.NumColorRenderTargets;

			OutRTInfo.bClearColor |= (LoadAction == ERenderTargetLoadAction::EClear);

			assert(!OutRTInfo.bHasResolveAttachments || ColorRenderTargets[Index].ResolveTarget);
			if (ColorRenderTargets[Index].ResolveTarget)
			{
				OutRTInfo.bHasResolveAttachments = true;
				OutRTInfo.ColorResolveRenderTarget[Index] = OutRTInfo.ColorRenderTarget[Index];
				OutRTInfo.ColorResolveRenderTarget[Index].Texture = ColorRenderTargets[Index].ResolveTarget;
			}
		}

		ERenderTargetActions DepthActions = GetDepthActions(DepthStencilRenderTarget.Action);
		ERenderTargetActions StencilActions = GetStencilActions(DepthStencilRenderTarget.Action);
		ERenderTargetLoadAction DepthLoadAction = GetLoadAction(DepthActions);
		ERenderTargetStoreAction DepthStoreAction = GetStoreAction(DepthActions);
		ERenderTargetLoadAction StencilLoadAction = GetLoadAction(StencilActions);
		ERenderTargetStoreAction StencilStoreAction = GetStoreAction(StencilActions);

		OutRTInfo.DepthStencilRenderTarget = FRHIDepthRenderTargetView(DepthStencilRenderTarget.DepthStencilTarget,
			DepthLoadAction,
			GetStoreAction(DepthActions),
			StencilLoadAction,
			GetStoreAction(StencilActions),
			DepthStencilRenderTarget.ExclusiveDepthStencil);
		OutRTInfo.bClearDepth = (DepthLoadAction == ERenderTargetLoadAction::EClear);
		OutRTInfo.bClearStencil = (StencilLoadAction == ERenderTargetLoadAction::EClear);

		OutRTInfo.ShadingRateTexture = ShadingRateTexture;
		OutRTInfo.ShadingRateTextureCombiner = ShadingRateTextureCombiner;
		OutRTInfo.MultiViewCount = MultiViewCount;
	}
}