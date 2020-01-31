#pragma once
#include"Camera.h"
namespace BlackPearl {

	class PerspectiveCamera :public Camera
	{
	public:
		//float fov, float width, float height, float znear, float zfar,
		PerspectiveCamera(EntityManager * entityManager, Entity::Id id, const ViewMatrixProps &viewMatrixProps=ViewMatrixProps());
		virtual ~PerspectiveCamera() = default;

		void RecalculateProjectionMatrix();

		inline void SetFov(float fov) { m_Fov = fov; RecalculateProjectionMatrix(); }
		inline void SetZnear(float znear) { m_zNear = znear; RecalculateProjectionMatrix(); }
		inline void SetZfar(float zfar) { m_zFar = zfar; RecalculateProjectionMatrix(); }
		inline void SetHeight(float height) { m_Height = height; RecalculateProjectionMatrix(); }
		inline void SetWidth(float width) { m_Width = width; RecalculateProjectionMatrix(); }

		float GetFov() const{ return m_Fov; }
		float GetZnear()const { return m_zNear; }
		float GetZfar()const { return m_zFar; }
		float GetAspectRadio()const { return m_Width / m_Height; }
	private:
		float m_Fov;
		float m_Width, m_Height;
		float m_zNear, m_zFar;
		


	};

}