#pragma once
#include "Material.h"
namespace BlackPearl {
	class MaterialManager
	{
	public:
		MaterialManager();
		~MaterialManager();

		std::shared_ptr<Material> GetMaterial(uint32_t matId) {
			if (m_MaterialDB.find(matId) == m_MaterialDB.end())
				return nullptr;
			else {
				return m_MaterialDB[matId];
			}

		}
		void AddMaterial(std::shared_ptr<Material> mat) {
			mat->SetId(m_MatMaxId);
			m_MaterialDB[m_MatMaxId] = mat;
			m_MatMaxId++;

		}

	private:
		std::unordered_map<uint32_t, std::shared_ptr<Material>> m_MaterialDB;
		uint32_t m_MatMaxId = 0;
	};
}


