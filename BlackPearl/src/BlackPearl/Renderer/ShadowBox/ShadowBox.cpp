#include "pch.h"
#include "ShadowBox.h"
#include "BlackPearl/Renderer/MasterRenderer/ShadowMapRenderer.h"
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace BlackPearl {

	float ShadowBox::s_ShadowDistance = 30.0f;
	float ShadowBox::s_Offset = 10.0f;
	glm::vec4 ShadowBox::s_Up = glm::vec4(0, 1, 0, 0);
	glm::vec4 ShadowBox::s_Forward = glm::vec4(0, 0, -1, 0);
	ShadowBox::~ShadowBox()
	{
	}

	void ShadowBox::CalculateCameraWidthsAndHeight()
	{
		m_FarWidth = s_ShadowDistance * glm::tan(glm::radians(m_Camera->GetFov()));
		m_NearWidth = m_Camera->GetZnear()* glm::tan(glm::radians(m_Camera->GetFov()));

		m_FarHeight = m_FarWidth / m_Camera->GetAspectRadio();
		m_NearHeight = m_NearWidth / m_Camera->GetAspectRadio();
	}

	void ShadowBox::Update()
	{
		//glm::mat4 rotation = CalculateCameraRotationMatrix();
		glm::vec3 forwardVector = m_Camera->Front();//glm::vec3(rotation*s_Forward);
		glm::vec3 toNear = glm::vec3(forwardVector);
		toNear = toNear * m_Camera->GetZnear();
		glm::vec3 toFar = glm::vec3(forwardVector);
		toFar = toFar * s_ShadowDistance;
		glm::vec3 centerNear = m_Camera->GetPosition() + toNear;
		glm::vec3 centerFar = m_Camera->GetPosition() + toFar;
		std::vector<glm::vec4>points = CalculateFrustumVertices(glm::mat4(1.0f),forwardVector,centerNear,centerFar);
		bool first = true;
		for (auto point : points) {
			if (first) {
				m_MinX = point.x;
				m_MaxX = point.x;
				m_MinY = point.y;
				m_MaxY = point.y;
				m_MinZ = point.z;
				m_MaxZ = point.z;
				first = false;
				continue;
			}
			if (point.x > m_MaxX) {
				m_MaxX = point.x;
			}
			else if (point.x < m_MinX) {
				m_MinX = point.x;
			}
			if (point.y > m_MaxY) {
				m_MaxY = point.y;
			}
			else if (point.y < m_MinY) {
				m_MinY = point.y;
			}
			if (point.z > m_MaxZ) {
				m_MaxZ = point.z;
			}
			else if (point.z < m_MinZ) {
				m_MinZ = point.z;
			}
		}
		m_MaxZ += s_Offset;
	}

	glm::mat4 ShadowBox::CalculateCameraRotationMatrix()
	{
		glm::mat4 rotation(1.0f);
		rotation = glm::rotate(rotation, glm::radians(-m_Camera->GetViewMatrixProps().Yaw), glm::vec3(0, 1, 0));
		rotation = glm::rotate(rotation, glm::radians(-m_Camera->GetViewMatrixProps().Pitch), glm::vec3(1, 0, 0));

		return rotation;
	}

	std::vector<glm::vec4> ShadowBox::CalculateFrustumVertices(glm::mat4 rotation, glm::vec3 forwardVector, glm::vec3 centerNear, glm::vec3 centerFar)
	{
		glm::vec3 up = m_Camera->Up();//glm::vec3(rotation*s_Up);
		glm::vec3 right =m_Camera->Right();// glm::cross(forwardVector, up);
		glm::vec3 down = glm::vec3(-up.x, -up.y, -up.z);
		glm::vec3 left = glm::vec3(-right.x, -right.y, -right.z);
		glm::vec3 farTop, farBottom, nearTop, nearBottom;
		farTop = centerFar + up * m_FarHeight;
		farBottom = centerFar + down * m_FarHeight;
		nearTop = centerNear + up * m_NearHeight;
		nearBottom = centerNear + down + m_NearHeight;

		std::vector<glm::vec4>points(8);
		points[0] = CalculateLightSpaceFrustumCorner(farTop,  right, m_FarWidth);
		points[1] = CalculateLightSpaceFrustumCorner(farTop,  left,  m_FarWidth);
		points[2] = CalculateLightSpaceFrustumCorner(farBottom, right, m_FarWidth);
		points[3] = CalculateLightSpaceFrustumCorner(farBottom, left, m_FarWidth);
		points[4] = CalculateLightSpaceFrustumCorner(nearTop, right, m_FarWidth);
		points[5] = CalculateLightSpaceFrustumCorner(nearTop, left, m_FarWidth);
		points[6] = CalculateLightSpaceFrustumCorner(nearBottom, right, m_FarWidth);
		points[7] = CalculateLightSpaceFrustumCorner(nearBottom, left, m_FarWidth);
		return points;

	}

	glm::vec4 ShadowBox::CalculateLightSpaceFrustumCorner(glm::vec3 startPoint, glm::vec3 direction, float width)
	{
		glm::vec3 point =  startPoint + direction * width;
		glm::vec4 point4f = glm::vec4(point, 1.0f);
		return m_LightViewMatrix * point4f;
	}

	float ShadowBox::GetWidth()
	{
		return m_MaxX-m_MinX;
	}

	float ShadowBox::GetHeight()
	{
		return m_MaxY-m_MinY;
	}

	float ShadowBox::GetLength()
	{
		return m_MaxZ-m_MinZ;
	}

	glm::vec3 ShadowBox::GetCenter()
	{
		//需要把它转化为世界坐标系，所以需要乘以lightViewMatrix的逆矩阵
		float x = (m_MinX + m_MaxX) / 2.0f;
		float y = (m_MinY + m_MaxY) / 2.0f;
		float z = (m_MinZ + m_MaxZ) / 2.0f;
		glm::mat4 invertedLight = glm::inverse(m_LightViewMatrix);
		return invertedLight * glm::vec4(x, y, z, 1.0f);
	}

}
