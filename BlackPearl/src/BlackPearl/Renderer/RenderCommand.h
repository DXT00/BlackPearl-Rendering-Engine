#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "BlackPearl/Math//vector.h"

namespace BlackPearl {

	class RenderCommand
	{
	public:
		
		inline static void SetClearColor(const math::float4 &color) {
			glClearColor(color.x,color.y,color.z,color.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef GE_ENABLE_STENCIL_TEST
			glClear(GL_STENCIL_TEST);
#endif // GE_ENABLE_STENCIL_TEST

		}



	};


}

