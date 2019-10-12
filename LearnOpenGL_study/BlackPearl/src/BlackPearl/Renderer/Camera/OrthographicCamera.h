#pragma once
#include "glm/glm.hpp"
#include"Camera.h"
namespace BlackPearl {

	class OrthographicCamera :public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, const ViewMatrixProps &viewMatrixProps);
		virtual ~OrthographicCamera() = default;



	};
}
