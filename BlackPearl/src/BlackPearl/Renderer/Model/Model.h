#pragma once
#include "pch.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/Material/MaterialColor.h"
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "BlackPearl/Renderer/Mesh/Vertex.h"
#include "BlackPearl/Renderer/Mesh/Meshlet.h"
#include "BlackPearl/Component/BoundingSphereComponent/BoundingSphere.h"
#include "ModelLoader.h"
#include "BlackPearl/Renderer/Mesh/MeshletGenerator.h"
#include "BlackPearl/RHI/DynamicRHI.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12ModelLoader.h"
#include "BlackPearl/AABB/AABB.h"
//#include <assimp/material.h>

//#include <assimp/cimport.h>
namespace BlackPearl {

	extern ModelLoader* g_modelLoader;

	struct ModelDesc {
		bool bIsAnimated;
		bool bSortVerticces;
		bool bCreateMeshlet;
		bool bIsMeshletModel;
		MeshletOption options;
		std::shared_ptr<Shader>shader;

		ModelDesc() 
			:bIsAnimated(false),
			bSortVerticces(false),
			bCreateMeshlet(false),
			bIsMeshletModel(false),
			options(MeshletOption()),
			shader(nullptr)
		{}
	};
	class Model
	{
	public:
		Model(const std::string& path,
			 const ModelDesc& desc
				)
			: desc(desc),
			shader(desc.shader) {
			boundingbox = std::make_shared<AABB>(glm::vec3(FLT_MAX), glm::vec3(FLT_MIN),true);
			if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
				//m_ModelLoader = DBG_NEW D3D12ModelLoader(isMeshletModel);
				if (desc.bCreateMeshlet && !desc.bIsMeshletModel) {
					m_MeshletGenerator = std::make_shared<MeshletGenerator>();
					m_MeshletGenerator->Process(meshes, desc.options);
				}
				m_ModelLoader = DBG_NEW D3D12ModelLoader(desc.bIsMeshletModel);
				m_ModelLoader->Load(meshes, m_BoundingSphere, path);
				//LoadMeshletModel(m_BoundingSphere, path);
			}
			else {
				//g_modelLoader->LoadModel(path);
				//TODO::
				//替换为				
				//m_ModelLoader = DBG_NEW ModelLoader();

			}
		};

		~Model() {
			GE_SAVE_DELETE(m_ModelLoader);

		};
		

		std::vector<std::shared_ptr<Mesh>>       GetMeshes() const { return meshes; }
		std::vector<std::shared_ptr<Mesh>>&  GetMeshlets() { return meshes; }

		std::shared_ptr<Shader> GetShader()const { return shader; }
		std::vector<Vertex>		GetMeshVertex() const { return vertices; }


		
		std::shared_ptr<AABB> GetAABB() const { return boundingbox; }
	public:
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::shared_ptr<AABB> boundingbox;
		std::shared_ptr<Shader> shader = nullptr;
		ModelDesc desc;

		/*for raytracing, calculate bounding box*/
		std::vector<Vertex> vertices;


	private:

	
		ModelLoader* m_ModelLoader;
		
		/* use for meshlet culling */
		BoundingSphere m_BoundingSphere;
		std::shared_ptr<MeshletGenerator> m_MeshletGenerator;
	

	
	
		
	};

}