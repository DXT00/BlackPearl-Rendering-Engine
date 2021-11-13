#pragma once
#include "Ray.h"
#include "glm/glm.hpp"
#include "BlackPearl/Renderer/Material/Material.h"

namespace BlackPearl {

	struct Vertex {
		bool isHit;
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uv;

	};

	struct HitRecord {
		HitRecord(std::shared_ptr<Ray> ray = NULL, const glm::vec3& pos = glm::vec3(0),
			const glm::vec3& normal = glm::vec3(0, 0, 1), float u = 0, float v = 0);

		std::shared_ptr<Ray> ray;
		Vertex vertex;
	};


	struct HitRst {
		HitRst(bool hit = false) : hit(hit), isMatCoverable(true), material(NULL) { }

		bool hit;
		HitRecord record;
		std::shared_ptr<Material> material;
		bool isMatCoverable;
		//------------
		static const HitRst InValid;
	};
}