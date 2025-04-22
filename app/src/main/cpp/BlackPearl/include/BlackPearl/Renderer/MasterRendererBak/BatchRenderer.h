#pragma once
#include "BlackPearl/Node/BatchNode.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
namespace BlackPearl {

	class BatchRenderer: public BasicRenderer
	{
	public:
		BatchRenderer();
		~BatchRenderer();

		

		void Render(BatchNode* batchNode, std::shared_ptr<Shader> shader);

	private:

		//void UpdateBatchNodeTransform(BatchNode* batchNode);
		//void UpdateObjsTransform(BatchNode* batchNode);

	};

}

