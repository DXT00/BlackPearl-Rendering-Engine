#pragma once
#include"GameEngine/Renderer/Shader.h"
#include<memory>
#include"Window.h"
#include"Renderer/Texture/Texture.h"
#include"Renderer/Camera/Camera.h"
class Application
{
public:
	Application();
	~Application();
	void Run();

private:
	std::shared_ptr<Shader> m_Shader;
	std::unique_ptr<Window> m_Window;

	std::unique_ptr<Texture> m_Texture1;
	std::unique_ptr<Texture> m_Texture2;
	unsigned int m_VertexArrayID, m_IndexBufferID, m_VertexBufferID;

	std::unique_ptr<Camera> m_Camera;


};

