#pragma once
#include "pch.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/Material/MaterialColor.h"
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "BlackPearl/Animation/Bone.h"
#include "BlackPearl/RayTracing/Vertex.h"
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

	extern DynamicRHI::Type g_RHIType;
	class Model
	{
	public:
		Model(const std::string& path,
				const std::shared_ptr<Shader>shader,
				const bool isAnimated,
				const bool verticesSorted,
				const bool createMeshlet = false,
				const bool isMeshletModel = false,
				MeshletOption options = MeshletOption()
				)
			: m_Shader(shader) {
			m_HasAnimation = isAnimated;
			m_SortVertices = verticesSorted;
			m_AABB = std::make_shared<AABB>(glm::vec3(FLT_MAX), glm::vec3(FLT_MIN),true);
			if (g_RHIType == DynamicRHI::Type::D3D12) {
				//m_ModelLoader = DBG_NEW D3D12ModelLoader(isMeshletModel);
				if (createMeshlet && !isMeshletModel) {
					m_MeshletGenerator = std::make_shared<MeshletGenerator>();
					m_MeshletGenerator->Process(m_Meshes, options);
				}
				m_ModelLoader = DBG_NEW D3D12ModelLoader(isMeshletModel);
				m_ModelLoader->Load(m_Meshes, m_BoundingSphere, path);
				//LoadMeshletModel(m_BoundingSphere, path);
			}
			else {
				LoadModel(path);
				//TODO::
				//替换为				
				//m_ModelLoader = std::make_shared<OpenGLModelLoader>(isMeshletModel, createMeshlet, options);

			}
		};

		~Model() {
			GE_SAVE_DELETE(m_ModelLoader);

		};
		void LoadModel(const std::string& path);
		void LoadMeshletModel(BoundingSphere& bounding_sphere, const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		std::shared_ptr<Mesh> ProcessMesh(aiMesh* aimesh);
		std::shared_ptr<Mesh> ProcessMesh(aiMesh* aimesh, std::vector<Vertex>& v_vertex, bool sort_vertices);

		void LoadMaterialTextures(
			aiMaterial* material,
			aiTextureType type,
			Texture::Type typeName,
			std::shared_ptr<Material::TextureMaps>& textures);

		void LoadMaterialColors(
			aiMaterial* material,
			MaterialColor& colors);

		/*Bones*/
		void LoadBones(aiMesh* aimesh);
		std::shared_ptr<Material> LoadMaterial(aiMaterial* aiMaterial);
		std::vector<glm::mat4>  CalculateBoneTransform(float timeInSecond);
		void ReadHierarchy(float timeInDurationSecond, aiNode* node, glm::mat4 parentTransform);

		aiNodeAnim* FindNode(std::string nodeName, aiAnimation* animation);
		glm::mat4 AiMatrix4ToMat4(aiMatrix4x4 aiMatrix);
		glm::mat4 AiMatrix4ToMat4(aiMatrix3x3 aiMatrix);

		/*Interpolate*/
		glm::vec3    CalculateInterpolatePosition(float timeInDurationSecond, aiNodeAnim* nodeAnim);
		aiQuaternion CalculateInterpolateRotation(float timeInDurationSecond, aiNodeAnim* nodeAnim);
		glm::vec3    CalculateInterpolateScale(float timeInDurationSecond, aiNodeAnim* nodeAnim);

		std::vector<std::shared_ptr<Mesh>>       GetMeshes() const { return m_Meshes; }
		std::vector<std::shared_ptr<Mesh>>&  GetMeshlets() { return m_Meshes; }

		std::shared_ptr<Shader> GetShader()const { return m_Shader; }
		std::vector<Vertex>		GetMeshVertex() const { return m_Vertices; }


		uint32_t FindRotation(float AnimationTime, const aiNodeAnim* nodeAnim);
		uint32_t FindScaling(float AnimationTime, const aiNodeAnim* nodeAnim);
		std::shared_ptr<AABB> GetAABB() const { return m_AABB; }
	public:
		std::vector<std::shared_ptr<Mesh>> m_Meshes;
	private:

		void UpdateAABB(const glm::vec3& pos);

		std::shared_ptr<Shader> m_Shader = nullptr;
		std::map<int, std::shared_ptr<Material>> m_ModelMaterials;
		//std::vector<Mesh> m_Meshes;

		std::string m_Directory;
		Assimp::Importer m_Importer;

		/*need ordered vertices when building BVHNode or Triangle mesh*/
		bool m_SortVertices = false;

		/*Animation*/
		bool m_HasAnimation = false;
		/*Bones*/
		uint32_t m_BoneCount = 0;
		/*store every vertex's jointId and weight*/
		std::vector<VertexBoneData> m_BoneDatas;

		std::unordered_map<std::string, int> m_BoneNameToIdex;
		std::vector<Bone> m_Bones;
		const aiScene* m_Scene;
		std::string m_Path;
		//aiMatrix4x4 m_GlobalInverseTransform;
		glm::mat4 m_GlobalInverseTransform;

		/*Vertices Num,Indices Num*/
		uint32_t m_VerticesNum = 0;
		uint32_t m_VerticesIdx = 0;

		/*for raytracing, calculate bounding box*/
		std::vector<Vertex> m_Vertices;
		ModelLoader* m_ModelLoader;
		
		/* use for meshlet culling */
		BoundingSphere m_BoundingSphere;
		std::shared_ptr<MeshletGenerator> m_MeshletGenerator;
		std::vector<uint8_t>  m_Buffer;

		std::vector<float> m_ModelVertices;
		std::vector<uint32_t> m_ModelVerticesIntJointIdx;
		std::vector<float> m_ModelVerticesFloatWeight;

		std::vector<uint32_t> m_ModelIndices;
		std::shared_ptr<AABB> m_AABB = nullptr;
		bool m_FirstVertex = true;
		
	};

}