#pragma once
#include "BlackPearl/Renderer/Material/Material.h"

namespace BlackPearl {

	class GenData_HV
	{
	public:
		GenData_HV();
		void ParseMatData(Object* obj);
		void ParseSceneData(Object* obj, std::vector<float>& packData);
		void ParseGroupData(Object* group, std::vector<float>& packData);
		void ParseSphereData(Object* sphere, std::vector<float>& packData);
		void ParseBVHNodeData(Object* bvh_node, std::vector<float>& packData);

		std::map<std::shared_ptr<Material>, std::vector<unsigned int>> GetMat2Vec() const { return m_Mat2Vec; }
		std::vector<float> GetSceneData() { return m_SceneData; }
		void SetMat(std::map<std::shared_ptr<Material>, size_t> const mat2idx);
	private:
		std::vector<float> m_SceneData;
		std::map<Object*, size_t> m_Hitable2Idx;
		std::map<std::shared_ptr<Material>, std::vector<unsigned int>> m_Mat2Vec;

	};


}

