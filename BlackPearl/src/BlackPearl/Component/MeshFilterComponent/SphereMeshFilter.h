#pragma once
#include "MeshFilter.h"
namespace BlackPearl {

	class SphereMeshFilter:public MeshFilter
	{
	public:
		SphereMeshFilter(float radius, unsigned int stackCount,unsigned int sectorCount)
			:MeshFilter() {
			m_Radius = radius;
			m_StackCount = stackCount;
			m_SectorCount = sectorCount;
			Init();
		}
		void Init();
		float GetRadius() const { return m_Radius; }
		~SphereMeshFilter();
	private:
		float m_Radius;
		unsigned int m_StackCount;//经度分区个数
		unsigned int m_SectorCount;//纬度分区个数

	};

}

