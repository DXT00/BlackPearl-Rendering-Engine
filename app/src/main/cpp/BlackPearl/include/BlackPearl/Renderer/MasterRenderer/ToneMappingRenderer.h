#pragma once
#include "BasicRenderer.h"
#include "RHI/RHIDevice.h"
#include "hlsl/core/tonemapping_cb.h"
namespace BlackPearl {


    class ToneMappingRenderer : public BasicRenderer {
        
        public:

            ToneMappingRenderer(IDevice * device);


            void Init(TextureHandle grabTexture, bool plsCopy = false);
            void Render(ICommandList * commandList, IFramebuffer * targetFramebuffer, Scene * scene);


            static void FillShaderParameters(ToneMappingConstants& output);

        private:
            MaterialShader* m_ToneMappingShader;

            BufferHandle        m_ToneMappingCB;


            BindingLayoutHandle m_ToneMappingBindingLayout;
            BindingSetHandle    m_ToneMappingBindingSet;

            GraphicsPipelineHandle m_ToneMappingPso = nullptr;

        };


    }
