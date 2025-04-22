#include "pch.h"
#include "Lumen/LumenSceneData.h"

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
