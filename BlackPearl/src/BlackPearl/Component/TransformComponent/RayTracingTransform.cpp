#include "pch.h"
#include "glm/glm.hpp"
#include "RayTracingTransform.h"
#include "BlackPearl/Math/Math.h"
#include "BlackPearl/Component/BVHNodeComponent/BVHNode.h"
namespace BlackPearl {
	void RTXTransformNode::BuildBox()
	{

		AABB child_box = m_BVHObj->GetComponent<BlackPearl::BVHNode>()->GetRootBox();
		if (!child_box.IsValid()) {
			m_Box = AABB::InValid;
			return;
		}
		glm::vec3 srcP[2] = { child_box.GetMinP(), child_box.GetMaxP() };
		std::vector<glm::vec3> tfmCornerPs;
		for (size_t i = 0; i < 8; i++) {
			std::bitset<3> binVal(i); //000,001,010
			glm::vec3 cornerP = glm::vec3(srcP[binVal[2]].x, srcP[binVal[1]].y, srcP[binVal[0]].z);
			glm::vec4 tfmCornerPQ = m_TransformMatrix * glm::vec4(cornerP, 1);
			tfmCornerPs.push_back(glm::vec3(tfmCornerPQ) / tfmCornerPQ.w);
		}

		glm::vec3 minP = Math::Min(tfmCornerPs);
		glm::vec3 maxP = Math::Max(tfmCornerPs);
		m_Box = AABB(minP, maxP);

	}
}
