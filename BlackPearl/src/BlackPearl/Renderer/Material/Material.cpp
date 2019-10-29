#include "pch.h"
#include "Material.h"


namespace BlackPearl {

	Material::~Material()
	{
	}

	void Material::AddTextureMap(std::shared_ptr<Texture> textureMap)
	{
		m_TextureMaps.push_back(textureMap);
	}

}
