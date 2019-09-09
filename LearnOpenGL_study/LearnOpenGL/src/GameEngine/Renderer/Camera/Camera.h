#pragma once
#include "glm/glm.hpp"
#include<string>
#include<initializer_list>
class Camera
{

public:
	enum {
		Orthographic,
		Perspective
	};
	struct ViewMatrixProps
	{
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;
		float Yaw;
		float Pitch;
		ViewMatrixProps()
		:Front(glm::vec3(0.0f,0.0f,-1.0f)), WorldUp(glm::vec3(0.0f,1.0f,0.0f)),Yaw(-90.0f),Pitch(0.0f){
		
			Front.x = cos(glm::radians(Yaw))*cos(glm::radians(Pitch));
			Front.y = sin(glm::radians(Pitch));
			Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(Front);

			Right = glm::normalize(glm::cross(Front, WorldUp));
			Up = glm::normalize(glm::cross(Right, Front));
		}
	};

	virtual ~Camera() = default;
	inline glm::vec3 GetPosition() const { return m_Position; }
	inline glm::vec3 Front() const { return m_ViewMatrixProps.Front; }
	inline glm::vec3 Up() const { return m_ViewMatrixProps.Up; }
	inline glm::vec3 Right() const { return m_ViewMatrixProps.Right; }
	inline float Yaw() const { return m_ViewMatrixProps.Yaw; }
	inline float Pitch() const { return m_ViewMatrixProps.Pitch; }



	inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	inline const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	void RecalculateViewMatrix();
	void RecalculateViewMatrix(float yaw, float pitch);
	
	inline void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
	inline void SetRotation(float yaw, float pitch) {
		m_ViewMatrixProps.Yaw = yaw,
		m_ViewMatrixProps.Pitch = pitch; 
		RecalculateViewMatrix(yaw, pitch);
	}
	
	inline void SetViewMatrix(glm::mat4 view) { m_ViewMatrix = view; m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix; }
	inline void SetProjectionMatrix(glm::mat4 projection) { m_ProjectionMatrix = projection; m_ProjectionMatrix * m_ViewMatrix; }
	inline void SetViewProjectionMatrix(glm::mat4 viewProjection) { m_ViewProjectionMatrix = viewProjection; }
	static Camera* Create(unsigned int type, const std::initializer_list<float> &projectionMatrixProps,const ViewMatrixProps &viewMatrixProps = ViewMatrixProps());

protected:
	glm::vec3 m_Position = { 0.0f,0.0f,0.0f };
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewProjectionMatrix;

	ViewMatrixProps m_ViewMatrixProps;
	


};

