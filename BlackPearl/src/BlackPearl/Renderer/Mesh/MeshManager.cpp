#include "pch.h"
#include "MeshManager.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"

void BlackPearl::MeshManager::AddObjs(Object* obj)
{
	for (size_t i = 0; i < obj->GetComponent<MeshRenderer>()->GetMeshes().size(); i++)
	{

	}
}
