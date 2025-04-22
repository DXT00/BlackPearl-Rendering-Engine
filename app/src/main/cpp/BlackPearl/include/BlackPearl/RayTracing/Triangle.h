#pragma once
#include "Hitable.h"
#include "glm/glm.hpp"
#include "BlackPearl/Renderer/Mesh/Vertex.h"
#include "BlackPearl/Component/Component.h"
namespace BlackPearl {
	class Triangle : public Component<Triangle>
	{
	public:
		Triangle(const std::vector<Vertex>& points);
		Triangle(const Vertex& p0, const Vertex& p1, const Vertex& p2);

		~Triangle();
		std::vector<Vertex> GetPoints() const;
		AABB GetBoundingBox() const { return m_Box; }
		// ÔÚmeshÖÐµÄid
		int Id = -1;

	private:
		void BuildBox();
		std::vector<Vertex> m_Points;
		AABB m_Box;
	};


}

