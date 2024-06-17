#pragma once
#include "BlackPearl/Map/MapManager.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BasicRenderer.h"
namespace BlackPearl {
	class MapRenderer:public BasicRenderer
	{
	public:
		MapRenderer(MapManager* mapManager);
		~MapRenderer();
		void Render(MapManager* mapManager);
	private:
		std::shared_ptr<Shader> m_MapShader;
		std::vector<float> m_MapPoint;
		std::shared_ptr<VertexArray> m_PointCubeVAO;


	};
}


