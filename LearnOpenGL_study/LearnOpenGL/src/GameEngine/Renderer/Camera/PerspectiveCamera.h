#pragma once
#include"Camera.h"
class PerspectiveCamera :public Camera
{
public:
	PerspectiveCamera(float fov, float width, float height, float znear, float zfar, const ViewMatrixProps &viewMatrixProps);
	virtual ~PerspectiveCamera() = default;

	void RecalculateProjectionMatrix();

	inline void SetFov(float fov) {m_Fov = fov; RecalculateProjectionMatrix();}
	inline void SetZnear(float znear) { m_zNear = znear; RecalculateProjectionMatrix(); }
	inline void SetZfar(float zfar) { m_zFar = zfar; RecalculateProjectionMatrix(); }



private:
	float m_Fov;
	float m_Width, m_Height;
	float m_zNear, m_zFar;


};

