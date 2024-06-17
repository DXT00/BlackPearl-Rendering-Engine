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
//		//�趨aiProcess_Triangulate�����Ǹ���Assimp�����ģ�Ͳ��ǣ�ȫ��������������ɣ�
//		//����Ҫ��ģ�����е�ͼԪ��״�任Ϊ�����Ρ�
//		//
//		//aiProcess_FlipUVs���ڴ����ʱ��תy����������꣨����ܻ��ǵ�����������̳���˵����
//		//��OpenGL�д󲿷ֵ�ͼ���y�ᶼ�Ƿ��ģ�����������ڴ���ѡ����޸������
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
