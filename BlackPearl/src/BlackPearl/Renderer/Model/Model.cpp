#include "pch.h"
#include "Model.h"
#include "BlackPearl/Renderer/Material/Texture.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/fwd.hpp"
#include "glm/gtx/quaternion.hpp"
#include "BlackPearl/Renderer/Mesh/MeshletConfig.h"
namespace BlackPearl {


	void Model::UpdateAABB(const glm::vec3& pos)
	{
		glm::vec3 min;
		glm::vec3 max;
		if (m_FirstVertex) {
			m_FirstVertex = false;
			min = pos;
			max = pos;
			return;
		}
		min = m_AABB->GetMinP();
		max = m_AABB->GetMaxP();

		if (pos.x < min.x) {
			min.x = pos.x;
		}
		if (pos.y < min.y) {
			min.y = pos.y;
		}
		if (pos.z < min.z) {
			min.z = pos.z;
		}
		if (pos.x > max.x) {
			max.x = pos.x;
		}
		if (pos.y > max.y) {
			max.y = pos.y;
		}
		if (pos.z > max.z) {
			max.z = pos.z;
		}

		m_AABB->SetP(min,max);
	}

}