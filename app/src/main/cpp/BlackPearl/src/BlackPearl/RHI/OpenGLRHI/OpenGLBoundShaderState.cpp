#include "pch.h"
#include "OpenGLBoundShaderState.h"

namespace BlackPearl {
	const TBitArray& BoundShaderState::GetTextureNeeds(int32_t& OutMaxTextureStageUsed)
	{
		// TODO: 在此处插入 return 语句
		return TBitArray();
	}
	const TBitArray& BlackPearl::BoundShaderState::GetUAVNeeds(int32_t& OutMaxUAVUnitUsed) const
	{
		return TBitArray();

		// TODO: 在此处插入 return 语句
	}

}
