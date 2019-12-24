#pragma once
#include "Texture.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "MaterialColor.h"
namespace BlackPearl {
	class Material
	{
	public:
		struct Props {
			float shininess;
			bool  isBinnLight;
			int  isTextureSample;//是否使用纹理
			Props():shininess(64.0f),isBinnLight(false),isTextureSample(1){}

		};
		struct TextureMaps {
			std::shared_ptr<Texture> diffuseTextureMap;
			std::shared_ptr<Texture> specularTextureMap;
			std::shared_ptr<Texture> emissionTextureMap;
			std::shared_ptr<Texture> normalTextureMap;
			std::shared_ptr<Texture> heightTextureMap;
			std::shared_ptr<Texture> cubeTextureMap;
			std::shared_ptr<Texture> depthTextureMap;
			std::shared_ptr<Texture> aoMap;
			std::shared_ptr<Texture> roughnessMap;
			std::shared_ptr<Texture> mentallicMap;

			TextureMaps() {
				diffuseTextureMap
				= specularTextureMap
				= emissionTextureMap
				= normalTextureMap
				= heightTextureMap
				= cubeTextureMap
				= depthTextureMap
				= aoMap
				= roughnessMap
				= mentallicMap
				= nullptr;
			}
		};

		Material(
			const std::shared_ptr<Shader>&shader,
			const std::shared_ptr<TextureMaps> &textureMaps,
			const MaterialColor &materialColors
		)
			:m_Shader(shader), m_TextureMaps(textureMaps), m_MaterialColors(materialColors), m_Props(Props()){}

		Material(
			const std::string shaderPath,
			const std::shared_ptr<TextureMaps> &textureMaps,
			glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 emissionColor
		);

		~Material();
		
		std::shared_ptr<Shader>      GetShader()const        { return m_Shader; }
		std::shared_ptr<TextureMaps> GetTextureMaps()const   { return m_TextureMaps; }
		MaterialColor                GetMaterialColor()const { return m_MaterialColors; }
		Props                        GetProps() const { return m_Props; }

		void SetShader(const std::string& shaderPath);
		void SetShader(const std::shared_ptr<Shader> &shader);
		void SetTexture(const std::shared_ptr<Texture> texture);
		void SetTexture(const Texture::Type type, const std::string& image);
		void SetMaterialColor(MaterialColor::Color color);

		void SetProps(const Props& props);
		void SetShininess(float shininess);
		void SetBinnLight(bool isBinnLight);
		void SetTextureSample(int isTextureSample);

	private:
		std::shared_ptr<Shader>		 m_Shader;
		std::shared_ptr<TextureMaps> m_TextureMaps;
		MaterialColor				 m_MaterialColors;
		Props                        m_Props;
	};

}