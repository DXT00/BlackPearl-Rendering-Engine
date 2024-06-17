#pragma once
#include "MeshFilter.h"
namespace BlackPearl {
	class SkyBoxMeshFilter :public MeshFilter
	{
	public:
		SkyBoxMeshFilter()
			:MeshFilter() {
			Init();
		}
		void Init();
		~SkyBoxMeshFilter();
	};
}


