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

		std::vector<uint32_t> indices = {
			0,1,2,
			1,2,3
		};
		SetVertices(vertices);
	//	SetIndices(indices);

		for (size_t i = 0; i < 6; i++)
		{
			positionData.push_back({ vertices[i * 8],vertices[i * 8 + 1],vertices[i * 8 + 2] });
			normalData.push_back({vertices[i * 8 + 3],vertices[i * 8 + 4] ,vertices[i * 8 + 5] });
			texcoord1Data.push_back({ vertices[i * 8 + 6],vertices[i * 8 + 7] });
		}
		indexData = indices;


	}
	PlaneMeshFilter::~PlaneMeshFilter()
	{
	}
}
