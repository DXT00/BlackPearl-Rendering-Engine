#pragma once
#ifdef GE_API_D3D12
#include "../DynamicRHI.h"
#include "BlackPearl/Window.h"
#include "BlackPearl/Log.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12Logger.h"

namespace BlackPearl {
	class D3D12DynamicRHI :public DynamicRHI
	{
	public:
		D3D12DynamicRHI() {

		}

		virtual Window* InitWindow() override;
		void InitLogger();
		void InitRHI() override;
		std::shared_ptr<D3D12Logger> GetCoreLogger() const { return m_D3D12Logger; };

	private:
		std::shared_ptr<D3D12Logger> m_D3D12Logger;
	};
}

#endif
