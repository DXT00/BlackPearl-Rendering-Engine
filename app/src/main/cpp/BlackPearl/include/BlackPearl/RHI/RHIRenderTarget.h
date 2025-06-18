#pragma once
#include "RHIDefinitions.h"
#include "RHITexture.h"
namespace BlackPearl {
	class FRHIRenderTargetView
	{
	public:
		ITexture* Texture = nullptr;
		uint32_t MipIndex = 0;

		/** Array slice or texture cube face.  Only valid if texture resource was created with TexCreate_TargetArraySlicesIndependently! */
		uint32_t ArraySliceIndex = -1;

		ERenderTargetLoadAction LoadAction = ERenderTargetLoadAction::ENoAction;
		ERenderTargetStoreAction StoreAction = ERenderTargetStoreAction::ENoAction;

		FRHIRenderTargetView() = default;
		FRHIRenderTargetView(FRHIRenderTargetView&&) = default;
		FRHIRenderTargetView(const FRHIRenderTargetView&) = default;
		FRHIRenderTargetView& operator=(FRHIRenderTargetView&&) = default;
		FRHIRenderTargetView& operator=(const FRHIRenderTargetView&) = default;

        explicit FRHIRenderTargetView(ITexture* InTexture, ERenderTargetLoadAction InLoadAction);
        explicit FRHIRenderTargetView(ITexture* InTexture, ERenderTargetLoadAction InLoadAction, uint32_t InMipIndex, uint32_t InArraySliceIndex);
        explicit FRHIRenderTargetView(ITexture* InTexture, uint32_t InMipIndex, uint32_t InArraySliceIndex, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction);

		bool operator==(const FRHIRenderTargetView& Other) const
		{
			return
				Texture == Other.Texture &&
				MipIndex == Other.MipIndex &&
				ArraySliceIndex == Other.ArraySliceIndex &&
				LoadAction == Other.LoadAction &&
				StoreAction == Other.StoreAction;
		}
	};

	class FRHIDepthRenderTargetView
	{
	public:
		ITexture* Texture;

		ERenderTargetLoadAction		DepthLoadAction;
		ERenderTargetStoreAction	DepthStoreAction;
		ERenderTargetLoadAction		StencilLoadAction;

	private:
		ERenderTargetStoreAction	StencilStoreAction;
		FExclusiveDepthStencil		DepthStencilAccess;
	public:

		// accessor to prevent write access to StencilStoreAction
		ERenderTargetStoreAction GetStencilStoreAction() const { return StencilStoreAction; }
		// accessor to prevent write access to DepthStencilAccess
		FExclusiveDepthStencil GetDepthStencilAccess() const { return DepthStencilAccess; }

		explicit FRHIDepthRenderTargetView() :
			Texture(nullptr),
			DepthLoadAction(ERenderTargetLoadAction::ENoAction),
			DepthStoreAction(ERenderTargetStoreAction::ENoAction),
			StencilLoadAction(ERenderTargetLoadAction::ENoAction),
			StencilStoreAction(ERenderTargetStoreAction::ENoAction),
			DepthStencilAccess(FExclusiveDepthStencil::DepthNop_StencilNop)
		{
			Validate();
		}

		//common case
		explicit FRHIDepthRenderTargetView(ITexture* InTexture, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction) :
			Texture(InTexture),
			DepthLoadAction(InLoadAction),
			DepthStoreAction(InStoreAction),
			StencilLoadAction(InLoadAction),
			StencilStoreAction(InStoreAction),
			DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
		{
			Validate();
		}

		explicit FRHIDepthRenderTargetView(ITexture* InTexture, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction, FExclusiveDepthStencil InDepthStencilAccess) :
			Texture(InTexture),
			DepthLoadAction(InLoadAction),
			DepthStoreAction(InStoreAction),
			StencilLoadAction(InLoadAction),
			StencilStoreAction(InStoreAction),
			DepthStencilAccess(InDepthStencilAccess)
		{
			Validate();
		}

		explicit FRHIDepthRenderTargetView(ITexture* InTexture, ERenderTargetLoadAction InDepthLoadAction, ERenderTargetStoreAction InDepthStoreAction, ERenderTargetLoadAction InStencilLoadAction, ERenderTargetStoreAction InStencilStoreAction) :
			Texture(InTexture),
			DepthLoadAction(InDepthLoadAction),
			DepthStoreAction(InDepthStoreAction),
			StencilLoadAction(InStencilLoadAction),
			StencilStoreAction(InStencilStoreAction),
			DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
		{
			Validate();
		}

		explicit FRHIDepthRenderTargetView(ITexture* InTexture, ERenderTargetLoadAction InDepthLoadAction, ERenderTargetStoreAction InDepthStoreAction, ERenderTargetLoadAction InStencilLoadAction, ERenderTargetStoreAction InStencilStoreAction, FExclusiveDepthStencil InDepthStencilAccess) :
			Texture(InTexture),
			DepthLoadAction(InDepthLoadAction),
			DepthStoreAction(InDepthStoreAction),
			StencilLoadAction(InStencilLoadAction),
			StencilStoreAction(InStencilStoreAction),
			DepthStencilAccess(InDepthStencilAccess)
		{
			Validate();
		}

		void Validate() const
		{
			// VK and Metal MAY leave the attachment in an undefined state if the StoreAction is DontCare. So we can't assume read-only implies it should be DontCare unless we know for sure it will never be used again.
			// ensureMsgf(DepthStencilAccess.IsDepthWrite() || DepthStoreAction == ERenderTargetStoreAction::ENoAction, TEXT("Depth is read-only, but we are performing a store.  This is a waste on mobile.  If depth can't change, we don't need to store it out again"));
			/*ensureMsgf(DepthStencilAccess.IsStencilWrite() || StencilStoreAction == ERenderTargetStoreAction::ENoAction, TEXT("Stencil is read-only, but we are performing a store.  This is a waste on mobile.  If stencil can't change, we don't need to store it out again"));*/
		}

		bool operator==(const FRHIDepthRenderTargetView& Other) const
		{
			return
				Texture == Other.Texture &&
				DepthLoadAction == Other.DepthLoadAction &&
				DepthStoreAction == Other.DepthStoreAction &&
				StencilLoadAction == Other.StencilLoadAction &&
				StencilStoreAction == Other.StencilStoreAction &&
				DepthStencilAccess == Other.DepthStencilAccess;
		}
	};

	class FRHISetRenderTargetsInfo
	{
	public:
		// Color Render Targets Info
		FRHIRenderTargetView ColorRenderTarget[c_MaxRenderTargets];
		int32_t NumColorRenderTargets;
		bool bClearColor;

		// Color Render Targets Info
		FRHIRenderTargetView ColorResolveRenderTarget[c_MaxRenderTargets];
		bool bHasResolveAttachments;

		// Depth/Stencil Render Target Info
		FRHIDepthRenderTargetView DepthStencilRenderTarget;
		bool bClearDepth;
		bool bClearStencil;

		TextureHandle ShadingRateTexture;
		EVRSRateCombiner ShadingRateTextureCombiner;

		uint8_t MultiViewCount;

		FRHISetRenderTargetsInfo() :
			NumColorRenderTargets(0),
			bClearColor(false),
			bHasResolveAttachments(false),
			bClearDepth(false),
			ShadingRateTexture(nullptr),
			MultiViewCount(0)
		{
		}

		FRHISetRenderTargetsInfo(int32_t InNumColorRenderTargets, const FRHIRenderTargetView* InColorRenderTargets, const FRHIDepthRenderTargetView& InDepthStencilRenderTarget) :
			NumColorRenderTargets(InNumColorRenderTargets),
			bClearColor(InNumColorRenderTargets > 0 && InColorRenderTargets[0].LoadAction == ERenderTargetLoadAction::EClear),
			bHasResolveAttachments(false),
			DepthStencilRenderTarget(InDepthStencilRenderTarget),
			bClearDepth(InDepthStencilRenderTarget.Texture&& InDepthStencilRenderTarget.DepthLoadAction == ERenderTargetLoadAction::EClear),
			ShadingRateTexture(nullptr),
			ShadingRateTextureCombiner(VRSRB_Passthrough)
		{
			assert(InNumColorRenderTargets <= 0 || InColorRenderTargets);
			for (int32_t Index = 0; Index < InNumColorRenderTargets; ++Index)
			{
				ColorRenderTarget[Index] = InColorRenderTargets[Index];
			}
		}
		// @todo metal mrt: This can go away after all the cleanup is done
		void SetClearDepthStencil(bool bInClearDepth, bool bInClearStencil = false)
		{
			if (bInClearDepth)
			{
				DepthStencilRenderTarget.DepthLoadAction = ERenderTargetLoadAction::EClear;
			}
			if (bInClearStencil)
			{
				DepthStencilRenderTarget.StencilLoadAction = ERenderTargetLoadAction::EClear;
			}
			bClearDepth = bInClearDepth;
			bClearStencil = bInClearStencil;
		}


	};



    struct FRHIRenderPassInfo
    {
        struct FColorEntry
        {
            TextureHandle RenderTarget = nullptr;
            TextureHandle ResolveTarget = nullptr;
            int32_t                ArraySlice = -1;
            uint8_t               MipIndex = 0;
            ERenderTargetActions  Action = ERenderTargetActions::DontLoad_DontStore;
			FColorEntry() {
				 RenderTarget = nullptr;
				 ResolveTarget = nullptr;
				 ArraySlice = -1;
				 MipIndex = 0;
				 Action = ERenderTargetActions::DontLoad_DontStore;
			}
        };
        FColorEntry ColorRenderTargets[c_MaxRenderTargets];

        struct FDepthStencilEntry
        {
            ITexture* DepthStencilTarget = nullptr;
            ITexture* ResolveTarget = nullptr;
            EDepthStencilTargetActions Action = EDepthStencilTargetActions::DontLoad_DontStore;
            FExclusiveDepthStencil ExclusiveDepthStencil;
        };
        FDepthStencilEntry DepthStencilRenderTarget;

        // Controls the area for a multisample resolve or raster UAV (i.e. no fixed-function targets) operation.
        RHIRect ResolveRect;

        // Some RHIs can use a texture to control the sampling and/or shading resolution of different areas 
        ITexture* ShadingRateTexture = nullptr;
        EVRSRateCombiner ShadingRateTextureCombiner = VRSRB_Passthrough;

        // Some RHIs require a hint that occlusion queries will be used in this render pass
        uint32_t NumOcclusionQueries = 0;
        bool bOcclusionQueries = false;

        // if this renderpass should be multiview, and if so how many views are required
        uint8_t MultiViewCount = 0;

        // Hint for some RHI's that renderpass will have specific sub-passes 
        ESubpassHint SubpassHint = ESubpassHint::None;

        FRHIRenderPassInfo() = default;
        FRHIRenderPassInfo(const FRHIRenderPassInfo&) = default;
        FRHIRenderPassInfo& operator=(const FRHIRenderPassInfo&) = default;
		// Color, no depth, optional resolve, optional mip, optional array slice
		explicit FRHIRenderPassInfo(ITexture* ColorRT, ERenderTargetActions ColorAction, ITexture* ResolveRT = nullptr, uint8_t InMipIndex = 0, int32_t InArraySlice = -1);
        explicit FRHIRenderPassInfo(ITexture* ColorRT, ERenderTargetActions ColorAction , ITexture* DepthRT, EDepthStencilTargetActions DepthActions, uint8_t InMipIndex, int32_t InArraySlice);

        
        // Color MRTs, no depth
		explicit FRHIRenderPassInfo(int32_t NumColorRTs, ITexture* ColorRTs[], ERenderTargetActions ColorAction);
		// Color MRTs, no depth
		explicit FRHIRenderPassInfo(int32_t NumColorRTs, ITexture* ColorRTs[], ERenderTargetActions ColorAction, ITexture* ResolveTargets[]);
		// Color MRTs and depth
		explicit FRHIRenderPassInfo(int32_t NumColorRTs, ITexture* ColorRTs[], ERenderTargetActions ColorAction, ITexture* DepthRT, EDepthStencilTargetActions DepthActions, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite);
		// Color MRTs and depth
		explicit FRHIRenderPassInfo(int32_t NumColorRTs, ITexture* ColorRTs[], ERenderTargetActions ColorAction, ITexture* ResolveRTs[], ITexture* DepthRT, EDepthStencilTargetActions DepthActions, ITexture* ResolveDepthRT, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite);
        // one Color , no depth, no resolve, multi mip, multi array slice, such as cubemap
        explicit FRHIRenderPassInfo(int32_t NumColorSlice, ITexture* ColorRTs, ERenderTargetActions ColorAction, uint8_t* InMipIndex, int32_t* InArraySlice, ITexture* DepthRT, EDepthStencilTargetActions DepthActions);
            // Depth, no color
		explicit FRHIRenderPassInfo(ITexture* DepthRT, EDepthStencilTargetActions DepthActions, ITexture* ResolveDepthRT = nullptr, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite);
		// Depth, no color, occlusion queries
		explicit FRHIRenderPassInfo(ITexture* DepthRT, uint32_t InNumOcclusionQueries, EDepthStencilTargetActions DepthActions, ITexture* ResolveDepthRT = nullptr, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite);
		// Color and depth
		explicit FRHIRenderPassInfo(ITexture* ColorRT, ERenderTargetActions ColorAction, ITexture* DepthRT, EDepthStencilTargetActions DepthActions, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite);
		// Color and depth with resolve
		explicit FRHIRenderPassInfo(ITexture* ColorRT, ERenderTargetActions ColorAction, ITexture* ResolveColorRT,
			ITexture* DepthRT, EDepthStencilTargetActions DepthActions, ITexture* ResolveDepthRT, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite);
		// Color and depth with resolve and optional sample density
		explicit FRHIRenderPassInfo(ITexture* ColorRT, ERenderTargetActions ColorAction, ITexture* ResolveColorRT,
			ITexture* DepthRT, EDepthStencilTargetActions DepthActions, ITexture* ResolveDepthRT,
			ITexture* InShadingRateTexture, EVRSRateCombiner InShadingRateTextureCombiner,
			FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite);
		inline int32_t GetNumColorRenderTargets() const;

		void ConvertToRenderTargetsInfo(FRHISetRenderTargetsInfo& OutRTInfo) const;

    };
}