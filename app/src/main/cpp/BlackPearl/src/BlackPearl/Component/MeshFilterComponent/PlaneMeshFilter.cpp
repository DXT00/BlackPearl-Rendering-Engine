#include"pch.h"
#include "PlaneMeshFilter.h"
namespace BlackPearl {
	void PlaneMeshFilter::Init()
	{
		std::vector<float> vertices = {
		// Positions          // Normals         // Texture Coords
		 5.0f, -0.5f,  5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 2.0f,
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
