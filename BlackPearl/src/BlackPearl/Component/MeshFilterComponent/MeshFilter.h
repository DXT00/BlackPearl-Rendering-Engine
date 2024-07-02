#pragma once
#include "BlackPearl/Math/Math.h"
#include "BlackPearl/Component/Component.h"

namespace BlackPearl {

	class MeshFilter :public Component<MeshFilter>
	{
	public:
		MeshFilter()
			:Component(Component::Type::MeshFilter) {}
		~MeshFilter();
		std::vector<float>  GetVertices() { return m_Vertices; }
		std::vector<unsigned int> GetIndices() { return m_Indices;}
		void SetVertices(std::vector<float> vertices) { m_Vertices = vertices; }
		void SetIndices(std::vector<unsigned int> indices) { m_Indices = indices; }

		virtual std::vector<math::float3>  GetPosition() const;
		
	protected:
		std::vector<float>        m_Vertices;
		std::vector<unsigned int> m_Indices;

	
	};

}

