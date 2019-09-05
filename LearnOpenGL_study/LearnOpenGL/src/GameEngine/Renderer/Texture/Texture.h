#pragma once

class Texture
{
public:
	Texture(const char *image);
	~Texture() {};

	void Bind();
	void UnBind();
private:
	unsigned int m_Texture;
};



