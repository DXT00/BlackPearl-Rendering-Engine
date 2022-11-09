#include "pch.h"
#include "MapRenderer.h"

namespace BlackPearl {

	MapRenderer::MapRenderer(MapManager* mapManager)
	{
		m_MapShader.reset(DBG_NEW Shader("assets/shaders/map/map.glsl"));
		std::vector<Area> areasList = mapManager->GetAreasList();
		float halfMapSize = (float)mapManager->GetHalfMapSize();
		for (int i = 0; i < areasList.size(); i++)
		{
			m_MapPoint.push_back(areasList[i].m_Xmin- halfMapSize);
			m_MapPoint.push_back(areasList[i].m_Ymin - halfMapSize);
			m_MapPoint.push_back(areasList[i].m_Zmin - halfMapSize);
		}
		m_PointCubeVAO.reset(DBG_NEW VertexArray());
		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(DBG_NEW VertexBuffer(m_MapPoint));

		VertexBufferLayout layout = {
		{ElementDataType::Float3,"aPos",false,0},	
		};
		vertexBuffer->SetBufferLayout(layout);
		m_PointCubeVAO->AddVertexBuffer(vertexBuffer);
		m_PointCubeVAO->UnBind();

	}

	MapRenderer::~MapRenderer()
	{
	}

	void MapRenderer::Render(MapManager* mapManager)
	{
		m_MapShader->Bind();
		m_MapShader->SetUniform1f("u_AreaSize", (float)mapManager->GetAreaSize());
		m_MapShader->SetUniformMat4f("u_ProjectionView", Renderer::GetSceneData()->ProjectionViewMatrix);
		m_MapShader->SetUniformMat4f("u_Projection", Renderer::GetSceneData()->ProjectionMatrix);
		m_MapShader->SetUniformMat4f("u_View", Renderer::GetSceneData()->ViewMatrix);
		m_PointCubeVAO->Bind();
		glDrawArrays(GL_POINTS, 0, m_MapPoint.size());
		glBindVertexArray(0);
		m_MapShader->Unbind();


	}

}