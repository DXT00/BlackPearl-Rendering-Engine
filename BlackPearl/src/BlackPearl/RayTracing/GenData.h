#pragma once
#include<memory>
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/RayTracing/GenData_HV.h"
#include "BlackPearl/RayTracing/GenData_MV.h"
#include "BlackPearl/RayTracing/GenData_TV.h"

namespace BlackPearl {
	class GenData
	{
	public:
		GenData(Object* obj);
		std::vector<float> GetSceneData() const { return m_HitableVisitor->GetSceneData(); }
		std::vector<float> GetMatData() const { return m_MaterialVisitor->GetMatData(); }
		std::vector<float> GetTexData() const { return m_TextureVisitor->GetTextureData(); }
		std::vector<float> GetPackData() const { return m_PackData; }
		std::map<std::shared_ptr<Texture>, size_t> GetImg2RenderIdMap() const { return m_TextureVisitor->GetImg2RenderIdMap(); }
		std::map<std::shared_ptr<CubeMapTexture>, size_t> GetCubeMap2RenderIdMap() const { return m_TextureVisitor->GetCubeMap2RenderIdMap(); }
		std::map<std::shared_ptr<Texture>, size_t> GetTex2Idx() const { return m_TextureVisitor->GetTex2IdxMap(); }


	private:
		std::shared_ptr<GenData_HV> m_HitableVisitor;
		std::shared_ptr<GenData_MV> m_MaterialVisitor;
		std::shared_ptr<GenData_TV> m_TextureVisitor;
		std::vector<float> m_PackData;

	};

}