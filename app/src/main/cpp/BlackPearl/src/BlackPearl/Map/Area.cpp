#include "pch.h"
#include "Area.h"

namespace BlackPearl {

	void Area::AddProbeId(unsigned int probeId)
	{
		m_ProbesId.push_back(probeId);
	}

	void Area::DeleteProbeId(unsigned int probeId)
	{
		m_ProbesId.erase(std::remove(m_ProbesId.begin(), m_ProbesId.end(), probeId), m_ProbesId.end());
	}

}

