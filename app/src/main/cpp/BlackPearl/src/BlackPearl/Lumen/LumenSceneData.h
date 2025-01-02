#pragma once
#include "BlackPearl/Renderer/RenderTargets.h"
#include "LumenCard.h"
#include "LumenStruct.h"
#include "BlackPearl/Common/CommonFunc.h"
#include "BlackPearl/Renderer/Buffer/Buffer.h"
#include "DirectXMath.h"
using namespace DirectX;
namespace BlackPearl {

	class LumenSceneData
	{
	public:

		LumenSceneData();
		~LumenSceneData();

		std::vector<LumenCard> m_Cards;
		std::set<int32_t> m_CardIndicesToUpdateInBuffer;
		std::shared_ptr<ShaderStorageBuffer> m_CardBuffer;

		// Captured from the triangle scene
		//std::shared_ptr<RenderTarget> m_AlbedoAtlas;
		//std::shared_ptr<RenderTarget> m_OpacityAtlas;
		//std::shared_ptr<RenderTarget> m_NormalAtlas;
		//std::shared_ptr<RenderTarget> m_EmissiveAtlas;
		//std::shared_ptr<RenderTarget> m_DepthAtlas;

		//// Generated
		//std::shared_ptr<RenderTarget> m_DirectLightingAtlas;
		//std::shared_ptr<RenderTarget> m_IndirectLightingAtlas;
		//std::shared_ptr<RenderTarget> m_RadiosityNumFramesAccumulatedAtlas;
		//std::shared_ptr<RenderTarget> m_FinalLightingAtlas;

		//// Radiosity probes
		//std::shared_ptr<RenderTarget> m_RadiosityTraceRadianceAtlas;
		//std::shared_ptr<RenderTarget> m_RadiosityTraceHitDistanceAtlas;
		//std::shared_ptr<RenderTarget> m_RadiosityProbeSHRedAtlas;
		//std::shared_ptr<RenderTarget> m_RadiosityProbeSHGreenAtlas;
		//std::shared_ptr<RenderTarget> m_RadiosityProbeSHBlueAtlas;

		//page table //TODO::
		//std::vector<FLumenPageTableEntry> PageTable;
		// Virtual surface cache page table
		XMINT2 PhysicalAtlasSize = XMINT2(0, 0);


	public:
		//int32_t GetNumCardPages() const { return PageTable.size(); }
		XMINT2 GetPhysicalAtlasSize() const { return PhysicalAtlasSize; }
		XMINT2 GetRadiosityAtlasSize() const;
	};

}

