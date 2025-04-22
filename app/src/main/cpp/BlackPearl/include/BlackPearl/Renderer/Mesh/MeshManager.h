#pragma once
#include "BlackPearl/Object/Object.h"
namespace BlackPearl {
	class MeshManager
	{
	public:
		void AddObjs(Object* obj);

	
	private:
		float* m_VertexBuffer;
		float* m_NormalBuffer;
		float* m_TexCordBuffer;
		float* m_TangentBuffer;
		float* m_BitangentBuffer;

	};

}

