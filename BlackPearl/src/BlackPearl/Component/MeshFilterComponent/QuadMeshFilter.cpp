#include "pch.h"
#include "QuadMeshFilter.h"

namespace BlackPearl {


	QuadMeshFilter::~QuadMeshFilter()
	{
	}

	void QuadMeshFilter::Init() {
		std::vector<float> vertices = {

		   // positions         // texCoords
		   -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		   -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
						 
		   -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			1.0f,  1.0f, 0.0f,  1.0f, 1.0f

		};
		SetVertices(vertices);

	}
}

