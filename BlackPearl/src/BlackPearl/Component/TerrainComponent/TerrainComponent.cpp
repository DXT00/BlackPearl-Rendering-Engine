#include "pch.h"
#include "TerrainComponent.h"
namespace BlackPearl {
	TerrainComponent::TerrainComponent(uint32_t width, uint32_t height, uint32_t chunkCntX, uint32_t chunkCntZ, uint32_t vertexPerChunk)
		:Component(Component::Type::Terrain),
		m_Width(width), 
		m_Height(height), 
		m_ChunckCntX(chunkCntX),
		m_ChunckCntZ(chunkCntZ),
		m_VertexPerChunk(vertexPerChunk),
		m_StaticTessLevel(16),
		m_DynamicTess(false)
	{
		CreateVertices();
	}


	TerrainComponent::~TerrainComponent()
	{
	}
	void TerrainComponent::CreateVertices()
	{
		float chunkSizeX = (float)m_Width / m_ChunckCntX;
		float chunkSizeZ = (float)m_Height / m_ChunckCntZ;

		for (size_t i = 0; i < m_ChunckCntX; i++)
		{
			for (size_t j = 0; j < m_ChunckCntZ; j++)
			{
				float a = m_Width / -2.0f + i * chunkSizeX;
				m_Vertices.push_back(-1.0 * m_Width / 2.0f + i * chunkSizeX);
				m_Vertices.push_back(0.0);
				m_Vertices.push_back(-1.0 * m_Height / 2.0f + j * chunkSizeZ);
				m_Vertices.push_back(i/(float)m_ChunckCntX);
				m_Vertices.push_back(j/(float)m_ChunckCntZ);

				m_Vertices.push_back(-1.0 * m_Width / 2.0f + (i+1) * chunkSizeX);
				m_Vertices.push_back(0.0);
				m_Vertices.push_back(-1.0 * m_Height / 2.0f + j * chunkSizeZ);
				m_Vertices.push_back((i+1)/ (float)m_ChunckCntX);
				m_Vertices.push_back(j / (float)m_ChunckCntZ);

				m_Vertices.push_back(-1.0 * m_Width / 2.0f + i * chunkSizeX);
				m_Vertices.push_back(0.0);
				m_Vertices.push_back(-1.0 * m_Height / 2.0f + (j+1) * chunkSizeZ);
				m_Vertices.push_back(i / (float)m_ChunckCntX);
				m_Vertices.push_back((j+1) / (float)m_ChunckCntZ);

				m_Vertices.push_back(-1.0 * m_Width / 2.0f + (i+1) * chunkSizeX);
				m_Vertices.push_back(0.0);
				m_Vertices.push_back(-1.0 * m_Height / 2.0f + (j + 1) * chunkSizeZ);
				m_Vertices.push_back((i +1)/ (float)m_ChunckCntX);
				m_Vertices.push_back((j + 1) / (float)m_ChunckCntZ);
			}
		}
	}
}