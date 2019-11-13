#pragma once
#include "Texture.h"
#include "BlackPearl/Renderer/Shader.h"
#include "MaterialColor.h"
namespace BlackPearl {
	class Material
	{
	public:
		struct TextureMaps {
			std::shared_ptr<Texture> diffuseTextureMap;
			std::shared_ptr<Texture> specularTextureMap;
			std::shared_ptr<Texture> emissionTextureMap;
			std::shared_ptr<Texture> normalTextureMap;
			std::shared_ptr<Texture> heightTextureMap;
			TextureMaps() {
				diffuseTextureMap
				= specularTextureMap
				= emissionTextureMap
				= normalTextureMap
				= heightTextureMap
				= nullptr;
			}
		};

		Material(
			const std::shared_ptr<Shader>&shader,
			const std::shared_ptr<TextureMaps> &textureMaps,
			const MaterialColor &materialColors
		)
			:m_Shader(shader), m_TextureMaps(textureMaps), m_MaterialColors(materialColors) {}

		Material(
			const std::string shaderPath,
			const std::shared_ptr<TextureMaps> &textureMaps,
			glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 emissionColor
		);

		~Material();
		
		std::shared_ptr<Shader> GetShader()const { return m_Shader; }
		std::shared_ptr<TextureMaps> GetTextureMaps() { return m_TextureMaps; }
		MaterialColor GetMaterialColor()const { return m_MaterialColors; }


		void SetMaterialColor(MaterialColor::Color color) { m_MaterialColors.SetColor(color); }
		void SetTexture(const std::shared_ptr<Texture> texture);
		
		std::shared_ptr<Shader> m_Shader;
		/*std::shared_ptr<Texture> m_DiffuseTextureMap;
		std::shared_ptr<Texture> m_SpecularTextureMap;
		std::shared_ptr<Texture> m_EmissionTextureMap;
		std::shared_ptr<Texture> m_NormalTextureMap;
		std::shared_ptr<Texture> m_HeightTextureMap;*/
		std::shared_ptr<TextureMaps> m_TextureMaps;
		//std::vector<std::shared_ptr<Texture>> m_TextureMaps;
		MaterialColor m_MaterialColors;


	};

}