#include "pch.h"
#include "PBRRenderer.h"

namespace BlackPearl {


	void PBRRenderer::Render(const std::vector<Object*>& objs)
	{
		m_PbrShader->Bind();
		DrawObjects(objs, m_PbrShader);
	}

	void PBRRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
	{
		commandList->beginMarker("BasePass");
		SceneData* view = Renderer::GetSceneData();
		SceneData* preView = Renderer::GetPreSceneData();

		m_DrawStrategy->PrepareForView(scene, *view);
		RenderPassTemplate(commandList, targetFramebuffer, view, m_DrawStrategy);

















		commandList->endMarker();
	}

	void PBRRenderer::Render(Object * obj)
	{
		m_PbrShader->Bind();
		DrawObject(obj, m_PbrShader);
	}

	PBRRenderer::~PBRRenderer()
	{
	}

}
