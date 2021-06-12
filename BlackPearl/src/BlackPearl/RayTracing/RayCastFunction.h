#pragma once
#include "Ray.h"
#include "HitRst.h"
namespace BlackPearl {
	
	//Ray Hitº¯Êý
	HitRst RayInSphere(std::shared_ptr<Ray> &ray);
	HitRst RayInBVHNode();
	

}

