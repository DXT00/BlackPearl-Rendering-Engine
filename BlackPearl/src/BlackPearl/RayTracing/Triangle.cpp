#include "pch.h"
#include "Triangle.h"
#include "BlackPearl/Core.h"
namespace BlackPearl {

	Triangle::Triangle(const std::vector<Vertex>& points)
	{
		GE_ASSERT(points.size()==3,"points size error");

		for (size_t i = 0; i < 3; i++)
		{
			m_Points.push_back(points[i]);
		}
	}

	Triangle::Triangle(const Vertex& p0, const Vertex& p1, const Vertex& p2)
	{
		m_Points.push_back(p0);
		m_Points.push_back(p1);
		m_Points.push_back(p2);
	}

	Triangle::~Triangle()
	{

	}

	void Triangle::BuildBox()
	{
		glm::vec3 min = glm::min(glm::min(m_Points[0].position, m_Points[1].position), m_Points[2].position);
		glm::vec3 max = glm::max(glm::max(m_Points[0].position, m_Points[1].position), m_Points[2].position);
		m_Box = AABB(min, max);
	}

	std::vector<Vertex> Triangle::GetPoints() const
	{
		return m_Points;
	}

}
