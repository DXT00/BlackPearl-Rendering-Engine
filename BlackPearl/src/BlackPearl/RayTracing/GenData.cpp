#include "pch.h"
#include "GenData.h"
#include "BlackPearl/Component/BasicInfoComponent/BasicInfo.h"
namespace BlackPearl {


	GenData::GenData(Object* obj)
	{
		m_HitableVisitor.reset(DBG_NEW GenData_HV());
		m_MaterialVisitor.reset(DBG_NEW GenData_MV());
		m_TextureVisitor.reset(DBG_NEW GenData_TV());

		m_HitableVisitor->ParseSceneData(obj,m_PackData);

		m_MaterialVisitor->ParseMatData(m_HitableVisitor->GetMat2Vec());
		m_HitableVisitor->SetMat(m_MaterialVisitor->GetMat2IdxMap());
		
		m_TextureVisitor->ParseColorData(m_MaterialVisitor->GetColor2Vec(), m_PackData);
		m_MaterialVisitor->SetTex(m_TextureVisitor->GetColor2IdxMap());
	}

}
