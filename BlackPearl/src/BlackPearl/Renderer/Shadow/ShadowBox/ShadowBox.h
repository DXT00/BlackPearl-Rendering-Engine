#pragma once
#include <vector>
#include"glm/glm.hpp"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
namespace BlackPearl {
	class ShadowBox
	{
	public:
		ShadowBox(glm::mat4 lightViewMatrix, PerspectiveCamera *camera) {
			m_LightViewMatrix = lightViewMatrix;
			m_Camera = camera;
			CalculateCameraWidthsAndHeight();
		};
		~ShadowBox();
	
		/**
	 * Calculates the width and height of the near and far planes of the
	 * camera's view frustum. However, this doesn't have to use the "actual" far
	 * plane of the view frustum. It can use a shortened view frustum if desired
	 * by bringing the far-plane closer, which would increase shadow resolution
	 * but means that distant objects wouldn't cast shadows.
	 */
		void CalculateCameraWidthsAndHeight();
		/**
	 * Updates the bounds of the shadow box based on the light direction and the
	 * camera's view frustum, to make sure that the box covers the smallest area
	 * possible while still ensuring that everything inside the camera's view
	 * (within a certain range) will cast shadows.
	 */
		void Update();



		glm::mat4 CalculateCameraRotationMatrix();

		std::vector<glm::vec4> CalculateFrustumVertices(
			glm::mat4 rotation,
			glm::vec3 forwardVector,
			glm::vec3 centerNear,
			glm::vec3 centerFar);

		glm::vec4 CalculateLightSpaceFrustumCorner(glm::vec3 startPoint, glm::vec3 direction, float width);
		float GetWidth();
		float GetHeight();
		float GetLength();
		glm::vec3 GetCenter();
		static float s_ShadowDistance;


	private:
		static glm::vec4 s_Up;
		static glm::vec4 s_Forward;
		static float s_Offset;
		float m_MinX, m_MaxX;
		float m_MinY, m_MaxY;
		float m_MinZ, m_MaxZ;
		glm::mat4 m_LightViewMatrix;
		PerspectiveCamera *m_Camera;
		float m_FarHeight, m_FarWidth, m_NearHeight, m_NearWidth;
	};
}


