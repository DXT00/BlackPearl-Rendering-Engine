#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"
namespace BlackPearl {

	class RenderCommand
	{
	public:
		
		inline static void SetClearColor(const glm::vec4 &color) {
			glClearColor(color.r,color.g,color.b,color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef GE_ENABLE_STENCIL_TEST
			glClear(GL_STENCIL_TEST);
#endif // GE_ENABLE_STENCIL_TEST

		}



	};


}

