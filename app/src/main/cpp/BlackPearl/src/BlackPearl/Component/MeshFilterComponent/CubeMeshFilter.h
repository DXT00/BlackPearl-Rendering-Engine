#pragma once
#include "MeshFilter.h"
namespace BlackPearl {
	
	class CubeMeshFilter:public MeshFilter
	{
	public:
		CubeMeshFilter()
			:MeshFilter() {
			Init();
		}
		void Init();

		~CubeMeshFilter();
	};

}