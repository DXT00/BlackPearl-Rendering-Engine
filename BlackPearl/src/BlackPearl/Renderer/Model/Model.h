//#pragma once
//#include "pch.h"
//#include "BlackPearl/Renderer/Mesh/Mesh.h"
//#include "BlackPearl/Renderer/Material/MaterialColor.h"
//#include "assimp/Importer.hpp"	//OO version Header!
//#include "assimp/postprocess.h"
//#include "assimp/scene.h"
//#include "BlackPearl/Animation/Bone.h"
////#include <assimp/material.h>
//
////#include <assimp/cimport.h>
//namespace BlackPearl {
//
//	class Model
//	{
//	public:
//
//		Model(const std::string& path,const std::shared_ptr<Shader>shader,const bool isAnimated)
//		:m_Shader(shader){
//			m_HasAnimation = isAnimated;
//			LoadModel(path);
//
//		};
//		//void Draw(const glm::mat4 & model, const LightSources& lightSources);
//
//		~Model() = default;
//		virtual void LoadModel(const std::string& path);
//
//		void ProcessNode(aiNode *node);
//		Mesh ProcessMesh(aiMesh *mesh);
//		void LoadMaterialTextures(
//			aiMaterial * material,
//			aiTextureType type,
//			Texture::Type typeName,
//			std::shared_ptr<Material::TextureMaps> &textures);
//
//		void LoadMaterialColors(
//			aiMaterial * material,
//			 MaterialColor &colors);
//
//		/*Bones*/
//		void LoadBones(aiMesh* aimesh);
//		std::vector<glm::mat4>  CalculateBoneTransform(float timeInSecond);
//		void ReadHierarchy(float timeInDurationSecond,aiNode* node,aiAnimation* animation, glm::mat4 parentTransform);
//		aiNodeAnim* FindNode(std::string nodeName, aiAnimation* animation);
//		glm::mat4 AiMatrix4ToMat4(aiMatrix4x4 aiMatrix);
//		glm::mat4 AiMatrix4ToMat4(aiMatrix3x3 aiMatrix);
//
//		/*Interpolate*/
//		glm::vec3 CalculateInterpolatePosition(float timeInDurationSecond, aiNodeAnim* nodeAnim);
//		aiQuaternion CalculateInterpolateRotation(float timeInDurationSecond, aiNodeAnim* nodeAnim);
//		glm::vec3 CalculateInterpolateScale(float timeInDurationSecond, aiNodeAnim* nodeAnim);
//
//		std::vector<Mesh>       GetMeshes()const { return m_Meshes; }
//		std::shared_ptr<Shader> GetShader()const { return m_Shader; }
//
//	protected:
//		/*unsigned int GetVertiesNum() {
//			unsigned int num = 0;
//			for (Mesh mesh : m_Meshes)
//				num += mesh.GetVerticesSize();
//			return num;
//		}*/
//
//		std::shared_ptr<Shader> m_Shader;//多个Mesh使用同一个shader
//		std::vector<Mesh> m_Meshes;
//
//		std::string m_Directory;
//
//		/*Animation*/
//		bool m_HasAnimation = false;
//		//std::vector<Texture> m_Textures;//存储所有的已经load的Textures.
//		/*Bones*/
//		unsigned int m_BoneCount=0;
//		std::vector<VertexBoneData> m_BoneDatas;
//
//		std::unordered_map<std::string, int> m_BoneNameToIdex;
//		std::vector<Bone> m_Bones;
//		const aiScene *m_Scene;
//		//std::shared_ptr<aiScene> m_Scene;
//		glm::mat4 m_GlobalInverseTransform;
//
//		/*Vertices Num,Indices Num*/
//		unsigned int m_VerticesNum = 0;
//
//		unsigned int m_VerticesIdx = 0;
//
//		aiAnimation m_Animation;
//		std::string m_Path;
//		//importer要放在外头，否则importer解析后 m_Scene会变成野指针！
//		Assimp::Importer m_Importer;
//
//	};
//
//}

#pragma once
#include "pch.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/Material/MaterialColor.h"
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "BlackPearl/Animation/Bone.h"
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

		~Model() = default;
		void LoadModel(const std::string& path);

		void ProcessNode(aiNode* node);
		Mesh ProcessMesh(aiMesh* mesh);
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
		std::vector<glm::mat4>  CalculateBoneTransform(float timeInSecond);
		void ReadHierarchy(float timeInDurationSecond, aiNode* node, aiMatrix4x4 parentTransform);
		aiNodeAnim* FindNode(std::string nodeName, aiAnimation* animation);
		glm::mat4 AiMatrix4ToMat4(aiMatrix4x4 aiMatrix);
		glm::mat4 AiMatrix4ToMat4(aiMatrix3x3 aiMatrix);

		/*Interpolate*/
		glm::vec3 CalculateInterpolatePosition(float timeInDurationSecond, aiNodeAnim* nodeAnim);
		aiQuaternion CalculateInterpolateRotation(float timeInDurationSecond, aiNodeAnim* nodeAnim);
		glm::vec3 CalculateInterpolateScale(float timeInDurationSecond, aiNodeAnim* nodeAnim);

		std::vector<Mesh>       GetMeshes()const { return m_Meshes; }
		std::shared_ptr<Shader> GetShader()const { return m_Shader; }


		unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	private:
		std::shared_ptr<Shader> m_Shader;
		std::vector<Mesh> m_Meshes;

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
		aiMatrix4x4 m_GlobalInverseTransform;

		/*Vertices Num,Indices Num*/
		unsigned int m_VerticesNum = 0;
		unsigned int m_VerticesIdx = 0;
	};

}