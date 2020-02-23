#include "pch.h"
//#include "AnimatedModel.h"
//#include "glm/glm.hpp"
//#include "assimp/mesh.h"
//#include "KeyFrame.h"
//namespace BlackPearl {
//	glm::mat4 AnimatedModel::AiMatrix4ToMat4(aiMatrix4x4 aiMatrix)
//	{
//		glm::mat4 res(0.0);
//		for (int i = 0; i < 4; i++)
//		{
//			for (int j = 0; j < 4; j++)
//			{
//				res[i][j] = aiMatrix[i][j];
//			}
//
//		}
//
//		return res;
//	}
//	void AnimatedModel::LoadModel(const std::string& path)
//	{
//		Assimp::Importer importer;
//		const aiScene* scene = importer.ReadFile(path,
//			aiProcess_MakeLeftHanded |
//			aiProcess_FlipWindingOrder |
//			aiProcess_FlipUVs |
//			aiProcess_PreTransformVertices |
//			aiProcess_CalcTangentSpace |
//			aiProcess_GenSmoothNormals |
//			aiProcess_Triangulate |
//			aiProcess_FixInfacingNormals |
//			aiProcess_FindInvalidData |
//			aiProcess_ValidateDataStructure | 0);
//		///*
//		//设定aiProcess_Triangulate，我们告诉Assimp，如果模型不是（全部）由三角形组成，
//		//它需要将模型所有的图元形状变换为三角形。
//		//
//		//aiProcess_FlipUVs将在处理的时候翻转y轴的纹理坐标（你可能还记得我们在纹理教程中说过，
//		//在OpenGL中大部分的图像的y轴都是反的，所以这个后期处理选项将会修复这个）
//		//
//		//*/
//		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//			GE_CORE_ERROR("ERROR::ASSIMP:: {0}", importer.GetErrorString())
//				return;
//		}
//		m_Directory = path.substr(0, path.find_last_of('/'));
//		LoadAnimation(scene);
//		ProcessNode(scene->mRootNode, scene);
//	}
//	void AnimatedModel::LoadBones(aiMesh* aimesh)
//	{
//		m_BoneCount += aimesh->mNumBones;
//		
//		for (unsigned int i = 0; i < aimesh->mNumBones; i++)
//		{
//			int boneIdx = i;
//			aiBone* bone = aimesh->mBones[i];
//			std::string name = bone->mName.data;
//			aiMatrix4x4 transformToParent = bone->mOffsetMatrix;
//			unsigned int numberOfVertex = bone->mNumWeights;
//			for (int i = 0; i < numberOfVertex; i++)
//			{
//				/*index of vertex affect by this bone*/
//				unsigned int vertexIdx = bone->mWeights[i].mVertexId;
//				/*weight of this bone to the vertex*/
//				float boneWeight = bone->mWeights[i].mWeight;
//
//				GE_ASSERT(m_BoneDatas[vertexIdx].jointIdx.size() < MAX_WEIGHT, "related bone's number larger than MAX_WEIGHT");
//				m_BoneDatas[vertexIdx].jointIdx.push_back(boneIdx);
//				m_BoneDatas[vertexIdx].weights.push_back(boneWeight);
//				m_BoneDatas[vertexIdx].transformToParent.push_back(AiMatrix4ToMat4(transformToParent));
//
//			}
//
//
//		}
//	}
//	void AnimatedModel::LoadAnimation(const aiScene* scene)
//	{
//		unsigned int animationCount = scene->mNumAnimations;
//		for (int i = 0; i < animationCount; i++)
//		{	
//			aiAnimation* animation = scene->mAnimations[i];
//			double durationTick = animation->mDuration;
//			aiString name = animation->mName;
//			unsigned int channelNum = animation->mNumChannels;
//			double tickPerSecond = animation->mTicksPerSecond;
//
//			for (int i = 0; i < channelNum; i++)
//			{
//				aiNodeAnim *channel = animation->mChannels[i];
//				
//			}
//
//		}
//
//
//	}
//}
//
