#pragma once
#include "MeshFilter.h"
namespace BlackPearl {
	class SkyBoxMeshFilter :public MeshFilter
	{
	public:
		SkyBoxMeshFilter(EntityManager * entityManager, Entity::Id id)
			:MeshFilter(entityManager, id) {
			Init();
		}
		void Init();
		~SkyBoxMeshFilter();
	};
}


