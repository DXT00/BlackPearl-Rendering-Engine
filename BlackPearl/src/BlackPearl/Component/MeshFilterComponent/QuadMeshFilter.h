#pragma once
#include "MeshFilter.h"
namespace BlackPearl {

	class QuadMeshFilter:public MeshFilter
	{
	public:
		QuadMeshFilter(EntityManager * entityManager, Entity::Id id)
			: MeshFilter(entityManager, id) {
			Init();
		};
		
		void Init();

		~QuadMeshFilter();
	};
}


