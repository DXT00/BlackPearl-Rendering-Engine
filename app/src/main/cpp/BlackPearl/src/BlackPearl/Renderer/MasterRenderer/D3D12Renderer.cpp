#include "pch.h"
#ifdef GE_API_D3D12
#include "D3D12Renderer.h"
namespace BlackPearl {

	D3D12Renderer::D3D12Renderer(int clientWidth, int clientHeight)
	{
		UpdateForSizeChange(clientWidth, clientHeight);
	}

	D3D12Renderer::~D3D12Renderer()
	{
	}

	void D3D12Renderer::UpdateForSizeChange(int clientWidth, int clientHeight)
	{
		m_Width = clientWidth;
		m_Height = clientHeight;
		m_AspectRatio = static_cast<float>(clientWidth) / static_cast<float>(clientHeight);
	}

}
#endif