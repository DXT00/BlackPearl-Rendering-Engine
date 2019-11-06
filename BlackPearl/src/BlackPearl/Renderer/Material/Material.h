#pragma once
#include "Texture.h"
#include "BlackPearl/Renderer/Shader.h"
#include "MaterialColor.h"
namespace BlackPearl {
	class Material
	{
	public:

		Material(
			const std::shared_ptr<Shader>&shader,
			const std::vector<std::shared_ptr<Texture>> &textureMaps,
			const std::vector<std::shared_ptr<MaterialColor>> &materialColors
		)
			:m_Shader(shader), m_TextureMaps(textureMaps), m_MaterialColors(materialColors) {}


		~Material();
		void AddTextureMap(std::shared_ptr<Texture> textureMap);
		std::vector<std::shared_ptr<Texture>> GetTextureMaps() { return m_TextureMaps; }
		std::shared_ptr<Shader> GetShader()const { return m_Shader; }
		std::vector<std::shared_ptr<MaterialColor>> GetMaterialColor()const { return m_MaterialColors; }
	private:
		std::shared_ptr<Shader> m_Shader;
		std::vector<std::shared_ptr<Texture>> m_TextureMaps;
		std::vector<std::shared_ptr<MaterialColor>> m_MaterialColors;


	};

}