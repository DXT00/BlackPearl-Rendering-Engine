#pragma once
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
namespace BlackPearl {
	class PBRRenderer:public BasicRenderer
	{
	public:
		PBRRenderer() {
		//	m_PbrShader.reset(DBG_NEW Shader("assets/shaders/pbr/Pbr.glsl"));
			m_PbrShader.reset(DBG_NEW Shader("assets/shaders/pbr/PbrTexture.glsl"));
		};
		void Render(const std::vector<Object*>& objs);
		void Render(Object* obj);
		std::shared_ptr<Shader> GetShader() { return m_PbrShader; }
		~PBRRenderer();
	private:
		std::shared_ptr<Shader> m_PbrShader;
	};
}


