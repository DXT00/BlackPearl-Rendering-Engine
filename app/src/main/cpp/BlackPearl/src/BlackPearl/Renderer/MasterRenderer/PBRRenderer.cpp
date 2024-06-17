#include "pch.h"
#include "PBRRenderer.h"

namespace BlackPearl {


	void PBRRenderer::Render(const std::vector<Object*>& objs)
	{
		m_PbrShader->Bind();
		DrawObjects(objs, m_PbrShader);
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
