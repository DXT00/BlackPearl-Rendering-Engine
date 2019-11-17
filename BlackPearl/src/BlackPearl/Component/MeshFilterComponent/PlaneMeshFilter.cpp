#include"pch.h"
#include "PlaneMeshFilter.h"
namespace BlackPearl {
	void PlaneMeshFilter::Init()
	{
		std::vector<float> vertices = {
			// 5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
			//-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
			//-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,
			// 5.0f, -0.5f, -5.0f,  1.0f, 1.0f
			 // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
			//normal
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f, 0.0f, 1.0f, 0.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f, 0.0f, 1.0f, 0.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f, 0.0f, 1.0f, 0.0f,
		};

		std::vector<unsigned int> indices = {
			0,1,2,
			1,2,3
		};
		SetVertices(vertices);
	//	SetIndices(indices);


	}
	PlaneMeshFilter::~PlaneMeshFilter()
	{
	}
}
