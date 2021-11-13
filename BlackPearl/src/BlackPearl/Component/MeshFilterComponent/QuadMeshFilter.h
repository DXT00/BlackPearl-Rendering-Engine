#pragma once
#include "MeshFilter.h"
namespace BlackPearl {

	class QuadMeshFilter:public MeshFilter
	{
	public:
		QuadMeshFilter()
			: MeshFilter() {
			Init();
		};
		
		void Init();

		~QuadMeshFilter();
	};
}


