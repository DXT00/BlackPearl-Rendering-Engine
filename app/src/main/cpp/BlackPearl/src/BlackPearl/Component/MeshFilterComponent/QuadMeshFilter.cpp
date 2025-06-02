#include "pch.h"
#include "Component/MeshFilterComponent/QuadMeshFilter.h"

namespace BlackPearl {


	QuadMeshFilter::~QuadMeshFilter()
	{
	}

	void QuadMeshFilter::Init() {
		std::vector<float> vertices = {

		   // positions          //normal             // texCoords
            -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // 左上
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // 左下
             1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // 右下
             1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f  // 右上


		};
		SetVertices(vertices);

		positionData.push_back({ -1.0f,  1.0f, 0.0f });
		positionData.push_back({ -1.0f, -1.0f, 0.0f });
		positionData.push_back({  1.0f, -1.0f, 0.0f});
		positionData.push_back({  1.0f,  1.0f, 0.0f });


		normalData.push_back({ 0.0f, 0.0f, 1.0f });
		normalData.push_back({ 0.0f, 0.0f, 1.0f });
		normalData.push_back({ 0.0f, 0.0f, 1.0f });
		normalData.push_back({ 0.0f, 0.0f, 1.0f });


		texcoordData.push_back({ 0.0f, 0.0f });
		texcoordData.push_back({ 0.0f, 1.0f });
		texcoordData.push_back({ 1.0f, 1.0f });
		texcoordData.push_back({ 1.0f, 0.0f });


		std::vector<uint32_t> indices = {
         0, 1, 2,  // 第一个三角形（左上-左下-右下）
         0, 2, 3   // 第二个三角形（左上-右下-右上）
		};
		indexData = indices;
		SetIndices(indexData);
	}
}

