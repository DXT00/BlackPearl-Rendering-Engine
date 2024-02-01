#pragma once
#include "glm/glm.hpp"
#include<string>
#include<initializer_list>
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/RHI/DynamicModule.h"
namespace BlackPearl {

	class Camera :public Component<Camera>
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
				: WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)), Yaw(-90.0f), Pitch(0.0f) {
				if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL) {
					Front = glm::vec3(0.0f, 0.0f, -1.0f);
				}
				else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
					Front = glm::vec3(0.0f, 0.0f, 1.0f);
					Yaw = 90.0f;

				}
				Front.x = cos(glm::radians(Yaw))*cos(glm::radians(Pitch));
				Front.y = sin(glm::radians(Pitch));
				Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
				Front = glm::normalize(Front);

				Right = glm::normalize(glm::cross(Front, WorldUp));
				Up = glm::normalize(glm::cross(Right, Front));
				//D3D12是左手坐标系
				/*if (g_RHIType == DynamicRHI::Type::D3D12) {
					Right = -Right;
				}*/
			}
		};

		virtual ~Camera() = default;
		inline glm::vec3 GetPosition() const { return m_Position; }
		inline glm::vec3 GetRotation() const { return glm::vec3(m_ViewMatrixProps.Pitch, m_ViewMatrixProps.Yaw,0.0f); }

		inline glm::vec3 Front() const { return m_ViewMatrixProps.Front; }
		inline glm::vec3 Up() const { return m_ViewMatrixProps.Up; }
		inline glm::vec3 Right() const { return m_ViewMatrixProps.Right; }
		inline float Yaw() const { return m_ViewMatrixProps.Yaw; }
		inline float Pitch() const { return m_ViewMatrixProps.Pitch; }



		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline const glm::mat4& GetProjectionViewMatrix() const { return m_ViewProjectionMatrix; }

		void RecalculateViewMatrix();
		void RecalculateViewMatrix(float yaw, float pitch);

		/*for cubeMap 6 face rendering:positiveX,negativeX,positiveY,negativeY,positiveZ,negativeZ*/
		void SwitchToFace(unsigned int i);

		inline void SetPositionAndUpdateMatrix(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
		//TODO::
		inline void SetPositionOnly(const glm::vec3& position) { m_Position = position; }
		inline void SetRotationAndUpdateMatrix(float yaw, float pitch) {
			m_ViewMatrixProps.Yaw = yaw,
			m_ViewMatrixProps.Pitch = pitch;
			RecalculateViewMatrix(yaw, pitch);
		}

		inline void SetViewMatrix(glm::mat4 view) { m_ViewMatrix = view; m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix; }
		inline void SetProjectionMatrix(glm::mat4 projection) { m_ProjectionMatrix = projection; m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix; }
		inline void SetViewProjectionMatrix(glm::mat4 viewProjection) { m_ViewProjectionMatrix = viewProjection; }
		//static Camera* Create(unsigned int type, const std::initializer_list<float> &projectionMatrixProps, const ViewMatrixProps &viewMatrixProps = ViewMatrixProps());
		Camera():
		Component(BaseComponent::Type::Camera){}//TODO::
		static Camera * Create(unsigned int type);

		ViewMatrixProps GetViewMatrixProps() const { return m_ViewMatrixProps; }
	protected:
		glm::vec3 m_Position = { 0.0f,0.0f,0.0f };
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		ViewMatrixProps m_ViewMatrixProps;



	};

}