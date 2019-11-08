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
			const MaterialColor &materialColors
		)
			:m_Shader(shader), m_TextureMaps(textureMaps), m_MaterialColors(materialColors) {}

		Material(
			const std::string shaderPath,
			const std::vector<std::shared_ptr<Texture>> &textureMaps,
			glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 emissionColor
		);

		~Material();
		void AddTextureMap(std::shared_ptr<Texture> textureMap);
		std::shared_ptr<Shader> GetShader()const { return m_Shader; }
		std::vector<std::shared_ptr<Texture>> GetTextureMaps() { return m_TextureMaps; }
		MaterialColor GetMaterialColor()const { return m_MaterialColors; }


		void SetMaterialColor(MaterialColor::Color color) { m_MaterialColors.SetColor(color); }
	private:
		std::shared_ptr<Shader> m_Shader;
		std::vector<std::shared_ptr<Texture>> m_TextureMaps;
		MaterialColor m_MaterialColors;


	};

}