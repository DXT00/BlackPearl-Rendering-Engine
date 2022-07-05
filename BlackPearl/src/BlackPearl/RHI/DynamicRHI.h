#pragma once
#include "BlackPearl/Window.h"
namespace BlackPearl{
	void DynamicRHIInit();
	Window* RHIInitWindow();
	class DynamicModule;
	class DynamicRHI
	{
	public:
		enum class Type {
			D3D12,
			OpenGL
		};
		static Type g_Type;
		virtual Window* InitWindow() = 0;

	};

}