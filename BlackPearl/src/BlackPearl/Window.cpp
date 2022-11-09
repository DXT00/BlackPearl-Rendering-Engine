#include "pch.h"
#include "Window.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Config.h"

namespace BlackPearl {

	Window::Window(const WindowData & data)
	{
		m_Data.Height = data.Height;
		m_Data.Width = data.Width;
		m_Data.Title = data.Title;

		GE_CORE_INFO("Creating window {0} ({1} ,{2})", data.Title, data.Width, data.Height);
		

	}

	Window::~Window()
	{
	}

}