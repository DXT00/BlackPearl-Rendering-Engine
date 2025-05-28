#pragma once
//#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
//#include "BlackPearl/Renderer/MasterRenderer/GeometryRenderer.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "BlackPearl/Renderer/DrawStrategy.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Shader/MaterialShader.h"
#include "BlackPearl/Renderer/Shader/ShaderParameters.h"
#include "hlsl/core/forward_cb.h"

namespace BlackPearl {

	extern ShaderFactory* g_shaderFactory;
	class ForwardShadingRenderer: public BasicRenderer//public GeometryRenderer
	{
	public:
		ForwardShadingRenderer(IDevice* device)
		:BasicRenderer(device)
		{
			
		    //	m_PbrShader.reset(DBG_NEW Shader("assets/shaders/pbr/Pbr.glsl"));
			//m_PbrShader.reset(DBG_NEW Shader("assets/shaders/pbr/PbrTexture.glsl"));
		};

		void Init();
		void Render(const std::vector<Object*>& objs);
		void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);
        void FillShaderParameters(LightSources* lightSource, ForwardShadingLightConstants& output);

		void Render(Object* obj);
		MaterialShader* GetShader() const { return m_ForwardShadingShader; }
		~ForwardShadingRenderer();

	private:
        BufferHandle        m_LightsCB;
        BindingLayoutHandle m_ForwardLightsBindingLayout;
        BindingSetHandle    m_ForwardLightsBindingSet;
		MaterialShader* m_ForwardShadingShader = nullptr;

		InstancedOpaqueDrawStrategy* m_DrawStrategy;


        ShaderParameters m_ShaderParameters[ShaderType::NUM_COMPILE_SHADER_STAGES];
	};
}


