#pragma once
#include "glm/glm.hpp"
#include"Camera.h"
class OrthographicCamera :public Camera
{
public:
	OrthographicCamera(float left, float right, float bottom, float top);
	virtual ~OrthographicCamera() = default;

	
	
};

