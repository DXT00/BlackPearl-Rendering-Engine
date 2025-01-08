#include "pch.h"
#include "QuadMeshFilter.h"

namespace BlackPearl {


	QuadMeshFilter::~QuadMeshFilter()
	{
	}

	void QuadMeshFilter::Init() {
		std::vector<float> vertices = {

		   // positions          //normal             // texCoords
		   -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,
		   -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,	  0.0f, 0.0f,
			1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,	  1.0f, 0.0f,
							
		   -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,
			1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,	  1.0f, 0.0f,
			1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,	  1.0f, 1.0f,


		};
		SetVertices(vertices);

		positionData.push_back({ -1.0f,  1.0f, 0.0f });
		positionData.push_back({ -1.0f, -1.0f, 0.0f });
		positionData.push_back({ 1.0f, -1.0f, 0.0f });
		positionData.push_back({ -1.0f,  1.0f, 0.0f });
		positionData.push_back({ 1.0f, -1.0f, 0.0f });
		positionData.push_back({ 1.0f,  1.0f, 0.0f });

		normalData.push_back({ 0.0f, 0.0f, 1.0f });
		normalData.push_back({ 0.0f, 0.0f, 1.0f });
		normalData.push_back({ 0.0f, 0.0f, 1.0f });
		normalData.push_back({ 0.0f, 0.0f, 1.0f });
		normalData.push_back({ 0.0f, 0.0f, 1.0f });
		normalData.push_back({ 0.0f, 0.0f, 1.0f });

		texcoord1Data.push_back({ 0.0f, 1.0f });
		texcoord1Data.push_back({ 0.0f, 0.0f });
		texcoord1Data.push_back({ 1.0f, 0.0f });
		texcoord1Data.push_back({ 0.0f, 1.0f });
		texcoord1Data.push_back({ 1.0f, 0.0f });
		texcoord1Data.push_back({ 1.0f, 1.0f });

		std::vector<uint32_t> indices = {
			0,1,2,
			1,2,3
		};
		indexData = indices;
		SetIndices(indexData);
	}
}

