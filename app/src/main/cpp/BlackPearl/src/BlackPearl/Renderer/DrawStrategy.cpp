#include "pch.h"
#include "DrawStrategy.h"
#include "BlackPearl/Renderer/CullingManager.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"

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
		//视锥裁剪

	}
	void InstancedOpaqueDrawStrategy::PrepareForView(Scene* Scene, const IView& view)
	{
		std::vector<bool> OutVisibleNodes;
		m_VisibleObjs =  g_cullingManager->CullOctree(Scene, view, OutVisibleNodes);
		
		_PrepareDrawItems();
		//m_Walker = SceneGraphWalker(rootNode.get());
		//m_ViewFrustum = view.GetViewFrustum();
		//m_ReadPtr = 0;
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
			Transform* transform = obj->GetComponent<Transform>();
			for (const auto & mesh: renderer->GetMeshes())
			{
				///mesh->UpdateInstanceBuffer(transform);

				// 一个DrawItem 对应一个mesh
				DrawItem item;
				//item.instance = meshInstance;
				item.mesh = mesh.get();
				item.transform = transform;
				//item.geometry = geometry.get();
				item.material = mesh->material.get();
				item.buffers = item.mesh->buffers.get();
				item.cullMode = (item.material->GetProps().isDoubleSided) ? RasterCullMode::None : RasterCullMode::Back;
				item.distanceToCamera = 0;

				m_InstanceChunk.push_back(item);
			}


			
		}


	}
	//void InstancedOpaqueDrawStrategy::_GetVisibleObject(const std::vector<bool>& outVisibleNodes)
	//{
	//	


	//}
	void TransparentDrawStrategy::PrepareForView(Scene* Scene, const IView& view)
	{
	}
	const DrawItem* TransparentDrawStrategy::GetNextItem()
	{
		return nullptr;
	}
}