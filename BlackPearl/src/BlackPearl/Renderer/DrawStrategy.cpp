#include "pch.h"
#include "DrawStrategy.h"
#include "BlackPearl/Renderer/CullingManager.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"

namespace BlackPearl {
	extern CullingManager* g_cullingManager;
	const DrawItem* PassthroughDrawStrategy::GetNextItem()
	{
		return nullptr;
	}
	void PassthroughDrawStrategy::SetData(const DrawItem* data, size_t count)
	{
	}
	void InstancedOpaqueDrawStrategy::FillChunk()
	{
		//��׶�ü�

	}
	void InstancedOpaqueDrawStrategy::PrepareForView(const std::shared_ptr<Node>& rootNode, Scene* Scene, const IView& view)
	{
		std::vector<bool> OutVisibleNodes;
		m_VisibleObjs =  g_cullingManager->CullOctree(Scene, view, OutVisibleNodes);
		
		_PrepareDrawItems();
		//m_Walker = SceneGraphWalker(rootNode.get());
		//m_ViewFrustum = view.GetViewFrustum();
		m_InstanceChunk.clear();
		m_ReadPtr = 0;
	}
	const DrawItem* InstancedOpaqueDrawStrategy::GetNextItem()
	{
		return nullptr;
	}
	void InstancedOpaqueDrawStrategy::_PrepareDrawItems()
	{
		m_InstanceChunk.clear();
		for (size_t i = 0; i < m_VisibleObjs.size(); i++)
		{
			Object* obj = m_VisibleObjs[i];
			MeshRenderer* renderer = obj->GetComponent<MeshRenderer>();
			for (const auto & mesh: renderer->GetMeshes())
			{
				// һ��DrawItem ��Ӧһ��mesh
				DrawItem item;
				item.instance = meshInstance;
				item.mesh = mesh;
				item.geometry = geometry.get();
				item.material = geometry->material.get();
				item.buffers = item.mesh->buffers.get();
				item.cullMode = (item.material->doubleSided) ?
			}


			
		}


	}
	//void InstancedOpaqueDrawStrategy::_GetVisibleObject(const std::vector<bool>& outVisibleNodes)
	//{
	//	


	//}
	void TransparentDrawStrategy::PrepareForView(const std::shared_ptr<Node>& rootNode, Scene* Scene, const IView& view)
	{
	}
	const DrawItem* TransparentDrawStrategy::GetNextItem()
	{
		return nullptr;
	}
}