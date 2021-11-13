#pragma once
#include <map>
#include "BlackPearl/Renderer/Material/Texture.h"
#include "BlackPearl/Renderer/Material/MaterialColor.h"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
namespace BlackPearl {
	class GenData_TV
	{
	public:
		GenData_TV();
		void ParseTextureData(std::map<std::shared_ptr<Texture>,std::vector<size_t>> tex2vec);
		void ParseColorData(std::map<std::shared_ptr<glm::vec3>, std::vector<size_t>> color2vec,std::vector<float>& packData);
		void ParseColor(std::shared_ptr <glm::vec3> color, std::vector<float>& packData);
		void ParseTex(std::shared_ptr<Texture> texture, std::vector<float>& packData);
		void ParseCubeMap(std::shared_ptr<Texture> texture);
		void ParseImgMap(std::shared_ptr<Texture> texture);

		std::vector<float> GetTextureData() const { return m_TexData; }
		std::map<std::shared_ptr<glm::vec3>, size_t> GetColor2IdxMap() const { return m_Color2Idx; }
		std::map<std::shared_ptr<Texture>, size_t> GetTex2IdxMap() const { return m_Tex2Idx; }

		std::map<std::shared_ptr<CubeMapTexture>, size_t> GetCubeMap2RenderIdMap() { return m_CubeMap2RenderId; }
		std::map<std::shared_ptr<Texture>, size_t> GetImg2RenderIdMap() { return m_Img2RenderId; }
	private:
		std::vector<float> m_TexData;
		std::map<std::shared_ptr<glm::vec3>, size_t> m_Color2Idx;
		std::map<std::shared_ptr<Texture>, size_t> m_Tex2Idx;
		//记录 texture的渲染id
		std::map<std::shared_ptr<Texture>, size_t> m_Img2RenderId;
		//记录 CubeMap的渲染id
		std::map<std::shared_ptr<CubeMapTexture>, size_t> m_CubeMap2RenderId;


	};


}

