#include "pch.h"
#include "MapManager.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Component/LightProbeComponent/LightProbeComponent.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
namespace BlackPearl {
	Area MapManager::GetArea(unsigned int areaId)
	{
		GE_ASSERT(areaId < m_AreasList.size(), "areaId>=m_AreasList.size()!");
		return m_AreasList[areaId];
	}

	int MapManager::CalculateAreaId(glm::vec3 pos)
	{
		unsigned int halfMapSize = m_MapSize / 2.0;
		if (abs(pos.x) < halfMapSize && abs(pos.y) < halfMapSize && abs(pos.z) < halfMapSize) {
			pos += glm::vec3(halfMapSize, halfMapSize, halfMapSize);
			int x = pos.x / m_AreaCount;
			int y = pos.y / m_AreaCount;
			int z = pos.z / m_AreaCount;
			return  x + z * m_AreaCount + y * m_AreaCount * m_AreaCount;
		}
		else
			return -1;

	}

	std::set<unsigned int> MapManager::FindNearByArea(glm::vec3 pos)
	{
		unsigned int halfMapSize = m_MapSize / 2.0;
		float SQRT2 = sqrt(2.0f);
		float SQRT3 = sqrt(3.0f);
	
		std::set<unsigned int> nearByArea;
		unsigned int areaId = CalculateAreaId(pos);
		unsigned int nearById;
		Area area = m_AreasList[areaId];
	
		//26 direction
		glm::vec3 positivePos = pos + glm::vec3(halfMapSize, halfMapSize, halfMapSize);
		if (positivePos.x - area.m_Xmin < m_Border) {
			//-x
			nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(-1, 0, 0));
			if(nearById!=-1)
				nearByArea.insert(nearById);
			//-x,-y
			if (positivePos.y - area.m_Ymin < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(-SQRT2, -SQRT2, 0));
				if (nearById != -1)
					nearByArea.insert(nearById);
				//-x,-y,+z
				if (-positivePos.z + area.m_Zmax < m_Border) {
					nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(-SQRT3, -SQRT3, SQRT3));
					if (nearById != -1)
						nearByArea.insert(nearById);
				}
				//-x,-y,-z
				if (positivePos.z - area.m_Zmin < m_Border) {
					nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(-SQRT3, -SQRT3, -SQRT3));
					if (nearById != -1)
						nearByArea.insert(nearById);
				}
			}
			//-x,+y
			if (-positivePos.y + area.m_Ymax < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(-SQRT2, SQRT2, 0));
				if (nearById != -1)
					nearByArea.insert(nearById);
				//-x,+y,+z
				if (-positivePos.z + area.m_Zmax < m_Border) {
					nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(-SQRT3,SQRT3,SQRT3));
					if (nearById != -1)
						nearByArea.insert(nearById);
				}
				//-x,+y,-z
				if (positivePos.z - area.m_Zmin < m_Border) {
					nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(-SQRT3, SQRT3, -SQRT3));
					if (nearById != -1)
						nearByArea.insert(nearById);
				}
			}
			//-x,-z
			if (positivePos.z - area.m_Zmin < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(-SQRT2,0, -SQRT2));
				if (nearById != -1)
					nearByArea.insert(nearById);
			}
			//-x,+z
			if (-positivePos.z + area.m_Zmax < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(-SQRT2, 0, SQRT2));
				if (nearById != -1)
					nearByArea.insert(nearById);
			}
		}
		if (area.m_Xmax- positivePos.x < m_Border) {
			//+x
			nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(1, 0, 0));
			if (nearById != -1)
				nearByArea.insert(nearById);
			//+x,-y
			if (positivePos.y - area.m_Ymin < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(SQRT2,-SQRT2, 0 ));
				if (nearById != -1)
					nearByArea.insert(nearById);
				//+x,-y,+z
				if (-positivePos.z + area.m_Zmax < m_Border) {
					nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(SQRT3, -SQRT3, SQRT3));
					if (nearById != -1)
						nearByArea.insert(nearById);
				}
				//+x,-y,-z
				if (positivePos.z - area.m_Zmin < m_Border) {
					nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(SQRT3, -SQRT3, -SQRT3));
					if (nearById != -1)
						nearByArea.insert(nearById);
				}
			}
			//+x,+y
			if (-positivePos.y +  area.m_Ymax < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(SQRT2, SQRT2, 0));
				if (nearById != -1)
					nearByArea.insert(nearById);
				//+x,+y,+z
				if (-positivePos.z + area.m_Zmax < m_Border) {
					nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(SQRT3, SQRT3, SQRT3));
					if (nearById != -1)
						nearByArea.insert(nearById);
				}
				//+x,+y,-z
				if (positivePos.z - area.m_Zmin < m_Border) {
					nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(SQRT3, SQRT3, -SQRT3));
					if (nearById != -1)
						nearByArea.insert(nearById);
				}
			}
			//+x,-z
			if (positivePos.z - area.m_Zmin < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(SQRT2, 0, -SQRT2));
				if (nearById != -1)
					nearByArea.insert(nearById);
			}
			//+x,+z
			if (-positivePos.z + area.m_Zmax < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(SQRT2, 0, SQRT2));
				if (nearById != -1)
					nearByArea.insert(nearById);
			}
		}
		if (area.m_Ymax - positivePos.y < m_Border) {
			//+y
			nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(0, 1, 0));
			if (nearById != -1)
				nearByArea.insert(nearById);
			//+y,+z
			if (area.m_Zmax - positivePos.z < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(SQRT2, 0, SQRT2));
				if (nearById != -1)
					nearByArea.insert(nearById);
			}
			//+y,-z
			if (positivePos.z - area.m_Zmin < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(SQRT2, 0, -SQRT2));
				if (nearById != -1)
					nearByArea.insert(nearById);
			}
		}
		if (positivePos.y-area.m_Ymin < m_Border) {
			//-y
			nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(0, -1, 0));
			if (nearById != -1)
				nearByArea.insert(nearById);
			//-y,+z
			if (area.m_Zmax - positivePos.z < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(-SQRT2, 0, SQRT2));
				if (nearById != -1)
					nearByArea.insert(nearById);
			}
			//-y,-z
			if (positivePos.z - area.m_Zmin < m_Border) {
				nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(-SQRT2, 0, -SQRT2));
				if (nearById != -1)
					nearByArea.insert(nearById);
			}

		}
		if (area.m_Zmax - positivePos.z < m_Border) {
			//+z
			nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(0, 0, 1));
			if (nearById != -1)
				nearByArea.insert(nearById);
		}
		if (positivePos.z - area.m_Zmin < m_Border) {
			//-z
			nearById = CalculateAreaId(pos + (float)m_AreaSize * glm::vec3(0, 0, -1));
			if (nearById != -1)
				nearByArea.insert(nearById);
		}
	
		return nearByArea;
	}

	unsigned int MapManager::AddProbeIdToArea(glm::vec3 probePos,unsigned int probeId)
	{
		unsigned int areaId = CalculateAreaId(probePos);
		GE_ASSERT(areaId != -1, "probe position out of Map range!");
		m_AreasList[areaId].AddProbeId(probeId);
		return areaId;
	}

	void MapManager::UpdateProbesArea(std::vector<Object*> probes)
	{
		m_ProbeGridPosChanged = false;
		for (int i = 0; i < probes.size();i++) {
			unsigned int lastAreaId = probes[i]->GetComponent<LightProbe>()->GetAreaId();
			unsigned int newAreaId = CalculateAreaId(probes[i]->GetComponent<Transform>()->GetPosition());
			unsigned int probeId = i;
			if (lastAreaId != newAreaId) {
			
				m_AreasList[lastAreaId].DeleteProbeId(i);
				m_AreasList[newAreaId].AddProbeId(i);
				m_ProbeGridPosChanged = true;
			}
		}
	}


}

