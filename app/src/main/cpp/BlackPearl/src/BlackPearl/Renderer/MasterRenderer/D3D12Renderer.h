#pragma once
#ifdef GE_API_D3D12
namespace BlackPearl {

	class D3D12Renderer
	{
	public:


	public:
		D3D12Renderer(int clientWidth,int clientHeight);
		~D3D12Renderer();

	protected:
		virtual void UpdateForSizeChange(int clientWidth, int clientHeight);

		int m_Width;
		int m_Height;
		float m_AspectRatio;

	};

}
#endif