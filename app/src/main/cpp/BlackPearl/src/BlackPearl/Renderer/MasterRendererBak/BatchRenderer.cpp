#include "pch.h"
#include "BatchRenderer.h"
#include "BlackPearl/Renderer/Shader/Shader.h"

namespace BlackPearl {
	BatchRenderer::BatchRenderer()
	{
	}

	BatchRenderer::~BatchRenderer()
	{
	}

	void BatchRenderer::Render(BatchNode* batchNode, std::shared_ptr<Shader> shader)
	{
		batchNode->UpdateObjs();
		DrawBatchNode(batchNode, shader);
	}

}
