#pragma once
#include "BlackPearl/RHI/DynamicRHI.h"
#include "BlackPearl/Window.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLLogger.h"
namespace BlackPearl {
	class OpenGLDynamicRHI : public DynamicRHI
	{
	public:
		Window* InitWindow() override;
		virtual void InitRHI() override;

		void EngineExit() override;
		//void InitLogger() override;
		//std::shared_ptr<Logger> GetCoreLogger() override  { return m_OpenGLLogger;};
	private:
		//std::shared_ptr<OpenGLLogger> m_OpenGLLogger;
	};
}


