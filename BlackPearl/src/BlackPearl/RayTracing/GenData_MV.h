#pragma once
#include "BlackPearl/Renderer/Material/Material.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLTexture.h"
#include "BlackPearl/Math/vector.h"
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
		std::map<TextureHandle, std::vector<size_t>> GetTex2Vec() const{ return m_Tex2Vec; }
		std::map< std::shared_ptr<math::float3>, std::vector<size_t>> GetColor2Vec() const { return m_Color2Vec; }
		void SetTex(const std::map<std::shared_ptr<math::float3>,size_t> &color2Idx);
		void SetTex(const std::map<TextureHandle, size_t>& tex2Idx);

	private:
		std::vector<float> m_MatData;
		std::map<std::shared_ptr<Material>, size_t> m_Mat2Idx;
		std::map<TextureHandle, std::vector<size_t>> m_Tex2Vec;
		std::map< std::shared_ptr<math::float3>, std::vector<size_t>> m_Color2Vec;
	};


}

