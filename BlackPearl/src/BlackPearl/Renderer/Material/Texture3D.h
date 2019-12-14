#pragma once
#include <vector>


namespace BlackPearl {
	class Texture3D
	{
	public:
		Texture3D(
			const std::vector<float>& textureBuffer,
			const int width,const int height,const int depth,
			const bool generateMipmaps = true);

		~Texture3D();

		//Clears this texture using a given clear color. //TODO::²»ÐèÒªUnbind?
		void Clear(float clearColor[4]);

		void Bind();
		void UnBind();
		unsigned int GetRendererID() { return m_TextureID; }

	private:
		unsigned int m_TextureID;
		int m_Width, m_Height, m_Depth;
		std::vector<float> m_ClearData;//TODO::
	};

}

