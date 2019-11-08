#pragma once
#include "MeshFilter.h"
namespace BlackPearl {
	
	class CubeMeshFilter:public MeshFilter
	{
	public:
		CubeMeshFilter(EntityManager * entityManager, Entity::Id id)
			:MeshFilter(entityManager, id) {
			Init();
		};
		void Init();

		~CubeMeshFilter();
	};

}