#pragma once
#include<vector>
#include"Area.h"
#include<glm/glm.hpp>
#include<BlackPearl/Object/Object.h>
#include "Core.h"
namespace BlackPearl {
	//Map is a Cube
	class MapManager
	{
	public:
		MapManager(float mapSize, float areaSize)
		:m_MapSize(mapSize),m_AreaSize(areaSize){
			m_HalfMapSize = m_MapSize / 2.0;
			m_AreaCount = mapSize / areaSize;
			m_TotalAreaCount = m_AreaCount * m_AreaCount * m_AreaCount;
			for (uint32_t y = 0; y < m_AreaCount; y++)
			{
				for (uint32_t z = 0; z < m_AreaCount; z++)
				{
					for (uint32_t x = 0; x < m_AreaCount; x++)
					{
						unsigned int areaId = x + z * m_AreaCount + y * m_AreaCount * m_AreaCount;
						m_AreasList.push_back(DBG_NEW Area(areaId, areaSize,x,y,z, m_HalfMapSize));
                       // GE_ASSERT(i == areaId, "areaId error");
					}
				}

			}
		}
		unsigned int GetMapSize()const { return m_MapSize; }
		unsigned int GetAreaSize()const { return m_AreaSize; }
		unsigned int GetHalfMapSize()const { return m_HalfMapSize; }

		Area* GetArea(int areaId);
		int CalculateAreaId(glm::vec3 pos);
		std::set<unsigned int> FindNearByArea(glm::vec3 pos);
        uint32_t AddProbeIdToArea(glm::vec3 probePos,unsigned int probeId);



		//判断probe是否跨区
		void UpdateProbesArea(std::vector<Object*> probes);
		std::vector<Area*> GetAreasList()const { 
            return m_AreasList; 
        
        }
		
		//GBufferRender::RenderSceneWithGBufferAndProbes中设置为false
		bool m_ProbeGridPosChanged = false;

        void AddObjectToArea(Object* obj);

	private:
		unsigned int m_AreaCount;//one dimemsion
		unsigned int m_TotalAreaCount;
        float m_MapSize;
		float m_HalfMapSize;

		unsigned int m_AreaSize;
		float m_Border = 2.0f;
		std::vector<Area*> m_AreasList;
	};


}

