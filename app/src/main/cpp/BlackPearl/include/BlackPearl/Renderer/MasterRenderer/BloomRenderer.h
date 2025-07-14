#pragma once
#include "Renderer/MasterRenderer/BasicRenderer.h"
namespace BlackPearl {
	class BloomRenderer : public BasicRenderer
	{

	public:
        BloomRenderer(IDevice* device);

        void Init();
        void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);

        
	private:


        struct PerViewData
        {
            GraphicsPipelineHandle bloomBlurPso;

            TextureHandle textureDownscale1;
            FramebufferHandle framebufferDownscale1;
            TextureHandle textureDownscale2;
            FramebufferHandle framebufferDownscale2;

            TextureHandle texturePass1Blur;
            FramebufferHandle framebufferPass1Blur;
            TextureHandle texturePass2Blur;
            FramebufferHandle framebufferPass2Blur;

            BindingSetHandle bloomBlurBindingSetPass1;
            BindingSetHandle bloomBlurBindingSetPass2;
            BindingSetHandle bloomBlurBindingSetPass3;
            BindingSetHandle blitFromDownscale1BindingSet;
            BindingSetHandle compositeBlitBindingSet;
        };

        std::vector<PerViewData> m_PerViewData;
        BufferHandle m_BloomHBlurCB;
        BufferHandle m_BloomVBlurCB;
        ShaderHandle m_BloomBlurPixelShader;
        BindingLayoutHandle m_BloomBlurBindingLayout;
        BindingLayoutHandle m_BloomApplyBindingLayout;

        //engine::BindingCache m_BindingCache;

	};


}
