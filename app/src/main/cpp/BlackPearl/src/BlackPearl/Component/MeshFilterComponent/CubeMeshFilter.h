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

		//virtual std::vector<math::float3>  GetPosition() const override { return mPos; }

	private:
		std::vector<math::float3> mPos;
	};

}