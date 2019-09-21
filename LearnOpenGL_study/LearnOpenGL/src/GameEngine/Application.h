#pragma once
#include"GameEngine/Renderer/Shader.h"
#include<memory>
#include"Window.h"
#include"Renderer/Texture/Texture.h"
#include"Renderer/Camera/Camera.h"
#include"Renderer/VertexArray.h"
#include"GameEngine/Timestep/Timestep.h"
#include "Event/MouseEvent.h"
#include "Renderer/Lighting/Light.h"
#include"Renderer/Lighting/LightSources.h"
class Application
{
public:
	Application();
	~Application();
	
	inline static Application &Get()  { return *s_Instance; }
	inline Window& GetWindow() { return *m_Window; }
	
	void Run();
	void InputCheck(float ts);
	void OnEvent(Event &event);

private:
	bool OnCameraRotate(MouseMovedEvent&e);

private:
	std::shared_ptr<Shader> m_Shader;
	std::unique_ptr<Window> m_Window;

	std::unique_ptr<Texture> m_Texture1;
	std::unique_ptr<Texture> m_Texture2;
	std::unique_ptr<Texture> m_DiffuseMap;
	std::unique_ptr<Texture> m_SpecularMap;
	std::unique_ptr<Texture> m_EmissionMap;

	unsigned int m_VertexArrayID, m_IndexBufferID, m_VertexBufferID;

	std::shared_ptr<VertexArray> m_VertexArray;



	std::unique_ptr<Camera> m_Camera;
	//std::shared_ptr<Light> m_SpotLightSource;
	LightSources m_LightSources;
	float m_CameraMoveSpeed = 5.0f;
	float m_CameraRotateSpeed = 9.0f;
	glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
	struct CameraRotation {
		float Yaw;
		float Pitch;
		
	};
	CameraRotation m_CameraRotation;
	float m_LastMouseX;
	float m_LastMouseY;


private:
	static Application* s_Instance;
	float m_LastFrameTime = 0.0f;


};

