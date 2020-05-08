#pragma once
#include<vector>
#include"Area.h"
#include<glm/glm.hpp>
#include"BlackPearl/LightProbes/LightProbes.h"
namespace BlackPearl {
	//Map is a Cube
	class MapManager
	{
	public:
		MapManager(unsigned int mapSize,unsigned int areaSize)
		:m_MapSize(mapSize),m_AreaSize(areaSize){
			m_HalfMapSize = m_MapSize / 2.0;
			m_AreaCount = mapSize / areaSize;
			m_TotalAreaCount = m_AreaCount * m_AreaCount * m_AreaCount;
			for (int y = 0; y < m_AreaCount; y++)
			{
				for (int z = 0; z < m_AreaCount; z++)
				{
					for (int x = 0; x < m_AreaCount; x++)
					{
						unsigned int areaId = x + z * m_AreaCount + y * m_AreaCount * m_AreaCount;
						m_AreasList.push_back(Area(areaId, areaSize,x,y,z));
					}
				}

			}
		}
		unsigned int GetMapSize()const { return m_MapSize; }
		unsigned int GetAreaSize()const { return m_AreaSize; }
		unsigned int GetHalfMapSize()const { return m_HalfMapSize; }

		Area GetArea(unsigned int areaId);
		int CalculateAreaId(glm::vec3 pos);
		std::set<unsigned int> FindNearByArea(glm::vec3 pos);
		unsigned int AddProbeIdToArea(glm::vec3 probePos,unsigned int probeId);
		//判断probe是否跨区
		void UpdateProbesArea(std::vector<LightProbe*> probes);
		std::vector<Area> GetAreasList()const { return m_AreasList; }
		
		//GBufferRender::RenderSceneWithGBufferAndProbes中设置为false
		bool m_ProbeGridPosChanged = false;

	private:
		unsigned int m_AreaCount;//one dimemsion
		unsigned int m_TotalAreaCount;
		unsigned int m_MapSize;
		unsigned int m_HalfMapSize;

		unsigned int m_AreaSize;
		float m_Border = 2.0f;
		std::vector<Area> m_AreasList;
	};


}

