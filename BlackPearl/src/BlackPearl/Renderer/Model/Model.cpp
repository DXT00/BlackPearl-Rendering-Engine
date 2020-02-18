#include "pch.h"
#include "Model.h"
#include "BlackPearl/Renderer/Material/Texture.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include <glm/gtc/matrix_transform.hpp>
//#include "BlackPearl/Renderer/Renderer.h"
namespace BlackPearl {

	static void glmInsertVector(glm::vec2 v, std::vector<float>& vec) {
		vec.push_back(v.x);
		vec.push_back(v.y);

	}
	static void glmInsertVector(glm::vec3 v, std::vector<float>& vec) {
		vec.push_back(v.x);
		vec.push_back(v.y);
		vec.push_back(v.z);
	}
	static void glmInsertVector(glm::vec4 v, std::vector<float>& vec) {
		vec.push_back(v.x);
		vec.push_back(v.y);
		vec.push_back(v.z);
		vec.push_back(v.w);

	}

	static void glmInsertVector(glm::u32vec4 v, std::vector<unsigned int>& vec) {
		vec.push_back(v.x);
		vec.push_back(v.y);
		vec.push_back(v.z);
		vec.push_back(v.w);

	}
	void Model::LoadModel(const std::string& path)
	{
		m_Path = path;
		//Assimp::Importer importer;
		m_Scene = m_Importer.ReadFile(path,
				aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_FlipUVs |
				aiProcess_JoinIdenticalVertices);
			/*aiProcess_MakeLeftHanded |
			aiProcess_FlipWindingOrder |
			aiProcess_FlipUVs |
			aiProcess_PreTransformVertices |
			aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			aiProcess_Triangulate |
			aiProcess_FixInfacingNormals |
			aiProcess_FindInvalidData |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ValidateDataStructure | 0);*/
		///*
		//设定aiProcess_Triangulate，我们告诉Assimp，如果模型不是（全部）由三角形组成，
		//它需要将模型所有的图元形状变换为三角形。
		//
		//aiProcess_FlipUVs将在处理的时候翻转y轴的纹理坐标（你可能还记得我们在纹理教程中说过，
		//在OpenGL中大部分的图像的y轴都是反的，所以这个后期处理选项将会修复这个）
		//
		//*/

		if (!m_Scene || m_Scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode) {
			GE_CORE_ERROR("ERROR::ASSIMP:: {0}", m_Importer.GetErrorString())
				return;
		}
		//m_Scene = scene;
		m_Directory = path.substr(0, path.find_last_of('/'));

		for (int i = 0; i < m_Scene->mNumMeshes; i++)
		{
			m_VerticesNum += m_Scene->mMeshes[i]->mNumVertices;
		}
		m_GlobalInverseTransform = m_Scene->mRootNode->mTransformation;
		m_GlobalInverseTransform.Inverse();
		

		m_BoneDatas.resize(m_VerticesNum);
		//m_Animation = *(m_Scene->mAnimations[0]);
		for (int i = 0; i < m_Scene->mNumMeshes; i++)
		{
			aiMesh *mesh= m_Scene->mMeshes[i];
			/*LoadBones(m_Scene->mMeshes[i]);
			m_VerticesIdx+= m_Scene->mMeshes[i]->mNumVertices;*/
			m_Meshes.push_back(ProcessMesh(mesh));
		}
		//ProcessNode(m_Scene->mRootNode);
	}


	void Model::ProcessNode(aiNode* node)
	{
		GE_CORE_INFO("node name"+std::string(node->mName.data));
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = m_Scene->mMeshes[node->mMeshes[i]];

			m_Meshes.push_back(ProcessMesh(mesh));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i]);
		}
	}
	void Model::LoadBones(aiMesh* aimesh)
	{

		for (unsigned int i = 0; i < aimesh->mNumBones; i++)
		{
			unsigned int boneIdx;
			aiBone* bone = aimesh->mBones[i];
			std::string name(bone->mName.data);
			aiMatrix4x4 meshToBoneTranform = bone->mOffsetMatrix;

			if (m_BoneNameToIdex.find(name) != m_BoneNameToIdex.end()) {
				boneIdx = m_BoneNameToIdex[name];
				m_Bones[boneIdx].meshToBoneTranform = (meshToBoneTranform);
			}
			else {
				m_BoneNameToIdex.insert(std::pair(name, m_BoneCount));
				boneIdx = m_BoneCount;
				m_Bones.push_back({ boneIdx,(meshToBoneTranform),glm::mat4(1.0) });
				m_BoneCount++;
			}

			unsigned int numberOfVertex = bone->mNumWeights;
			for (int i = 0; i < numberOfVertex; i++)
			{
				/*index of vertex affect by this bone*/
				unsigned int vertexIdx = bone->mWeights[i].mVertexId;
				/*weight of this bone to the vertex*/
				float boneWeight = bone->mWeights[i].mWeight;

				GE_ASSERT(m_BoneDatas[m_VerticesIdx + vertexIdx].currentPos < MAX_WEIGHT, "related bone's number larger than MAX_WEIGHT");
				m_BoneDatas[m_VerticesIdx + vertexIdx].jointIdx[m_BoneDatas[m_VerticesIdx + vertexIdx].currentPos] = boneIdx;
				m_BoneDatas[m_VerticesIdx + vertexIdx].weights[m_BoneDatas[m_VerticesIdx + vertexIdx].currentPos] = boneWeight;
				m_BoneDatas[m_VerticesIdx + vertexIdx].currentPos++;
			}


		}
	}
	Mesh Model::ProcessMesh(aiMesh* aimesh)
	{
		;
		std::vector<float> vertices;
		std::vector<unsigned int> verticesIntjointIdx;
		std::vector<float> verticesfloatWeight;

		std::vector<unsigned int> indices;
		// maps
		std::shared_ptr<Material::TextureMaps> textures(DBG_NEW Material::TextureMaps());
		MaterialColor  colors;
		VertexBufferLayout layout1, layout2;
		VertexBufferLayout layout = {
			{ElementDataType::Float3,"aPos",false,0},
			{ElementDataType::Float3,"aNormal",false,1},
			{ElementDataType::Float2,"aTexCoords",false,2}
			//	{ElementDataType::Float3,"aTangent",false},
			//	{ElementDataType::Float3,"aBitTangent",false},

		};
		if (m_HasAnimation) {
			layout1 = { { ElementDataType::Int4,"aJointIndices",false,3 } };
			layout2 = { { ElementDataType::Float4,"aWeights",false,4} };
		}

		if (m_HasAnimation && aimesh->mNumBones >= 0)
			LoadBones(aimesh);

		for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
		{

			glm::vec3 pos;
			pos.x = aimesh->mVertices[i].x;
			pos.y = aimesh->mVertices[i].y;
			pos.z = aimesh->mVertices[i].z;
			glmInsertVector(pos, vertices);

			glm::vec3 normal;
			normal.x = aimesh->mNormals[i].x;
			normal.y = aimesh->mNormals[i].y;
			normal.z = aimesh->mNormals[i].z;
			glmInsertVector(normal, vertices);

			glm::vec2 textCords = glm::vec2(0.0f, 0.0f);
			if (aimesh->mTextureCoords[0]) {//判断顶点是否有材质属性

				textCords.x = aimesh->mTextureCoords[0][i].x;
				textCords.y = aimesh->mTextureCoords[0][i].y;
			}
			glmInsertVector(textCords, vertices);

			if (m_HasAnimation && m_BoneDatas.size() >= 0) {
				unsigned int vertexIdx = m_VerticesIdx + i;
				glmInsertVector(glm::u32vec4(m_BoneDatas[vertexIdx].jointIdx[0], m_BoneDatas[vertexIdx].jointIdx[1], m_BoneDatas[vertexIdx].jointIdx[2], m_BoneDatas[vertexIdx].jointIdx[3]), verticesIntjointIdx);
				glmInsertVector(glm::vec4(m_BoneDatas[vertexIdx].weights[0], m_BoneDatas[vertexIdx].weights[1], m_BoneDatas[vertexIdx].weights[2], m_BoneDatas[vertexIdx].weights[3]), verticesfloatWeight);

				GE_ASSERT(m_BoneDatas[vertexIdx].weights[0] + m_BoneDatas[vertexIdx].weights[1] + m_BoneDatas[vertexIdx].weights[2] + m_BoneDatas[vertexIdx].weights[3] == 1.0f, "total weight!=1");
			}
			//tangent
			//glm::vec3 tangent = glm::vec3(0.0f);
			//if (aimesh->mTangents != NULL) {
			//	tangent.x = aimesh->mTangents[i].x;
			//	tangent.y = aimesh->mTangents[i].y;
			//	tangent.z = aimesh->mTangents[i].z;

			//}
			//glmInsertVector(tangent, vertices);

			////bittangent
			//glm::vec3 bitTangent = glm::vec3(0.0f);
			//if (aimesh->mBitangents != NULL) {
			//	bitTangent.x = aimesh->mBitangents[i].x;
			//	bitTangent.y = aimesh->mBitangents[i].y;
			//	bitTangent.z = aimesh->mBitangents[i].z;
			//}

			//glmInsertVector(bitTangent, vertices);

		}


		for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
		{
			aiFace face = aimesh->mFaces[i];
			GE_ASSERT(face.mNumIndices == 3, "face.mNumIndices!=3");
			for (unsigned int i = 0; i < face.mNumIndices; i++)
			{
				indices.push_back(face.mIndices[i]);

			}
		}
		if (aimesh->mMaterialIndex >= 0) {
			aiMaterial* material = m_Scene->mMaterials[aimesh->mMaterialIndex];
			LoadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::Type::DiffuseMap, textures);
			LoadMaterialTextures(material, aiTextureType_SPECULAR, Texture::Type::SpecularMap, textures);
			LoadMaterialTextures(material, aiTextureType_NORMALS, Texture::Type::NormalMap, textures);
			LoadMaterialTextures(material, aiTextureType_HEIGHT, Texture::Type::HeightMap, textures);
			//LoadMaterialTextures(material, aiTextureType_DISPLACEMENT, Texture::Type::DiffuseMap, textures);//TODO

			LoadMaterialColors(material, colors);
		}



		float* vertices_ = DBG_NEW float[vertices.size()];
		//if (vertices.size() > 0)
		memcpy(vertices_, &vertices[0], vertices.size() * sizeof(float));//注意memcpy最后一个参数是字节数!!!
		std::shared_ptr<VertexBuffer> vertexBuffer(DBG_NEW VertexBuffer(vertices_, vertices.size() * sizeof(float)));
		vertexBuffer->SetBufferLayout(layout);

		unsigned int* indices_ = DBG_NEW unsigned int[indices.size()];
		memcpy(indices_, &indices[0], indices.size() * sizeof(unsigned int));
		std::shared_ptr<IndexBuffer> indexBuffer(DBG_NEW IndexBuffer(indices_, indices.size() * sizeof(unsigned int)));
		std::shared_ptr<Material> material(new Material(m_Shader, textures, colors));

		m_VerticesIdx += aimesh->mNumVertices;


		if (m_HasAnimation) {
			unsigned int* verticesIntjointIdx_ = DBG_NEW unsigned int[verticesIntjointIdx.size()];
			memcpy(verticesIntjointIdx_, &verticesIntjointIdx[0], verticesIntjointIdx.size() * sizeof(unsigned int));//注意memcpy最后一个参数是字节数!!!
			std::shared_ptr<VertexBuffer> vertexBuffer1(DBG_NEW VertexBuffer(verticesIntjointIdx_, verticesIntjointIdx.size() * sizeof(unsigned int)));
			vertexBuffer1->SetBufferLayout(layout1);

			float* verticesfloatWeight_ = DBG_NEW float[verticesfloatWeight.size()];
			memcpy(verticesfloatWeight_, &verticesfloatWeight[0], verticesfloatWeight.size() * sizeof(float));//注意memcpy最后一个参数是字节数!!!
			std::shared_ptr<VertexBuffer> vertexBuffer2(DBG_NEW VertexBuffer(verticesfloatWeight_, verticesfloatWeight.size() * sizeof(float)));
			vertexBuffer2->SetBufferLayout(layout2);
			return Mesh(material, indexBuffer, { vertexBuffer,vertexBuffer1,vertexBuffer2 });
		}




		/*	return Mesh(
				vertices_, vertices.size() * sizeof(float),
				indices_, indices.size() * sizeof(unsigned int),
				material,
				layout);*/

		return Mesh(material, indexBuffer, { vertexBuffer });


	}

	void Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, Texture::Type typeName, std::shared_ptr<Material::TextureMaps>& textures)
	{
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
		{
			std::shared_ptr<Texture> texture;
			aiString path;
			material->GetTexture(type, i, &path);

			std::string path_str = m_Directory + "/" + std::string(path.C_Str());//
			if (typeName == Texture::Type::DiffuseMap)
				textures->diffuseTextureMap.reset(DBG_NEW Texture(typeName, path_str.c_str()));
			if (typeName == Texture::Type::SpecularMap)
				textures->specularTextureMap.reset(DBG_NEW Texture(typeName, path_str.c_str()));
			if (typeName == Texture::Type::EmissionMap)
				textures->emissionTextureMap.reset(DBG_NEW Texture(typeName, path_str.c_str()));
			if (typeName == Texture::Type::NormalMap)
				textures->normalTextureMap.reset(DBG_NEW Texture(typeName, path_str.c_str()));
			/*if (typeName == Texture::Type::HeightMap)
				textures->heightTextureMap.reset(DBG_NEW Texture(typeName, path_str.c_str()));
			*/


		}

	}

	void Model::LoadMaterialColors(aiMaterial* material, MaterialColor& colors)
	{

		aiColor3D color(0.f, 0.f, 0.f);
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
			colors.SetDiffuseColor(glm::vec3(color.r, color.g, color.b));
		//colors.push_back(MaterialColor(MaterialColor::Type::DiffuseColor,));

		color = { 0.f, 0.f, 0.f };
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color))
			colors.SetAmbientColor(glm::vec3(color.r, color.g, color.b));

		//colors.push_back(MaterialColor(MaterialColor::Type::AmbientColor, glm::vec3(color.r, color.g, color.b)));

		color = { 0.f, 0.f, 0.f };

		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color))
			colors.SetSpecularColor(glm::vec3(color.r, color.g, color.b));

		color = { 0.f, 0.f, 0.f };

		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, color))
			colors.SetEmissionColor(glm::vec3(color.r, color.g, color.b));
		//colors.push_back(MaterialColor(MaterialColor::Type::SpecularColor, glm::vec3(color.r, color.g, color.b)));

	}

	std::vector<glm::mat4> Model::CalculateBoneTransform(float timeInSecond)
	{

		/*m_Scene = importer.ReadFile(m_Path,
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices);*/
		unsigned int animationNum = m_Scene->mNumAnimations;
		aiAnimation* animation = m_Scene->mAnimations[0];// &m_Animation;
		float durationTick = (float)animation->mDuration;
		float tickPerSecond = (float)animation->mTicksPerSecond;
		if (tickPerSecond == 0.0) {
			tickPerSecond = 25.0f;
		}
		float timeInTick = timeInSecond * tickPerSecond;
		//double durationSecond = durationTick / tickPerSecond;
		//double timeInDurationSecond = fmod(timeInSecond, durationSecond);
		float timeInDurationSecond = fmod(timeInTick, durationTick);
		aiMatrix4x4 indentity{
		1.0,0.0,0.0,0.0,
		0.0,1.0,0.0,0.0,
		0.0,0.0,1.0,0.0,
		0.0,0.0,0.0,1.0	
		};
		ReadHierarchy(timeInDurationSecond, m_Scene->mRootNode, indentity);

		std::vector<glm::mat4> boneFinalTransforms;
		for (int i = 0; i < m_BoneCount; i++)
		{
			boneFinalTransforms.push_back(m_Bones[i].finalTransform);
		}

		return boneFinalTransforms;

	}
	void Model::ReadHierarchy(float timeInDurationSecond, aiNode* node, aiMatrix4x4 parentTransform)
	{
		std::string nodeName(node->mName.data);
		aiAnimation* animation = m_Scene->mAnimations[0];
		aiNodeAnim* nodeAnim = FindNode(nodeName, animation);
		aiMatrix4x4 transform = (node->mTransformation);
		

		if (nodeAnim) {

			unsigned int positionNum = nodeAnim->mNumPositionKeys;
			unsigned int rotationNum = nodeAnim->mNumRotationKeys;
			unsigned int scaleNum = nodeAnim->mNumScalingKeys;
			glm::vec3 position = CalculateInterpolatePosition(timeInDurationSecond, nodeAnim);
			aiQuaternion quaternion = CalculateInterpolateRotation(timeInDurationSecond, nodeAnim);
			glm::vec3 scale = CalculateInterpolateScale(timeInDurationSecond, nodeAnim);


			
			aiMatrix3x3 rotateM = (quaternion.GetMatrix());
		//	rotateM = glm::transpose(rotateM);
			//glm::mat4 scaleM = glm::scale(glm::mat4(1.0f), scale);

			transform = aiMatrix4x4(rotateM);
			transform.a4 = position.x; transform.b4 = position.y; transform.c4 = position.z; transform.d4 = 1.0f;


		}
		aiMatrix4x4 globalTransform = parentTransform * transform  ;
		if (m_BoneNameToIdex.find(nodeName) != m_BoneNameToIdex.end()) {
			int boneIdex = m_BoneNameToIdex[nodeName];
			aiMatrix4x4 aiFinalTransform = m_GlobalInverseTransform * globalTransform * m_Bones[boneIdex].meshToBoneTranform;
			//m_Bones[boneIdex].finalTransform = m_GlobalInverseTransform * globalTransform * m_Bones[boneIdex].meshToBoneTranform;
			m_Bones[boneIdex].finalTransform = glm::transpose(AiMatrix4ToMat4(aiFinalTransform));
			glm::mat4 temp = glm::mat4(
				glm::vec4(aiFinalTransform.a1,aiFinalTransform.a2, aiFinalTransform.a3, aiFinalTransform.a4),
				glm::vec4(aiFinalTransform.b1, aiFinalTransform.b2, aiFinalTransform.b3, aiFinalTransform.b4),
				glm::vec4(aiFinalTransform.c1, aiFinalTransform.c2, aiFinalTransform.c3, aiFinalTransform.c4),
				glm::vec4(aiFinalTransform.d1, aiFinalTransform.d2, aiFinalTransform.d3, aiFinalTransform.d4)
			);
			glm::mat4 a(1.0f);
		}
		else {
			GE_CORE_ERROR("no such bone" + nodeName);
		}

		for (int i = 0; i < node->mNumChildren; i++)
		{
			ReadHierarchy(timeInDurationSecond, node->mChildren[i], globalTransform);
		}



	}

	glm::vec3 Model::CalculateInterpolatePosition(float timeInDurationSecond, aiNodeAnim* nodeAnim)
	{
		glm::vec3 pos;
		aiVectorKey currentKey;
		aiVectorKey nextKey;
		if (nodeAnim->mNumPositionKeys == 1)
			return glm::vec3(nodeAnim->mPositionKeys[0].mValue.x, nodeAnim->mPositionKeys[0].mValue.y, nodeAnim->mPositionKeys[0].mValue.z);
		for (int i = 0; i < nodeAnim->mNumPositionKeys; i++)
		{
			if (timeInDurationSecond >= nodeAnim->mPositionKeys[i].mTime) {
				currentKey = nodeAnim->mPositionKeys[i];
			}
			else {
				nextKey = nodeAnim->mPositionKeys[i];
				break;
			}
		}
		double durationOfTwoKey = nextKey.mTime - currentKey.mTime;
		double ratioToCurrentKey = (timeInDurationSecond - currentKey.mTime) / durationOfTwoKey;
		double ratioToNextKey = nextKey.mTime - timeInDurationSecond / durationOfTwoKey;
		pos = (float)ratioToCurrentKey * glm::vec3(currentKey.mValue.x, currentKey.mValue.y, currentKey.mValue.z)
			+ (float)ratioToNextKey * glm::vec3(nextKey.mValue.x, nextKey.mValue.y, nextKey.mValue.z);

		return pos;
	}
	aiQuaternion Model::CalculateInterpolateRotation(float timeInDurationSecond, aiNodeAnim* nodeAnim)
	{
		aiQuaternion out;
		aiQuatKey currentKey;
		aiQuatKey nextKey;
		if (nodeAnim->mNumPositionKeys == 1)
			return  nodeAnim->mRotationKeys[0].mValue;
		//for (int i = 0; i < nodeAnim->mNumPositionKeys; i++)
		//{
		//	if (timeInDurationSecond >= nodeAnim->mRotationKeys[i].mTime) {
		//		currentKey = nodeAnim->mRotationKeys[i];
		//	}
		//	else {
		//		nextKey = nodeAnim->mRotationKeys[i];
		//		break;
		//	}
		//}
		//double durationOfTwoKey = nextKey.mTime - currentKey.mTime;
		//double ratioToCurrentKey = (timeInDurationSecond - currentKey.mTime) / durationOfTwoKey;
		//double ratioToNextKey = nextKey.mTime - timeInDurationSecond / durationOfTwoKey;
		///*quaternion = (float)ratioToCurrentKey * glm::vec4(currentKey.mValue.x, currentKey.mValue.y, currentKey.mValue.z, currentKey.mValue.w);
		//	+ (float)ratioToNextKey * glm::vec4(nextKey.mValue.x, nextKey.mValue.y, nextKey.mValue.z,nextKey.mValue.w);*/
		//aiQuaternion::Interpolate(out, currentKey.mValue, nextKey.mValue, ratioToCurrentKey);
		//out = out.Normalize();

		//return out;

		// we need at least two values to interpolate...
		

		unsigned int RotationIndex = FindRotation(timeInDurationSecond, nodeAnim);
		unsigned int  NextRotationIndex = (RotationIndex + 1);
		GE_ASSERT(NextRotationIndex < nodeAnim->mNumRotationKeys,"NextRotationIndex >=pNodeAnim->mNumRotationKeys");
		float DeltaTime = nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime;
		float Factor = (timeInDurationSecond - (float)nodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
		out = out.Normalize();
		return out;
	}
	glm::vec3 Model::CalculateInterpolateScale(float timeInDurationSecond, aiNodeAnim* nodeAnim)
	{
		glm::vec3 scale;
		aiVectorKey currentKey;
		aiVectorKey nextKey;
		if (nodeAnim->mNumScalingKeys == 1)
			return glm::vec3(nodeAnim->mScalingKeys[0].mValue.x, nodeAnim->mScalingKeys[0].mValue.y, nodeAnim->mScalingKeys[0].mValue.z);
		for (int i = 0; i < nodeAnim->mNumScalingKeys; i++)
		{
			if (timeInDurationSecond >= nodeAnim->mScalingKeys[i].mTime) {
				currentKey = nodeAnim->mScalingKeys[i];
			}
			else {
				nextKey = nodeAnim->mScalingKeys[i];
				break;
			}
		}
		double durationOfTwoKey = nextKey.mTime - currentKey.mTime;
		double ratioToCurrentKey = (timeInDurationSecond - currentKey.mTime) / durationOfTwoKey;
		double ratioToNextKey = nextKey.mTime - timeInDurationSecond / durationOfTwoKey;
		scale = (float)ratioToCurrentKey * glm::vec3(currentKey.mValue.x, currentKey.mValue.y, currentKey.mValue.z)
			+ (float)ratioToNextKey * glm::vec3(nextKey.mValue.x, nextKey.mValue.y, nextKey.mValue.z);

		return scale;
	}



	aiNodeAnim* Model::FindNode(std::string nodeName, aiAnimation* animation)
	{
		for (int i = 0; i < animation->mNumChannels; i++)
		{
			aiNodeAnim* nodeAnim = animation->mChannels[i];
			if (std::string(nodeAnim->mNodeName.data) == nodeName) {
				return nodeAnim;
			}

		}
		return nullptr;
	}
	glm::mat4 Model::AiMatrix4ToMat4(aiMatrix4x4 aiMatrix)
	{
		glm::mat4 res(0.0);
		res[0][0] = aiMatrix.a1; res[0][1] = aiMatrix.a2; res[0][2] = aiMatrix.a3; res[0][3] = aiMatrix.a4;
		res[1][0] = aiMatrix.b1; res[1][1] = aiMatrix.b2; res[1][2] = aiMatrix.b3; res[1][3] = aiMatrix.b4;
		res[2][0] = aiMatrix.c1; res[2][1] = aiMatrix.c2; res[2][2] = aiMatrix.c3; res[2][3] = aiMatrix.c4;
		res[3][0] = aiMatrix.d1; res[3][1] = aiMatrix.d2; res[3][2] = aiMatrix.d3; res[3][3] = aiMatrix.d4;

		return res;
	}
	glm::mat4 Model::AiMatrix4ToMat4(aiMatrix3x3 aiMatrix)
	{
		glm::mat4 res(0.0);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				res[i][j] = aiMatrix[i][j];
			}

		}
		res[3][0] = 0.0f; res[3][1] = 0.0f; res[3][2] = 0.0f; res[3][3] = 1.0f;

		return res;
	}



	unsigned int Model::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumRotationKeys > 0);

		for (unsigned int  i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);
	}
}