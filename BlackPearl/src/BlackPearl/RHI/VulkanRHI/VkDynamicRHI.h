#pragma once
#include "glm/glm.hpp"
#include "vulkan/vulkan.h"
#include "../DynamicRHI.h"
namespace BlackPearl {

	class VkDynamicRHI : public DynamicRHI
	{
	public:


		VkDynamicRHI() {

		}
		virtual Window* InitWindow() override;

		virtual void InitRHI() override;

	

	};

}

