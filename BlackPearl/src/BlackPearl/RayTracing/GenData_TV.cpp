#include "pch.h"
#include "GenData_TV.h"

namespace BlackPearl {
	// TexT : Texture Type
	const float TexT_ConstTexture = 0.0f;
	const float TexT_ImgTexture = 1.0f;
	const float TexT_Skybox = 2.0f;

	GenData_TV::GenData_TV()
	{
		
	}
	void GenData_TV::ParseTextureData(std::map<std::shared_ptr<Texture>, std::vector<size_t>> tex2vec)
	{
		for (auto pair:tex2vec)
		{
			std::shared_ptr<Texture> texture = pair.first;
		}
	}

	void GenData_TV::ParseColorData(std::map<std::shared_ptr<glm::vec3>, std::vector<size_t>> color2vec, std::vector<float>& packData)
	{
		for (auto pair : color2vec)
		{
			auto color = pair.first;
			ParseColor(color, packData);
		}
	}

	void GenData_TV::ParseColor(std::shared_ptr <glm::vec3>color, std::vector<float>& packData)
	{
		auto targetPair = m_Color2Idx.find(color);
		if (targetPair != m_Color2Idx.end())
			return;

		m_Color2Idx[color] = m_TexData.size();

		m_TexData.push_back(TexT_ConstTexture);

		m_TexData.push_back(packData.size() / 4);
	
		packData.push_back(color->r);
		packData.push_back(color->g);
		packData.push_back(color->b);
		packData.push_back(0);
	}

}