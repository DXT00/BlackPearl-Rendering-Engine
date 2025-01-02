#pragma once
//#include "BlackPearl/RHI/Common/stb_util.h"
namespace BlackPearl {
	class HDRTexture
	{
	public:
		HDRTexture(const std::string &image);
		~HDRTexture();

		void LoadHdrTexture(const std::string& image);
		void Bind();
		void UnBind();
	private:
		unsigned int m_RendererID;
	};


}

