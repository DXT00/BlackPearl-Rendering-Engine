#include "pch.h"
#include "LumenSceneData.h"

namespace BlackPearl {

	LumenSceneData::LumenSceneData()
	{
	}

	LumenSceneData::~LumenSceneData()
	{
	}

	XMINT2 LumenSceneData::GetRadiosityAtlasSize() const
	{
		return PhysicalAtlasSize;
	}
}
