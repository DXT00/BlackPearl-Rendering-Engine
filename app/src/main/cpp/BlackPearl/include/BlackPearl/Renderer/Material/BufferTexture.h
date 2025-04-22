#pragma once
namespace BlackPearl {
	/*
	A Buffer Texture is a one-dimensional Texture whose 
	storage comes from a Buffer Object. They are used 
	to allow a shader to access a large table of memory
	that is managed by a buffer object.
	
	Buffer textures are defined by the fact that the storage 
	for the texels comes directly from a buffer object.
	
	Instead of creating storage for a buffer texture, the 
	user associates a buffer texture with an already existing
	buffer object.

	wiki:https://www.khronos.org/opengl/wiki/Buffer_Texture
	*/
	class BufferTexture
	{
	public:
		BufferTexture(unsigned int bufferSize,int internalFormat,const void* data);
		~BufferTexture();
		void Bind();
		void Unbind();
		void BindTexture();
		void UnbindTexture();
		unsigned int GetRendererID()const { return m_RendererID; }
		unsigned int GetTextureID()const { return m_TextureID; }
	private:
		unsigned int m_RendererID;
		unsigned int m_TextureID;

	};

}


