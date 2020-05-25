#include "pch.h"
#include "SkyboxRenderer.h"

namespace BlackPearl {



	SkyboxRenderer::SkyboxRenderer()
	{
		std::vector<std::string> morningBox = {
			 "assets/skybox/skybox1/SkyBrightMorning_Right.png",
			"assets/skybox/skybox1/SkyBrightMorning_Left.png",
			"assets/skybox/skybox1/SkyBrightMorning_Top.png",
			"assets/skybox/skybox1/SkyBrightMorning_Bottom.png",
			"assets/skybox/skybox1/SkyBrightMorning_Front.png",
			"assets/skybox/skybox1/SkyBrightMorning_Back.png"
		};
		std::vector<std::string> sunSetBox = {
		 "assets/skybox/skybox1/SkyMorning_Right.png",
		"assets/skybox/skybox1/SkyMorning_Left.png",
		"assets/skybox/skybox1/SkyMorning_Top.png",
		"assets/skybox/skybox1/SkyMorning_Bottom.png",
		"assets/skybox/skybox1/SkyMorning_Front.png",
		"assets/skybox/skybox1/SkyMorning_Back.png"
		};
		std::vector<std::string> nightBox = {
		 "assets/skybox/skybox1/SkyNight_Right.png",
		"assets/skybox/skybox1/SkyNight_Left.png",
		"assets/skybox/skybox1/SkyNight_Top.png",
		"assets/skybox/skybox1/SkyNight_Bottom.png",
		"assets/skybox/skybox1/SkyNight_Front.png",
		"assets/skybox/skybox1/SkyNight_Back.png"
		};
		m_SkyboxShader.reset(DBG_NEW Shader("assets/shaders/SkyBoxMultiTexture.glsl"));
		m_SkyBoxTexture[0].reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, nightBox , GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE));
		m_SkyBoxTexture[1].reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap,morningBox , GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE));
		m_SkyBoxTexture[2].reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap,sunSetBox , GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE));
	}

	void SkyboxRenderer::Render(Object* skybox,float timeSecond)
	{
		float currentTimeS = fmod(timeSecond ,m_TotalTimeIntervalS);
		int state = int(currentTimeS / m_StateIntervalS);
		int nextState = state + 1;
		float stateFactor = nextState * m_StateIntervalS - currentTimeS; 
		float nextStateFactor = currentTimeS - state * m_StateIntervalS;
		nextState %= 3;
		state %= 3;
		m_SkyboxShader->Bind();

		m_SkyboxShader->SetUniform1f("u_Factor" + std::to_string(state), stateFactor/m_StateIntervalS);
		m_SkyboxShader->SetUniform1f("u_Factor" + std::to_string(nextState), nextStateFactor / m_StateIntervalS);
		m_SkyboxShader->SetUniform1f("u_Factor" + std::to_string(3 - state - nextState), 0);

		m_SkyboxShader->SetUniform1i("u_Skybox" + std::to_string(state), state);
		m_SkyboxShader->SetUniform1i("u_Skybox" + std::to_string(nextState), nextState);
		m_SkyboxShader->SetUniform1i("u_Skybox" + std::to_string(3-state-nextState), 3 - state - nextState);
		glActiveTexture(GL_TEXTURE0 + state);
		m_SkyBoxTexture[state]->Bind();
		glActiveTexture(GL_TEXTURE0 + nextState);
		m_SkyBoxTexture[nextState]->Bind();
		glActiveTexture(GL_TEXTURE0 + 3 - state - nextState);
		m_SkyBoxTexture[3 - state - nextState]->Bind();
		DrawObject(skybox,m_SkyboxShader);

	}
	
	void SkyboxRenderer::Render(Object* skybox)
	{
		DrawObject(skybox);
	}
	void SkyboxRenderer::Render(Object* skybox, Renderer::SceneData* scene)
	{
		DrawObject(skybox, scene);
	}
	void SkyboxRenderer::Render(Object* skybox, float timeSecond, Renderer::SceneData* scene)
	{

		float currentTimeS = fmod(timeSecond, m_TotalTimeIntervalS);
		int state = int(currentTimeS / m_StateIntervalS);
		int nextState = state + 1;
		float stateFactor = nextState * m_StateIntervalS - currentTimeS;
		float nextStateFactor = currentTimeS - state * m_StateIntervalS;
		nextState %= 3;
		state %= 3;
		m_SkyboxShader->Bind();

		m_SkyboxShader->SetUniform1f("u_Factor" + std::to_string(state), stateFactor / m_StateIntervalS);
		m_SkyboxShader->SetUniform1f("u_Factor" + std::to_string(nextState), nextStateFactor / m_StateIntervalS);
		m_SkyboxShader->SetUniform1f("u_Factor" + std::to_string(3 - state - nextState), 0);

		m_SkyboxShader->SetUniform1i("u_Skybox" + std::to_string(state), state);
		m_SkyboxShader->SetUniform1i("u_Skybox" + std::to_string(nextState), nextState);
		m_SkyboxShader->SetUniform1i("u_Skybox" + std::to_string(3 - state - nextState), 3 - state - nextState);
		glActiveTexture(GL_TEXTURE0 + state);
		m_SkyBoxTexture[state]->Bind();
		glActiveTexture(GL_TEXTURE0 + nextState);
		m_SkyBoxTexture[nextState]->Bind();
		glActiveTexture(GL_TEXTURE0 + 3 - state - nextState);
		m_SkyBoxTexture[3 - state - nextState]->Bind();
		DrawObject(skybox, m_SkyboxShader,scene);
	}

}