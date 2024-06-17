#pragma once
#include "BlackPearl/Component/Component.h"

namespace BlackPearl {
	class TerrainComponent : public Component<TerrainComponent>
	{
	public:
		TerrainComponent(uint32_t width, uint32_t height, uint32_t chunkCntX, uint32_t chunkCntZ, uint32_t vertexPerChunk = 4);
		~TerrainComponent();

		uint32_t GetChunkCnt() const{ return m_ChunckCntX * m_ChunckCntZ; }
		uint32_t GetVertexPerChunk() const { return m_VertexPerChunk; }
		const std::vector<float>& GetVertices() const { return m_Vertices; }
		void SetTessLevel(uint32_t level) { m_StaticTessLevel = level; }
		uint32_t GetStaticTessLevel() const{ return m_StaticTessLevel; }

		void SetDynamicTess(bool dynamic) { m_DynamicTess = dynamic; }
		bool GetDynamicTess() const { return m_DynamicTess; }

	private:
		void CreateVertices();

		// chunck num in x axis
		uint32_t m_ChunckCntX;
		// chunck num in z axis
		uint32_t m_ChunckCntZ;
		uint32_t m_Width;
		uint32_t m_Height;

		uint32_t m_VertexPerChunk;

		// controls tessellation levels according to the distance from camera to patch/chunk
		bool	 m_DynamicTess;
		uint32_t m_StaticTessLevel;

		std::vector<float> m_Vertices;
	};

}

