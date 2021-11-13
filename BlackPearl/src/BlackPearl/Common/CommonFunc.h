#pragma once
#include <memory>
#include <vector>
#include "BlackPearl/Renderer/Material/Texture.h"
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Object/Object.h"
#include "glm/glm.hpp"
namespace BlackPearl {
	class CommonFunc
	{
	public:
		static void ShowGBuffer(unsigned int row, unsigned int col, Object* quad, std::shared_ptr<GBuffer> gBuffer, std::vector<std::shared_ptr<Texture> >textures);

	};

}

