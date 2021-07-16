#include "pch.h"
#include "BoundingBox.h"
#include "BlackPearl/Object/Object.h"
namespace BlackPearl {

	

	//Model->boundingBox--->triangleMeshes.bondingbox (BVHNode)
	AABB BoundingBox::Build()
	{
		for (const auto &mesh: m_Meshes)
		{
			const std::vector<std::shared_ptr<VertexBuffer>>& vertexBuffers = mesh.GetVertexArray()->GetVertexBuffers();
			for (const auto &buffer: vertexBuffers)
			{	
				uint32_t stride = buffer->GetBufferLayout().GetStride();
				for (size_t i = 0; i < buffer->GetVertexSize(); i++)
				{

				}
			}



		}





		return AABB();
	}


}
