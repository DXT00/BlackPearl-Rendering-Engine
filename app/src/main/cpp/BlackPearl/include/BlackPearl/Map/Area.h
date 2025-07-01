#pragma once
#include<vector>
#include "Object/Object.h"
#include "Math/vector.h"

namespace BlackPearl {
	class Area
	{
	public:
		Area(uint32_t id, float areaSize, uint32_t x, uint32_t y, uint32_t z, float halfMapSize)
			:m_Id(id),m_X(x),m_Y(y),m_Z(z) {
			m_Xmin = m_X* areaSize;
			m_Xmax = m_Xmin + areaSize;
			m_Ymin = m_Y * areaSize;
			m_Ymax = m_Ymin + areaSize;
			m_Zmin = m_Z * areaSize;
			m_Zmax = m_Zmin + areaSize;

            // make range to [-halfMapSize, halfMapSize]
            m_Xmin -= halfMapSize;
            m_Xmax -= halfMapSize;
            m_Ymin -= halfMapSize;
            m_Ymax -= halfMapSize;
            m_Zmin -= halfMapSize;
            m_Zmax -= halfMapSize;

            
		}
		std::vector<unsigned int> GetProbesId() const{ return m_ProbesId; }
		void AddProbeId(unsigned int probeId);
		void DeleteProbeId(unsigned int probeId);
		unsigned int GetId()const { return m_Id; }
		float m_Xmin, m_Xmax, m_Ymin, m_Ymax, m_Zmin, m_Zmax;
		unsigned int m_X, m_Y, m_Z;

        void AddObject(Object* obj);
        std::vector<Object*> GetObjects() const;
        void DeleteObjects(Object* obj);
        math::float3 GetCenter() const;
        math::float3 GetExtend() const;

        void RegisterVoxelId(uint32_t voxelId) {
            m_VoxelId = voxelId;
        }
        uint32_t GetVoxelId() const { return m_VoxelId; }
	private:
		unsigned int m_Id;
		std::vector<unsigned int> m_ProbesId;
        std::vector<Object*> m_Objects;
        uint32_t m_VoxelId; 

	};

}