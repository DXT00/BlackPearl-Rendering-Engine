#pragma once
#include <vector>
#include "OpenGLTexture.h"

namespace BlackPearl {
	class Texture3D : public Texture
	{
	public:

        Texture3D(
            const TextureDesc& desc
        );


		//Texture3D(
		//	const std::vector<float>& textureBuffer,
		//	const int width,const int height,const int depth,
		//	const bool generateMipmaps = true);
		/*Texture3D(
			const std::vector<unsigned int>& textureBuffer,
			const int width, const int height, const int depth,
			const bool generateMipmaps = true);*/
		~Texture3D();

		virtual void Init(
			const TextureDesc& desc,
			float* data = nullptr) override;

		//Clears this texture using a given clear color. //TODO::不需要Unbind?
		void Clear(float clearColor[4]);
	//	void Clear(GLuint clearColor[4]);
		void Bind();
		void UnBind();
		//unsigned int GetRendererID() { return m_TextureID; }

	private:
		//unsigned int m_TextureID;
		//int m_Width, m_Height, m_Depth;
		//int m_MipLevel = 5;
		//std::vector<float> m_ClearData;//TODO::
	};

}

