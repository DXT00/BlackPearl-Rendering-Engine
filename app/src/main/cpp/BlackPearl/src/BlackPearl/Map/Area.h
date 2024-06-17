#pragma once
#include<vector>
namespace BlackPearl {
	class Area
	{
	public:
		Area(unsigned int id,unsigned int areaSize,unsigned int x,unsigned int y,unsigned z) 
			:m_Id(id),m_X(x),m_Y(y),m_Z(z) {
			m_Xmin = m_X* areaSize;
			m_Xmax = m_Xmin + areaSize;
			m_Ymin = m_Y * areaSize;
			m_Ymax = m_Ymin + areaSize;
			m_Zmin = m_Z * areaSize;
			m_Zmax = m_Zmin + areaSize;
		}
		std::vector<unsigned int> GetProbesId() const{ return m_ProbesId; }
		void AddProbeId(unsigned int probeId);
		void DeleteProbeId(unsigned int probeId);
		unsigned int GetId()const { return m_Id; }
		unsigned int m_Xmin, m_Xmax, m_Ymin, m_Ymax, m_Zmin, m_Zmax;
		unsigned int m_X, m_Y, m_Z;
	private:
		unsigned int m_Id;
		std::vector<unsigned int> m_ProbesId;

	};

}