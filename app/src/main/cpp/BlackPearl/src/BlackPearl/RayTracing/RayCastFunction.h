#pragma once
#include "Ray.h"
#include "HitRst.h"
namespace BlackPearl {
	
	//Ray Hit����
	HitRst RayInSphere(std::shared_ptr<Ray> &ray);
	HitRst RayInBVHNode();
	

}

