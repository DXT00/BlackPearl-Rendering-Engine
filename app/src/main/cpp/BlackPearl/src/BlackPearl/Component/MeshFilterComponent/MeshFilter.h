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
		std::vector<float>  GetVertices() const{ return m_Vertices; }
		std::vector<uint32_t> GetIndices() const{ return m_Indices;}
		void SetVertices(std::vector<float> vertices) { m_Vertices = vertices; }
		void SetIndices(std::vector<uint32_t> indices) { m_Indices = indices; }

		std::vector<uint32_t> indexData;

		std::vector<math::float3> positionData;
		std::vector<math::float3> prePositionData;

		std::vector<math::float2> texcoord1Data;
		std::vector<math::float2> texcoord2Data;
		std::vector<math::float3> normalData;
		std::vector<math::float3> tangentData;
		std::vector<math::float3> bitangentData;
		//virtual std::vector<math::float3>  GetPosition() const;
		
	protected:
		std::vector<float>        m_Vertices;
		std::vector<uint32_t> m_Indices;

	
	};

}

