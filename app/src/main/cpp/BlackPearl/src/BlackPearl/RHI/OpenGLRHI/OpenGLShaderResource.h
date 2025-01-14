#pragma once
namespace BlackPearl{
	enum
	{
		OGL_MAX_UNIFORM_BUFFER_BINDINGS = 12,	// @todo-mobile: Remove me
		OGL_FIRST_UNIFORM_BUFFER = 0,			// @todo-mobile: Remove me
		OGL_UAV_NOT_SUPPORTED_FOR_GRAPHICS_UNIT = -1, // for now, only CS and PS supports UAVs/ images
	};


}
