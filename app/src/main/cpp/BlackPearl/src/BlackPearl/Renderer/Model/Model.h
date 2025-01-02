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
//#include "ModelLoader.h"

#include "BlackPearl/RHI/DynamicRHI.h"
#include "BlackPearl/AABB/AABB.h"
#include "BlackPearl/Animation/Bone.h"
//#include <assimp/material.h>

//#include <assimp/cimport.h>
namespace BlackPearl {

	class Bone;
	struct AnimationInfo {
		AnimationInfo() {
			animationNum = 0;
			animationDuration = 0;
			animationTickPerSecond = 0;
		}
		uint32_t animationNum;
		float animationDuration;
		float animationTickPerSecond;
		const aiScene* modelScene;
		std::vector<Bone> bones;

		/*Bones*/
		uint32_t boneCount = 0;
		/*store every vertex's jointId and weight*/
		std::vector<VertexBoneData> boneDatas;

		std::unordered_map<std::string, int> boneNameToIdex;
	};

	struct ModelDesc {
		bool bIsAnimated;
		bool bSortVerticces;
		bool bCreateMeshlet;
		bool bIsMeshletModel;
		MeshletOption options;
		std::shared_ptr<Shader>shader;
		AnimationInfo animationInfo;

		ModelDesc() 
			:bIsAnimated(false),
			bSortVerticces(false),
			bCreateMeshlet(false),
			bIsMeshletModel(false),
			options(MeshletOption()),
			animationInfo(AnimationInfo()),
			shader(nullptr)
		{}
	};
	class Model
	{
	public:
		Model(const std::string& path,
			 const ModelDesc& desc
				)
			: desc(desc)
			{
			boundingbox = std::make_shared<AABB>(math::float3(FLT_MAX), math::float3(FLT_MIN),true);
			if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
				
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
			//GE_SAVE_DELETE(m_ModelLoader);

		};
		

		std::vector<std::shared_ptr<Mesh>>       GetMeshes() const { return meshes; }
		std::vector<std::shared_ptr<Mesh>>&  GetMeshlets() { return meshes; }

		std::shared_ptr<Shader> GetShader()const { return desc.shader; }
		std::vector<Vertex>		GetMeshVertex() const { return vertices; }


		
		std::shared_ptr<AABB> GetAABB() const { return boundingbox; }
	public:
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::shared_ptr<AABB> boundingbox;
		ModelDesc desc;

		/*for raytracing, calculate bounding box*/
		std::vector<Vertex> vertices;



	private:

	
		//ModelLoader* m_ModelLoader;
		
		/* use for meshlet culling */
		//BoundingSphere m_BoundingSphere;

	

	
	
		
	};

}