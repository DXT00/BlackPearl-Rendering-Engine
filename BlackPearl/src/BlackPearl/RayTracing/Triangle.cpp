#include "pch.h"
#include "Triangle.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Math/Math.h"
namespace BlackPearl {

	Triangle::Triangle(const std::vector<Vertex>& points)
		:Component(Component::Type::Triangle)
	{
		GE_ASSERT(points.size()==3,"points size error");

		for (size_t i = 0; i < 3; i++)
		{
			m_Points.push_back(points[i]);
		}
		BuildBox();
	}

	Triangle::Triangle(const Vertex& p0, const Vertex& p1, const Vertex& p2)
		:Component(Component::Type::Triangle)
	{
		m_Points.push_back(p0);
		m_Points.push_back(p1);
		m_Points.push_back(p2);
		BuildBox();
	}

	Triangle::~Triangle()
	{

	}

	void Triangle::BuildBox()
	{
		math::float3 min = Math::Min(Math::Min(m_Points[0].position, m_Points[1].position), m_Points[2].position);
		math::float3 max = Math::Max(Math::Max(m_Points[0].position, m_Points[1].position), m_Points[2].position);
		m_Box = AABB(min, max, true);
	}

	std::vector<Vertex> Triangle::GetPoints() const
	{
		return m_Points;
	}

}
