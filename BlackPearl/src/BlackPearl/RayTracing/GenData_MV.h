#pragma once
#include "BlackPearl/Renderer/Material/Material.h"
namespace BlackPearl {
	class GenData_MV
	{
	public:
		void ParseMatData(std::map<std::shared_ptr<Material>, std::vector<unsigned int>> mat2vec);

		void ParseDiffuseMat(const std::shared_ptr<Material>& material);
		void ParseSpecularMat(const std::shared_ptr<Material>& material);
		void ParseMetalMat(const std::shared_ptr<Material>& material);
		void ParseDielectricMat(const std::shared_ptr<Material>& material);
		void ParseEmissiomMat(const std::shared_ptr<Material>& material);
		std::vector<float> GetMatData() const { return m_MatData; }
		std::map<std::shared_ptr<Material>, size_t> GetMat2IdxMap() const { return m_Mat2Idx; }
		std::map<std::shared_ptr<Texture>, std::vector<size_t>> GetTex2Vec() const{ return m_Tex2Vec; }
		std::map< std::shared_ptr<glm::vec3>, std::vector<size_t>> GetColor2Vec() const { return m_Color2Vec; }

		//暂时只只支持color 
		void SetTex(const std::map<std::shared_ptr<glm::vec3>,size_t> &color2Idx);
	private:
		std::vector<float> m_MatData;
		std::map<std::shared_ptr<Material>, size_t> m_Mat2Idx;
		std::map<std::shared_ptr<Texture>, std::vector<size_t>> m_Tex2Vec;
		std::map< std::shared_ptr<glm::vec3>, std::vector<size_t>> m_Color2Vec;
	};


}

