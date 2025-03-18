#pragma once
//#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
//#include "BlackPearl/Renderer/MasterRenderer/GeometryRenderer.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "BlackPearl/Renderer/DrawStrategy.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Shader/MaterialShader.h"

namespace BlackPearl {

	extern ShaderFactory* g_shaderFactory;
	class PBRRenderer: public BasicRenderer//public GeometryRenderer
	{
	public:
		PBRRenderer(IDevice* device)
		:BasicRenderer(device)
		{
			
		    //	m_PbrShader.reset(DBG_NEW Shader("assets/shaders/pbr/Pbr.glsl"));
			//m_PbrShader.reset(DBG_NEW Shader("assets/shaders/pbr/PbrTexture.glsl"));
		};

		void Init();
		void Render(const std::vector<Object*>& objs);
		void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);

		void Render(Object* obj);
		MaterialShader* GetShader() const { return m_PbrShader; }
		~PBRRenderer();

	private:
		MaterialShader* m_PbrShader;

		InstancedOpaqueDrawStrategy* m_DrawStrategy;

        BufferHandle  m_ForwardViewCB;
        SamplerHandle m_ShadowSampler;


        ShaderParameters m_ShaderParameters[ShaderType::NUM_COMPILE_SHADER_STAGES];
	};
}


