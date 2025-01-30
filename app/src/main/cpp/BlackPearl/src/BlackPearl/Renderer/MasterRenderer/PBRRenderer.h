#pragma once
//#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/GeometryRenderer.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "BlackPearl/Renderer/DrawStrategy.h"
namespace BlackPearl {
	extern ShaderFactory* g_shaderFactory;
	class PBRRenderer:public BasicRenderer, public GeometryRenderer
	{
	public:
		PBRRenderer() {
			m_DrawStrategy = DBG_NEW InstancedOpaqueDrawStrategy();


			ShaderDesc desc = ShaderDesc(ShaderType::All);
			desc.debugName = "PbrShader";
			m_PbrShader = g_shaderFactory->CreateShader("assets/shaders/pbr/PbrTexture.glsl", "main", nullptr, desc);
		//	m_PbrShader.reset(DBG_NEW Shader("assets/shaders/pbr/Pbr.glsl"));
			//m_PbrShader.reset(DBG_NEW Shader("assets/shaders/pbr/PbrTexture.glsl"));
		};

		
		void Render(const std::vector<Object*>& objs);
		void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);

		void Render(Object* obj);
		ShaderHandle GetShader() { return m_PbrShader; }
		~PBRRenderer();
	private:
		ShaderHandle m_PbrShader;

		InstancedOpaqueDrawStrategy* m_DrawStrategy;
	};
}


