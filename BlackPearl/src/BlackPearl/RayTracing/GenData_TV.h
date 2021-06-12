#pragma once
#include <map>
#include "BlackPearl/Renderer/Material/Texture.h"
#include "BlackPearl/Renderer/Material/MaterialColor.h"
namespace BlackPearl {
	class GenData_TV
	{
	public:
		GenData_TV();
		void ParseTextureData(std::map<std::shared_ptr<Texture>,std::vector<size_t>> tex2vec);
		void ParseColorData(std::map<std::shared_ptr<glm::vec3>, std::vector<size_t>> color2vec,std::vector<float>& packData);
		void ParseColor(std::shared_ptr <glm::vec3> color, std::vector<float>& packData);
		std::vector<float> GetTextureData() const { return m_TexData; }
		std::map<std::shared_ptr<glm::vec3>, size_t> GetColor2IdxMap() const { return m_Color2Idx; }
	private:
		std::vector<float> m_TexData;
		std::map<std::shared_ptr<glm::vec3>, size_t> m_Color2Idx;

	};


}

