#pragma once
#include "pch.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/Material/MaterialColor.h"
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "BlackPearl/Animation/Bone.h"
#include "BlackPearl/RayTracing/Vertex.h"
//#include <assimp/material.h>

//#include <assimp/cimport.h>
namespace BlackPearl {

	class Model
	{
	public:
		Model(const std::string& path, const std::shared_ptr<Shader>shader, const bool isAnimated)
			:m_Shader(shader) {
			m_HasAnimation = isAnimated;
			LoadModel(path);
		};

		~Model() {
			//GE_SAVE_DELETE(m_Scene);

		};
		void LoadModel(const std::string& path);

		Mesh ProcessMesh(aiMesh* aimesh, std::vector<Vertex>& v_vertex);
		Mesh ProcessMesh(aiMesh* aimesh, std::vector<Vertex>& v_vertex, bool face);

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
		glm::vec3 CalculateInterpolatePosition(float timeInDurationSecond, aiNodeAnim* nodeAnim);
		aiQuaternion CalculateInterpolateRotation(float timeInDurationSecond, aiNodeAnim* nodeAnim);
		glm::vec3 CalculateInterpolateScale(float timeInDurationSecond, aiNodeAnim* nodeAnim);

		std::vector<Mesh>       GetMeshes()const { return m_Meshes; }
		std::shared_ptr<Shader> GetShader()const { return m_Shader; }
		std::vector<Vertex>		GetMeshVertex() const { return m_Vertices; }


		unsigned int FindRotation(float AnimationTime, const aiNodeAnim* nodeAnim);
		unsigned int FindScaling(float AnimationTime, const aiNodeAnim* nodeAnim);
	private:
		std::shared_ptr<Shader> m_Shader;
		std::vector<Mesh> m_Meshes;
		std::map<int, std::shared_ptr<Material>> m_ModelMaterials;

		std::string m_Directory;
		Assimp::Importer m_Importer;

		/*Animation*/
		bool m_HasAnimation = false;
		/*Bones*/
		unsigned int m_BoneCount = 0;
		/*store every vertex's jointId and weight*/
		std::vector<VertexBoneData> m_BoneDatas;

		std::unordered_map<std::string, int> m_BoneNameToIdex;
		std::vector<Bone> m_Bones;
		const aiScene* m_Scene;
		std::string m_Path;
		//aiMatrix4x4 m_GlobalInverseTransform;
		glm::mat4 m_GlobalInverseTransform;

		/*Vertices Num,Indices Num*/
		unsigned int m_VerticesNum = 0;
		unsigned int m_VerticesIdx = 0;

		/*for raytracing, calculate bounding box*/
		std::vector<Vertex> m_Vertices;

	};

}