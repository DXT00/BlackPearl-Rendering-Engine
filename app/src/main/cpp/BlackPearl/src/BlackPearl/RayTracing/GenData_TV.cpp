#include "pch.h"
#include "GenData_TV.h"

namespace BlackPearl {


	GenData_TV::GenData_TV()
	{
		
	}
	void GenData_TV::ParseTextureData(std::map<std::shared_ptr<Texture>, std::vector<size_t>> tex2vec)
	{
		for (auto pair:tex2vec)
		{
			std::shared_ptr<Texture> texture = pair.first;

			if (texture->GetType() == Texture::Type::CubeMap) {
				ParseCubeMap(texture);
			}
			else {
				ParseImgMap(texture);
			}
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

		m_TexData.push_back(Texture::Type::None);

		m_TexData.push_back(packData.size() / 4);
	
		packData.push_back(color->r);
		packData.push_back(color->g);
		packData.push_back(color->b);
		packData.push_back(0);
	}



	void GenData_TV::ParseCubeMap(std::shared_ptr<Texture> texture)
	{
		auto targetPair = m_Tex2Idx.find(texture);
		if (targetPair != m_Tex2Idx.end())
			return;

		m_Tex2Idx[texture] = m_TexData.size();

		m_TexData.push_back(texture->GetType());

		std::shared_ptr<CubeMapTexture> cube_texture = std::static_pointer_cast<CubeMapTexture>(texture);
		if (m_CubeMap2RenderId.find(cube_texture) == m_CubeMap2RenderId.end()) {
			m_CubeMap2RenderId[cube_texture] = m_CubeMap2RenderId.size();
		}
		m_TexData.push_back(m_CubeMap2RenderId[cube_texture]);
	}

	void GenData_TV::ParseImgMap(std::shared_ptr<Texture> texture)
	{
		auto targetPair = m_Tex2Idx.find(texture);
		if (targetPair != m_Tex2Idx.end())
			return;

		m_Tex2Idx[texture] = m_TexData.size();
		m_TexData.push_back(texture->GetType());


		if (m_Img2RenderId.find(texture) == m_Img2RenderId.end()) {
			m_Img2RenderId[texture] = m_Img2RenderId.size();
		}

		m_TexData.push_back(m_Img2RenderId[texture]);


	}

}