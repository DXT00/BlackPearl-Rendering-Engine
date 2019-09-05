#pragma once
#include"GameEngine/Renderer/Shader.h"
#include<memory>
#include"Window.h"
#include"Renderer/Texture/Texture.h"
#include"Renderer/Camera/Camera.h"
#include"GameEngine/Timestep/Timestep.h"

class Application
{
public:
	Application();
	~Application();
	void Run();
	inline static Application &Get()  { return *s_Instance; }
	inline Window& GetWindow() { return *m_Window; }
	void InputCheck(Timestep ts);
private:
	std::shared_ptr<Shader> m_Shader;
	std::unique_ptr<Window> m_Window;

	std::unique_ptr<Texture> m_Texture1;
	std::unique_ptr<Texture> m_Texture2;
	unsigned int m_VertexArrayID, m_IndexBufferID, m_VertexBufferID;

	std::unique_ptr<Camera> m_Camera;
	float m_CameraMoveSpeed = 5;
	glm::vec3 m_CameraPosition ;
private:
	static Application* s_Instance;
	float m_LastFrameTime = 0.0f;


};

