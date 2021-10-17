#include "pch.h"
#include "GenData_MV.h"


namespace BlackPearl {

	void GenData_MV::ParseMatData(std::map<std::shared_ptr<Material>, std::vector<unsigned int>> mat2vec)
	{

		for (auto const& pair : mat2vec) {
			Material::RTXType mat_type = pair.first->GetRTXType();
			if (mat_type == Material::RTXType::RTX_DIFFUSE)
				ParseDiffuseMat(pair.first);
			else if (mat_type == Material::RTXType::RTX_SPECULAR || mat_type == Material::RTXType::RTX_METALLIC)
				ParseSpecularMat(pair.first);
			else if (mat_type == Material::RTXType::RTX_DIELECTRIC) {
				ParseDielectricMat(pair.first);
			}

		}
	}

	void GenData_MV::ParseDiffuseMat(const std::shared_ptr<Material> &material)
	{
		/*if (m_Mat2Idx.find(material) != m_Mat2Idx.end()) {
			return;
		}
		m_Mat2Idx[material] = m_MatData.size();
		m_MatData.push_back(material->GetType());*/

	}

	void GenData_MV::ParseSpecularMat(const std::shared_ptr<Material>& material)
	{
		if (material == NULL)
			return;
		if (m_Mat2Idx.find(material) != m_Mat2Idx.end()) {
			return;
		}
		m_Mat2Idx[material] = m_MatData.size();
		m_MatData.push_back(float(material->GetRTXType()));


		if (material->GetProps().isSpecularTextureSample && material->GetTextureMaps()->specularTextureMap != NULL) {
			m_Tex2Vec[material->GetTextureMaps()->specularTextureMap].push_back(m_MatData.size());
		}
		else {
			std::shared_ptr<glm::vec3> color;
			color.reset(DBG_NEW glm::vec3(material->GetMaterialColor().Get().specularColor));
			m_Color2Vec[color].push_back(m_MatData.size());

		}
		m_MatData.push_back(-1);//´æ´¢texture idx»òcolor idx
		m_MatData.push_back(material->GetProps().shininess);
	}

	void GenData_MV::ParseMetalMat(const std::shared_ptr<Material>& material)
	{


	}

	void GenData_MV::ParseDielectricMat(const std::shared_ptr<Material>& material)
	{
		if (material == NULL)
			return;
		if (m_Mat2Idx.find(material) != m_Mat2Idx.end()) {
			return;
		}
		m_Mat2Idx[material] = m_MatData.size();
		m_MatData.push_back(float(material->GetRTXType()));




		//m_MatData.push_back(-1);//´æ´¢texture idx»òcolor idx
		m_MatData.push_back(material->GetProps().refractIndex);
	}

	void GenData_MV::ParseEmissiomMat(const std::shared_ptr<Material>& material)
	{
	}

	void GenData_MV::SetTex(const std::map<std::shared_ptr<glm::vec3>, size_t>& color2Idx)
	{

		for (auto& pair : color2Idx) {
			auto target = m_Color2Vec.find(pair.first);
			if (target != m_Color2Vec.end()) {
				for (auto const& idx : target->second)
					m_MatData[idx] = pair.second;
			}
		}

		/*for (auto colorVec :m_Color2Vec)
		{
			size_t texDataIdx;
			if (color2Idx.find(colorVec.first) != color2Idx.end()) {

				texDataIdx = color2Idx[colorVec.first];
			}
			else {
				continue;
			}
			for (auto idx : colorVec.second) {
				m_MatData[idx] = texDataIdx;
			}
		}*/
		
	}


}
