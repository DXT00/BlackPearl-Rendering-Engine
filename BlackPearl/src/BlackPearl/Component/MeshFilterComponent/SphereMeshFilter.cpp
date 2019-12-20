#include "pch.h"
#include "SphereMeshFilter.h"

namespace BlackPearl {

#define PI 3.14159265359

	void SphereMeshFilter::Init()
	{
		std::vector<float> vertices;


		const int sectorStep = 2 * PI / m_SectorCount;
		const int stackStep = PI / m_StackCount;
		for (int i = 0; i <= m_StackCount; i++)
		{
			float stackAngle = PI / 2.0 - PI * i / m_StackCount;
			float rCosPhi = m_Radius * cos(stackAngle);
			float rSinPhi = m_Radius * sin(stackAngle);
			for (int j = 0; j <= m_SectorCount; j++)
			{
				float sectorAngle = 2.0 * PI* j / m_SectorCount;
				float x = rCosPhi * cos(sectorAngle);
				float y = rCosPhi * sin(sectorAngle);
				float z = rSinPhi;
				//position
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
				//normal
				vertices.push_back(x / m_Radius);
				vertices.push_back(y / m_Radius);
				vertices.push_back(z / m_Radius);
				//texture uv
				vertices.push_back((float)j / m_SectorCount);
				vertices.push_back((float)i / m_StackCount);
			}
		}

		SetVertices(vertices); 
		std::vector<unsigned int> indices;
		for (int i = 0; i < m_StackCount; i++)
		{
			int k1 = i * (m_StackCount + 1);
			int k2 = k1 + m_StackCount + 1;
			for (int j = 0; j < m_SectorCount; j++, k1++, k2++)
			{
				//除了头一行和尾一行是三角形外
				//中间的行都是矩形（两个三角形）
				if (i != 0) {
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);

				}

				if (i != m_StackCount - 1) {

					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}

			}

		}
		SetIndices(indices);

	}

	SphereMeshFilter::~SphereMeshFilter()
	{
	}

}
