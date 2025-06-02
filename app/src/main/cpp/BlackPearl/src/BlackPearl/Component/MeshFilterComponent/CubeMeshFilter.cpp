#include "pch.h"
#include "Component/MeshFilterComponent/CubeMeshFilter.h"

namespace BlackPearl {


	void CubeMeshFilter::Init()
	{
		//std::vector<float>  vertices = {//������˳��Ϊ��ʱ��-->�������޳�
		//	//// Positions          // Normals         // Texture Coords
		//	////Back face
		//	//-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		//	// 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		//	// 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,	
		//	// 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		//	//-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		//	//-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		//	////Front face
		//	//-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		//	// 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		//	// 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		//	// 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		//	//-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		//	//-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		//	////Left face
		//	//-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		//	//-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		//	//-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		//	//-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		//	//-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		//	//-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		//	////Right face
		//	// 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		//	// 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		//	// 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		//	// 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		//	// 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		//	// 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		//	// //Bottom face
		//	//-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		//	// 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		//	// 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		//	// 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		//	//-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		//	//-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		//	////Top face
		//	//-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		//	// 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		//	// 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		//	// 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		//	//-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		//	//-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f

		//	// back face
		//	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		//	 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		//	 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		//	 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		//	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		//	-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		//	// front face
		//	-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		//	 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		//	 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		//	 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		//	-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		//	-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		//	// left face
		//	-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		//	-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		//	-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		//	-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		//	-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		//	-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		//	// right face
		//	 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		//	 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		//	 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		//	 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		//	 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		//	 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		//	// bottom face
		//	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		//	 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		//	 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		//	 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		//	-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		//	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		//	// top face
		//	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		//	 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		//	 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		//	 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		//	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		//	-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left     
		//	
		//};
	

		// Cube vertices positions and corresponding triangle normals. total 24 point , The sequence must be:
        //Slot_aPos,Slot_aPrePos, Slot_aNormal, Slot_aTexCoords
		std::vector<float>  vertices =
		{
            // pos               //prePos,              normal,               uv
			-1.0f, 1.0f, -1.0f,   -1.0f, 1.0f, -1.0f,    0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
			1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
			1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
			-1.0f, 1.0f,  1.0f,   -1.0f, 1.0f,  1.0f,    0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
                                                                            
			-1.0f, -1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
			1.0f,  -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
			1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			-1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
                                                                            
			-1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
			-1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
			-1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

			1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
			1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
			1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f,

			-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 
			 1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
			 1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

			-1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,   0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
			 1.0f, -1.0f, 1.0f,   1.0f, -1.0f, 1.0f,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
			 1.0f,  1.0f, 1.0f,   1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
			-1.0f,  1.0f, 1.0f,  -1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f
		};

		SetVertices(vertices);
		//std::vector<uint32_t>  indices = {
		//	0,1,2,
		//	0,2,3,
		//	4,5,6,
		//	4,6,7,
		//	7,3,0,
		//	0,7,4,
		//	6,2,1,
		//	6,1,5,
		//	0,1,5,
		//	0,4,5,
		//	3,2,6,
		//	3,7,6

//		};
// 		   			// Cube indices.
		std::vector<uint32_t>  indices =
		{
			3,1,0,
			2,1,3,

			6,4,5,
			7,4,6,

			11,9,8,
			10,9,11,

			14,12,13,
			15,12,14,

			19,17,16,
			18,17,19,

			22,20,21,
			23,20,22
		};
		SetIndices(indices);

        int dataPerVertex = 11;//pos+prePos+normal+uv, 

		for (size_t i = 0; i < (int)vertices.size()/ dataPerVertex; i++)
		{
			positionData.push_back({ vertices[i * dataPerVertex],vertices[i * dataPerVertex + 1],vertices[i * dataPerVertex + 2] });
			prePositionData.push_back({ vertices[i * dataPerVertex + 3],vertices[i * dataPerVertex + 4],vertices[i * dataPerVertex + 5] });
			normalData.push_back({ vertices[i * dataPerVertex + 6],vertices[i * dataPerVertex + 7] ,vertices[i * dataPerVertex + 8] });
			texcoordData.push_back({ vertices[i * dataPerVertex + 9],vertices[i * dataPerVertex + 10] });
		}
		indexData = indices;
	}

	CubeMeshFilter::~CubeMeshFilter()
	{
	}
}