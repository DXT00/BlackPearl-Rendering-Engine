#include "pch.h"
#include "Map/Area.h"

namespace BlackPearl {

	void Area::AddProbeId(unsigned int probeId)
	{
		m_ProbesId.push_back(probeId);
	}

	void Area::DeleteProbeId(unsigned int probeId)
	{
		m_ProbesId.erase(std::remove(m_ProbesId.begin(), m_ProbesId.end(), probeId), m_ProbesId.end());
	}

    void Area::AddObject(Object* obj)
    {
        m_Objects.push_back(obj);
    }

    std::vector<Object*> Area::GetObjects() const {
        return m_Objects;
    }

    void Area::DeleteObjects(Object* obj) {
        std::vector<Object*> objs;
        for (size_t i = 0; i < m_Objects.size(); i++)
        {
            if (m_Objects[i] != obj) {
                objs.push_back(obj);
            }
        }
        m_Objects = objs;
    }

    math::float3 Area::GetCenter() const {

        float x = (float)(m_Xmax + m_Xmin) * 0.5f;
        float y = (float)(m_Ymax + m_Ymin) * 0.5f;
        float z = (float)(m_Zmax + m_Zmin) * 0.5f;


        return math::float3(x,y,z);
    }

    math::float3 Area::GetExtend() const
    {
        float x = (float)(m_Xmax - m_Xmin);
        float y = (float)(m_Ymax - m_Ymin);
        float z = (float)(m_Zmax - m_Zmin);
        return math::float3(x,y,z);
    }

}

