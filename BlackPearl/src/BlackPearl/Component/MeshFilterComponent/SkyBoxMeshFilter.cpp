#include "pch.h"
#include "SkyBoxMeshFilter.h"

namespace BlackPearl {


	void SkyBoxMeshFilter::Init()
	{
		std::vector<float> vertices = {
			//back
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			//left
			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,
			//right
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 //front
			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,
			//top
			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,
			//botttom
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f


			// //right
			// 1.0f, -1.0f, -1.0f,
			// 1.0f, -1.0f,  1.0f,
			// 1.0f,  1.0f,  1.0f,
			// 1.0f,  1.0f,  1.0f,
			// 1.0f,  1.0f, -1.0f,
			// 1.0f, -1.0f, -1.0f,
			////left
			//-1.0f, -1.0f,  1.0f,
			//-1.0f, -1.0f, -1.0f,
			//-1.0f,  1.0f, -1.0f,
			//-1.0f,  1.0f, -1.0f,
			//-1.0f,  1.0f,  1.0f,
			//-1.0f, -1.0f,  1.0f,
			////top
			//-1.0f,  1.0f, -1.0f,
			// 1.0f,  1.0f, -1.0f,
			// 1.0f,  1.0f,  1.0f,
			// 1.0f,  1.0f,  1.0f,
			//-1.0f,  1.0f,  1.0f,
			//-1.0f,  1.0f, -1.0f,
			////botttom
			//-1.0f, -1.0f, -1.0f,
			//-1.0f, -1.0f,  1.0f,
			// 1.0f, -1.0f, -1.0f,
			// 1.0f, -1.0f, -1.0f,
			//-1.0f, -1.0f,  1.0f,
			// 1.0f, -1.0f,  1.0f,
			////front
		 //  -1.0f, -1.0f,  1.0f,
		 //  -1.0f,  1.0f,  1.0f,
			//1.0f,  1.0f,  1.0f,
			//1.0f,  1.0f,  1.0f,
			//1.0f, -1.0f,  1.0f,
		 //  -1.0f, -1.0f,  1.0f,
			////back
			//-1.0f,  1.0f, -1.0f,
			//-1.0f, -1.0f, -1.0f,
			// 1.0f, -1.0f, -1.0f,
			// 1.0f, -1.0f, -1.0f,
			// 1.0f,  1.0f, -1.0f,
			//-1.0f,  1.0f, -1.0f
		};
		SetVertices(vertices);
		math::float3 pos;
		for (size_t i = 0; i < (int)vertices.size()/3; i++)
		{
			positionData.push_back({ vertices[i*3], vertices[i * 3 + 1] ,vertices[i * 3 + 2] });
		}

	}
	SkyBoxMeshFilter::~SkyBoxMeshFilter()
	{
	}

}
