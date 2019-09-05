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

	virtual ~Camera() = default;
	inline float GetRotation()const { return m_Rotation; }
	inline glm::vec3 GetPosition() const { return m_Position; }

	inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	inline const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	void RecalculateViewMatrix();

	inline void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
	inline void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix();}
	
	inline void SetViewMatrix(glm::mat4 view) { m_ViewMatrix = view; m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix; }
	inline void SetProjectionMatrix(glm::mat4 projection) { m_ProjectionMatrix = projection; m_ProjectionMatrix * m_ViewMatrix; }
	inline void SetViewProjectionMatrix(glm::mat4 viewProjection) { m_ViewProjectionMatrix = viewProjection; }
	static Camera* Create(unsigned int type, std::initializer_list<float> l);

protected:
	glm::vec3 m_Position = { 0.0f,0.0f,0.0f };
	float m_Rotation = 0.0f;
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewProjectionMatrix;

};

