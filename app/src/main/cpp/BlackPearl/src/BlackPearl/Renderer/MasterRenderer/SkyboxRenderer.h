#pragma once
#include <BlackPearl/Object/Object.h>
#include <BlackPearl/Renderer/Shader/Shader.h>
#include <BlackPearl/Renderer/Material/CubeMapTexture.h>
#include <BlackPearl/Renderer/MasterRenderer/BasicRenderer.h>
namespace BlackPearl {
	class SkyboxRenderer:public BasicRenderer
	{
	public:
		SkyboxRenderer();

		void Render(Object* skybox);
		void Render(Object* skybox, float timeSecond);
		void Render(Object* skybox,Renderer::SceneData* scene);
		void Render(Object* skybox, float timeSecond, Renderer::SceneData* scene);

	private:
		std::shared_ptr<Shader> m_SkyboxShader;
		std::shared_ptr<CubeMapTexture> m_SkyBoxTexture[3];

		float m_TotalTimeIntervalS = 50.0f;//second
		float m_StateIntervalS = m_TotalTimeIntervalS / 3.0f;


	};
}


