#include "pch.h"
#include "ModelLoader.h"
#include "BlackPearl/Renderer/Mesh/Vertex.h"
#include "BlackPearl/Renderer/Model/Model.h"
#include "BlackPearl/RHI/RHIBindingSet.h"
#include "BlackPearl/Common/CommonFunc.h"
namespace BlackPearl {


    
    ModelLoader::ModelLoader()
    {
    }

    ModelLoader::~ModelLoader()
    {
    }

    void ModelLoader::RegisterDeviceManager(DeviceManager* deviceManager)
    {
        m_DeviceManager = deviceManager;
    }


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

	static void glmInsertVector(glm::u32vec4 v, std::vector<uint32_t>& vec) {
		vec.push_back(v.x);
		vec.push_back(v.y);
		vec.push_back(v.z);
		vec.push_back(v.w);

	}
	static glm::mat4 ConvertMatrix(const aiMatrix4x4& aiMat)
	{
		return {
		aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
		aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
		aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
		aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
		};
	}

	glm::mat3 aiMatrix3x3ToGlm(const aiMatrix3x3& from)
	{
		glm::mat3 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2;	to[2][0] = from.a3;
		to[0][1] = from.b1; to[1][1] = from.b2;	to[2][1] = from.b3;
		to[0][2] = from.c1; to[1][2] = from.c2;	to[2][2] = from.c3;

		return to;
	}
	Model* ModelLoader::LoadModel(const std::string& path, const ModelDesc& desc)
	{

		Model* model = DBG_NEW Model(path,desc);

		m_CurrentModel = model;
		m_FirstVertex = true;
		m_Path = path;
		m_Scene = m_Importer.ReadFile(path,
			aiProcess_Triangulate |  //���������ι��ɵ�ģ��ת��Ϊ�������ι��ɵ�ģ��
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices);//

		//��Ҫ��aiProcess_FlipUVs����������������ᷴ������


		if (!m_Scene || m_Scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode) {
			GE_CORE_ERROR("ERROR::ASSIMP:: {0}", m_Importer.GetErrorString())
				return;
		}

		m_Directory = path.substr(0, path.find_last_of('/'));

		for (int i = 0; i < m_Scene->mNumMeshes; i++)
		{
			m_VerticesNum += m_Scene->mMeshes[i]->mNumVertices;
		}
		m_GlobalInverseTransform = glm::inverse(ConvertMatrix(m_Scene->mRootNode->mTransformation));


		m_BoneDatas.resize(m_VerticesNum);


		for (int m = 0; m < m_Scene->mNumMaterials; m++) {

			m_CurentModelMaterials[m] = LoadMaterial(m_Scene->mMaterials[m]);
		}

		ProcessNode(m_Scene->mRootNode, m_Scene);

		return model;
	}

	

	void ModelLoader::LoadBones(aiMesh* aimesh)
	{

		for (uint32_t i = 0; i < aimesh->mNumBones; i++)
		{
			uint32_t boneIdx;
			aiBone* bone = aimesh->mBones[i];
			std::string name(bone->mName.data);
			glm::mat4 meshToBoneTranform = ConvertMatrix(bone->mOffsetMatrix);

			if (m_BoneNameToIdex.find(name) != m_BoneNameToIdex.end()) {
				boneIdx = m_BoneNameToIdex[name];
				//m_Bones[boneIdx].meshToBoneTranform = (meshToBoneTranform);
			}
			else {
				m_BoneNameToIdex.insert(std::pair(name, m_BoneCount));
				boneIdx = m_BoneCount;
				m_Bones.push_back({ boneIdx,(meshToBoneTranform),glm::mat4(1.0) });
				m_BoneCount++;
			}

			uint32_t numberOfVertex = bone->mNumWeights;
			for (int i = 0; i < numberOfVertex; i++)
			{
				/*index of vertex affect by this bone*/
				uint32_t vertexIdx = bone->mWeights[i].mVertexId;
				/*weight of this bone to the vertex*/
				float boneWeight = bone->mWeights[i].mWeight;

				GE_ASSERT(m_BoneDatas[m_VerticesIdx + vertexIdx].currentPos < MAX_WEIGHT, "related bone's number larger than MAX_WEIGHT");
				if (m_BoneDatas[m_VerticesIdx + vertexIdx].currentPos < MAX_WEIGHT) {
					uint32_t row = m_BoneDatas[m_VerticesIdx + vertexIdx].currentPos / 4;
					uint32_t col = m_BoneDatas[m_VerticesIdx + vertexIdx].currentPos % 4;
					m_BoneDatas[m_VerticesIdx + vertexIdx].jointIdx[row][col] = boneIdx;
					m_BoneDatas[m_VerticesIdx + vertexIdx].weights[row][col] = boneWeight;
					m_BoneDatas[m_VerticesIdx + vertexIdx].currentPos++;
				}
			}
		}

	}
	std::shared_ptr<Material> ModelLoader::LoadMaterial(aiMaterial* material)
	{
		std::shared_ptr<Material> meshMaterial;


		std::shared_ptr<Material::TextureMaps> textures(DBG_NEW Material::TextureMaps());
		MaterialColor  colors;
		float shininess;
		material->Get(AI_MATKEY_SHININESS, shininess);

		LoadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::Type::DiffuseMap, textures);
		LoadMaterialTextures(material, aiTextureType_SPECULAR, Texture::Type::SpecularMap, textures);
		LoadMaterialTextures(material, aiTextureType_NORMALS, Texture::Type::NormalMap, textures);//
		LoadMaterialTextures(material, aiTextureType_HEIGHT, Texture::Type::HeightMap, textures);
		//	LoadMaterialTextures(material, aiTextureType_OPACITY, Texture::Type::OpacityMap, textures); //
		LoadMaterialTextures(material, aiTextureType_AMBIENT, Texture::Type::SpecularMap, textures); //


		//LoadMaterialTextures(material, aiTextureType_REFLECTION, Texture::Type::RoughnessMap, textures);
		LoadMaterialTextures(material, aiTextureType_LIGHTMAP, Texture::Type::AoMap, textures);
		LoadMaterialTextures(material, aiTextureType_EMISSIVE, Texture::Type::EmissionMap, textures);

		//LoadMaterialTextures(material, aiTextureType_DISPLACEMENT, Texture::Type::DiffuseMap, textures);//TODO

		LoadMaterialColors(material, colors);

		meshMaterial.reset(DBG_NEW Material(m_CurrentModel->shader, textures, colors));
		meshMaterial->SetShininess(shininess);
		return meshMaterial;
	}

	void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = m_Scene->mMeshes[i];

			m_CurrentModel->meshes.push_back(ProcessMesh(mesh, m_CurrentModel->vertices, m_CurrentModel->desc.bSortVerticces));
		}

		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			this->ProcessNode(node->mChildren[i], scene);
		}
	}

	//TODO:: Animation ModelLoader Vertexδ����
	std::shared_ptr<Mesh> ModelLoader::ProcessMesh(aiMesh* aimesh)
	{
		std::vector<float> vertexData;
		std::vector<uint32_t> indicesData;
		std::vector<uint32_t> jointIdData;
		std::vector<float> jointWeightData;

		bool hasAnimation = m_CurrentModel->desc.bIsAnimated;

		VertexBufferLayout layout1, layout2;
		VertexBufferLayout layout = {
			{ElementDataType::Float3,"aPos",false,0},
			{ElementDataType::Float3,"aNormal",false,1},
			{ElementDataType::Float2,"aTexCoords",false,2}
		};
		if (aimesh->HasTangentsAndBitangents()) {
			layout.AddElement({ ElementDataType::Float3,"aTangent",false,3 });
			layout.AddElement({ ElementDataType::Float3,"aBitangent",false,4 });
		}
		if (hasAnimation) {
			layout1 = { { ElementDataType::Int4,"aJointIndices",false,5 },
						{ ElementDataType::Int4,"aJointIndices1",false,6 },
						{ ElementDataType::Int4,"aJointIndices2",false,7 } };
			layout2 = { { ElementDataType::Float4,"aWeights",false,8},
						{ ElementDataType::Float4,"aWeights1",false,9},
						{ ElementDataType::Float4,"aWeights2",false,10} };
		}

		if (hasAnimation && aimesh->mNumBones >= 0)
			LoadBones(aimesh);

		for (uint32_t i = 0; i < aimesh->mNumVertices; i++)
		{
			Vertex vertex;

			glm::vec3 pos;
			pos.x = aimesh->mVertices[i].x;
			pos.y = aimesh->mVertices[i].y;
			pos.z = aimesh->mVertices[i].z;
			glmInsertVector(pos, vertexData);
			vertex.position = pos;
			UpdateAABB(pos);

			glm::vec3 normal;
			normal.x = aimesh->mNormals[i].x;
			normal.y = aimesh->mNormals[i].y;
			normal.z = aimesh->mNormals[i].z;
			glmInsertVector(normal, vertexData);
			vertex.normal = normal;

			glm::vec2 textCords = glm::vec2(0.0f, 0.0f);
			if (aimesh->mTextureCoords[0]) {//�ж϶����Ƿ��в�������

				textCords.x = aimesh->mTextureCoords[0][i].x;
				textCords.y = aimesh->mTextureCoords[0][i].y;
			}
			glmInsertVector(textCords, vertexData);
			vertex.texCoords = textCords;

			if (hasAnimation && m_BoneDatas.size() >= 0) {
				uint32_t vertexIdx = m_VerticesIdx + i;
				glmInsertVector(glm::u32vec4(m_BoneDatas[vertexIdx].jointIdx[0][0], m_BoneDatas[vertexIdx].jointIdx[0][1], m_BoneDatas[vertexIdx].jointIdx[0][2], m_BoneDatas[vertexIdx].jointIdx[0][3]), jointIdData);
				glmInsertVector(glm::u32vec4(m_BoneDatas[vertexIdx].jointIdx[1][0], m_BoneDatas[vertexIdx].jointIdx[1][1], m_BoneDatas[vertexIdx].jointIdx[1][2], m_BoneDatas[vertexIdx].jointIdx[1][3]), jointIdData);
				glmInsertVector(glm::u32vec4(m_BoneDatas[vertexIdx].jointIdx[2][0], m_BoneDatas[vertexIdx].jointIdx[2][1], m_BoneDatas[vertexIdx].jointIdx[2][2], m_BoneDatas[vertexIdx].jointIdx[2][3]), jointIdData);

				glmInsertVector(glm::vec4(m_BoneDatas[vertexIdx].weights[0][0], m_BoneDatas[vertexIdx].weights[0][1], m_BoneDatas[vertexIdx].weights[0][2], m_BoneDatas[vertexIdx].weights[0][3]), jointWeightData);
				glmInsertVector(glm::vec4(m_BoneDatas[vertexIdx].weights[1][0], m_BoneDatas[vertexIdx].weights[1][1], m_BoneDatas[vertexIdx].weights[1][2], m_BoneDatas[vertexIdx].weights[1][3]), jointWeightData);
				glmInsertVector(glm::vec4(m_BoneDatas[vertexIdx].weights[2][0], m_BoneDatas[vertexIdx].weights[2][1], m_BoneDatas[vertexIdx].weights[2][2], m_BoneDatas[vertexIdx].weights[2][3]), jointWeightData);

			}
			//tangent
			glm::vec3 tangent = glm::vec3(0.0f);
			glm::vec3 bitTangent = glm::vec3(0.0f);

			if (aimesh->HasTangentsAndBitangents()) {
				tangent.x = aimesh->mTangents[i].x;
				tangent.y = aimesh->mTangents[i].y;
				tangent.z = aimesh->mTangents[i].z;
				//bittangent			
				bitTangent.x = aimesh->mBitangents[i].x;
				bitTangent.y = aimesh->mBitangents[i].y;
				bitTangent.z = aimesh->mBitangents[i].z;
				glmInsertVector(tangent, vertexData);
				glmInsertVector(bitTangent, vertexData);
				vertex.tangent = tangent;
				vertex.bitTangent = bitTangent;
			}
			//v_vertex.push_back(vertex);
		}


		for (uint32_t i = 0; i < aimesh->mNumFaces; i++)
		{
			aiFace face = aimesh->mFaces[i];
			GE_ASSERT(face.mNumIndices == 3, "face.mNumIndices!=3");
			for (uint32_t j = 0; j < face.mNumIndices; j++)
			{
				indicesData.push_back(face.mIndices[j]);
			}
		}


		std::shared_ptr<VertexBuffer> vertexBuffer(DBG_NEW VertexBuffer(vertexData.data(), vertexData.size() * sizeof(float)));
		vertexBuffer->SetBufferLayout(layout);


		std::shared_ptr<IndexBuffer> indexBuffer(DBG_NEW IndexBuffer(indicesData.data(), indicesData.size() * sizeof(uint32_t)));

		m_VerticesIdx += aimesh->mNumVertices;


		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		auto buffers = mesh->buffers;
		BufferDesc indexBufferDesc;
		indexBufferDesc.isIndexBuffer = true;
		indexBufferDesc.byteSize = buffers->indexData.size() * sizeof(uint32_t);
		indexBufferDesc.debugName = "IndexBuffer";
		indexBufferDesc.canHaveTypedViews = true;
		indexBufferDesc.canHaveRawViews = true;
		indexBufferDesc.format = Format::R32_UINT;
		indexBufferDesc.isAccelStructBuildInput = false;

		buffers->indexBuffer = m_DeviceManager->GetDevice()->createBuffer(indexBufferDesc);
		buffers->indexData = indicesData;

		buffers->vertexBufferLayout = layout;

		BufferDesc vertexBufferDesc;
		vertexBufferDesc.isVertexBuffer = true;
		vertexBufferDesc.byteSize = vertexData.size() * sizeof(float);
		vertexBufferDesc.debugName = "VertexBuffer";
		vertexBufferDesc.canHaveTypedViews = true;
		vertexBufferDesc.canHaveRawViews = true;
		vertexBufferDesc.isAccelStructBuildInput = false;

		buffers->vertexBuffer = m_DeviceManager->GetDevice()->createBuffer(vertexBufferDesc);
		buffers->vertexData = vertexData;


		mesh->material = m_CurentModelMaterials[aimesh->mMaterialIndex];


		if (hasAnimation) {

			std::shared_ptr<VertexBuffer> vertexBuffer1(DBG_NEW VertexBuffer(jointIdData.data(), jointIdData.size() * sizeof(uint32_t)));
			vertexBuffer1->SetBufferLayout(layout1);

			std::shared_ptr<VertexBuffer> vertexBuffer2(DBG_NEW VertexBuffer(jointWeightData.data(), jointWeightData.size() * sizeof(float)));
			vertexBuffer2->SetBufferLayout(layout2);

			//TODO::
			buffers->jointIdData = jointIdData;
			buffers->jointWeightData = jointWeightData;

			return std::make_shared<Mesh>(m_CurentModelMaterials[aimesh->mMaterialIndex], indexBuffer, std::vector<std::shared_ptr<VertexBuffer>>{ vertexBuffer, vertexBuffer1, vertexBuffer2 });
		}

		//return std::make_shared<Mesh>(m_CurentModelMaterials[aimesh->mMaterialIndex], indexBuffer, std::vector<std::shared_ptr<VertexBuffer>>{ vertexBuffer });
		return mesh;
	}

	//v_vertex use for building BVH Node and Triangle Mesh, so vertices need to be sorted according to indices.
	std::shared_ptr<Mesh> ModelLoader::ProcessMesh(aiMesh* aimesh, std::vector<Vertex>& v_vertex, bool sort_vertices)
	{
		if (!sort_vertices) {
			return ProcessMesh(aimesh);
		}

		std::vector<float> vertexData;
		std::vector<uint32_t> indicesData;
		std::vector<uint32_t> jointIdData;
		std::vector<float> jointWeightData;

		bool hasAnimation = m_CurrentModel->desc.bIsAnimated;

		VertexBufferLayout layout1, layout2;
		VertexBufferLayout layout = {
			{ElementDataType::Float3,"aPos",false,0},
			{ElementDataType::Float3,"aNormal",false,1},
			{ElementDataType::Float2,"aTexCoords",false,2}
		};

		if (aimesh->HasTangentsAndBitangents()) {
			layout.AddElement({ ElementDataType::Float3,"aTangent",false,3 });
			layout.AddElement({ ElementDataType::Float3,"aBitangent",false,4 });
		}
		if (hasAnimation) {
			layout1 = { { ElementDataType::Int4,"aJointIndices",false,5 },
						{ ElementDataType::Int4,"aJointIndices1",false,6 },
						{ ElementDataType::Int4,"aJointIndices2",false,7 } };
			layout2 = { { ElementDataType::Float4,"aWeights",false,8},
						{ ElementDataType::Float4,"aWeights1",false,9},
						{ ElementDataType::Float4,"aWeights2",false,10} };
		}

		if (hasAnimation && aimesh->mNumBones >= 0)
			LoadBones(aimesh);

		for (size_t i = 0; i < aimesh->mNumFaces; i++)
		{
			aiFace face = aimesh->mFaces[i];

			GE_ASSERT(face.mNumIndices == 3, "face.mNumIndices!=3");


			for (size_t j = 0; j < face.mNumIndices; j++)
			{
				indicesData.push_back(face.mIndices[j]);
				size_t idx = face.mIndices[j];
				Vertex vertex;
				glm::vec3 pos;
				pos.x = aimesh->mVertices[idx].x;
				pos.y = aimesh->mVertices[idx].y;
				pos.z = aimesh->mVertices[idx].z;
				glmInsertVector(pos, vertexData);
				vertex.position = pos;
				UpdateAABB(pos);

				glm::vec3 normal;
				normal.x = aimesh->mNormals[idx].x;
				normal.y = aimesh->mNormals[idx].y;
				normal.z = aimesh->mNormals[idx].z;
				glmInsertVector(normal, vertexData);
				vertex.normal = normal;

				glm::vec2 textCords = glm::vec2(0.0f, 0.0f);
				if (aimesh->mTextureCoords[0]) {//�ж϶����Ƿ��в�������

					textCords.x = aimesh->mTextureCoords[0][idx].x;
					textCords.y = aimesh->mTextureCoords[0][idx].y;
				}
				glmInsertVector(textCords, vertexData);
				vertex.texCoords = textCords;

				if (hasAnimation && m_BoneDatas.size() >= 0) {
					uint32_t vertexIdx = m_VerticesIdx + idx;
					glmInsertVector(glm::u32vec4(m_BoneDatas[vertexIdx].jointIdx[0][0], m_BoneDatas[vertexIdx].jointIdx[0][1], m_BoneDatas[vertexIdx].jointIdx[0][2], m_BoneDatas[vertexIdx].jointIdx[0][3]), jointIdData);
					glmInsertVector(glm::u32vec4(m_BoneDatas[vertexIdx].jointIdx[1][0], m_BoneDatas[vertexIdx].jointIdx[1][1], m_BoneDatas[vertexIdx].jointIdx[1][2], m_BoneDatas[vertexIdx].jointIdx[1][3]), jointIdData);
					glmInsertVector(glm::u32vec4(m_BoneDatas[vertexIdx].jointIdx[2][0], m_BoneDatas[vertexIdx].jointIdx[2][1], m_BoneDatas[vertexIdx].jointIdx[2][2], m_BoneDatas[vertexIdx].jointIdx[2][3]), jointIdData);

					glmInsertVector(glm::vec4(m_BoneDatas[vertexIdx].weights[0][0], m_BoneDatas[vertexIdx].weights[0][1], m_BoneDatas[vertexIdx].weights[0][2], m_BoneDatas[vertexIdx].weights[0][3]), jointWeightData);
					glmInsertVector(glm::vec4(m_BoneDatas[vertexIdx].weights[1][0], m_BoneDatas[vertexIdx].weights[1][1], m_BoneDatas[vertexIdx].weights[1][2], m_BoneDatas[vertexIdx].weights[1][3]), jointWeightData);
					glmInsertVector(glm::vec4(m_BoneDatas[vertexIdx].weights[2][0], m_BoneDatas[vertexIdx].weights[2][1], m_BoneDatas[vertexIdx].weights[2][2], m_BoneDatas[vertexIdx].weights[2][3]), jointWeightData);

					//	GE_ASSERT(m_BoneDatas[vertexIdx].weights[0] + m_BoneDatas[vertexIdx].weights[1] + m_BoneDatas[vertexIdx].weights[2] + m_BoneDatas[vertexIdx].weights[3] == 1.0f, "total weight!=1");
				}
				//tangent
				glm::vec3 tangent = glm::vec3(0.0f);
				glm::vec3 bitTangent = glm::vec3(0.0f);

				if (aimesh->HasTangentsAndBitangents()) {
					tangent.x = aimesh->mTangents[idx].x;
					tangent.y = aimesh->mTangents[idx].y;
					tangent.z = aimesh->mTangents[idx].z;
					//bittangent

					bitTangent.x = aimesh->mBitangents[idx].x;
					bitTangent.y = aimesh->mBitangents[idx].y;
					bitTangent.z = aimesh->mBitangents[idx].z;
					glmInsertVector(tangent, vertexData);
					glmInsertVector(bitTangent, vertexData);
					vertex.tangent = tangent;
					vertex.bitTangent = bitTangent;
				}
				v_vertex.push_back(vertex);
			}
		}

		std::shared_ptr<VertexBuffer> vertexBuffer(DBG_NEW VertexBuffer(vertexData.data(), vertexData.size() * sizeof(float)));
		vertexBuffer->SetBufferLayout(layout);

		std::shared_ptr<IndexBuffer> indexBuffer(DBG_NEW IndexBuffer(indicesData.data(), indicesData.size() * sizeof(uint32_t)));

		m_VerticesIdx += aimesh->mNumVertices;


		if (hasAnimation) {

			std::shared_ptr<VertexBuffer> vertexBuffer1(DBG_NEW VertexBuffer(jointIdData.data(), jointIdData.size() * sizeof(uint32_t)));
			vertexBuffer1->SetBufferLayout(layout1);

			std::shared_ptr<VertexBuffer> vertexBuffer2(DBG_NEW VertexBuffer(jointWeightData.data(), jointWeightData.size() * sizeof(float)));
			vertexBuffer2->SetBufferLayout(layout2);
			return std::shared_ptr<Mesh>(DBG_NEW Mesh(m_CurentModelMaterials[aimesh->mMaterialIndex], indexBuffer, { vertexBuffer,vertexBuffer1,vertexBuffer2 }));

		}

		return std::shared_ptr<Mesh>(DBG_NEW Mesh(m_CurentModelMaterials[aimesh->mMaterialIndex], indexBuffer, { vertexBuffer }));


	}
	void ModelLoader::LoadMaterialTextures(aiMaterial* material, aiTextureType type, Texture::Type typeName, std::shared_ptr<Material::TextureMaps>& textures)
	{
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		for (uint32_t i = 0; i < material->GetTextureCount(type); i++)
		{
			std::shared_ptr<Texture> texture;
			aiString path;
			material->GetTexture(type, i, &path);

			std::string path_str = m_Directory + "/" + std::string(path.C_Str());//
			if (typeName == Texture::Type::DiffuseMap)
				textures->diffuseTextureMap.reset(DBG_NEW Texture(typeName, path_str.c_str(), GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGBA, GL_REPEAT, GL_UNSIGNED_BYTE, true));
			if (typeName == Texture::Type::SpecularMap)
				textures->specularTextureMap.reset(DBG_NEW Texture(typeName, path_str.c_str(), GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGBA, GL_REPEAT, GL_UNSIGNED_BYTE, true));
			if (typeName == Texture::Type::EmissionMap)
				textures->emissionTextureMap.reset(DBG_NEW Texture(typeName, path_str.c_str(), GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));
			if (typeName == Texture::Type::NormalMap)
				textures->normalTextureMap.reset(DBG_NEW Texture(typeName, path_str.c_str(), GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));
			if (typeName == Texture::Type::AoMap)
				textures->aoMap.reset(DBG_NEW Texture(typeName, path_str.c_str(), GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));
			if (typeName == Texture::Type::MentallicMap)
				textures->mentallicMap.reset(DBG_NEW Texture(typeName, path_str.c_str(), GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));
			if (typeName == Texture::Type::RoughnessMap)
				textures->roughnessMap.reset(DBG_NEW Texture(typeName, path_str.c_str(), GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));
			if (typeName == Texture::Type::HeightMap)
				textures->heightTextureMap.reset(DBG_NEW Texture(typeName, path_str.c_str(), GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RED, GL_REPEAT, GL_UNSIGNED_BYTE, true));
			if (typeName == Texture::Type::OpacityMap)
				textures->heightTextureMap.reset(DBG_NEW Texture(typeName, path_str.c_str(), GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));


		}

	}

	void ModelLoader::LoadMaterialColors(aiMaterial* material, MaterialColor& colors)
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

	std::vector<glm::mat4> ModelLoader::CalculateBoneTransform(float timeInSecond)
	{

		uint32_t animationNum = m_Scene->mNumAnimations;
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
		glm::mat4 indentity(1.0f);
		/*	{
			1.0,0.0,0.0,0.0,
			0.0,1.0,0.0,0.0,
			0.0,0.0,1.0,0.0,
			0.0,0.0,0.0,1.0
			};*/
		ReadHierarchy(timeInDurationSecond, m_Scene->mRootNode, indentity);

		std::vector<glm::mat4> boneFinalTransforms;
		for (int i = 0; i < m_BoneCount; i++)
		{
			boneFinalTransforms.push_back(m_Bones[i].finalTransform);
		}

		return boneFinalTransforms;

	}
	void ModelLoader::ReadHierarchy(float timeInDurationSecond, aiNode* node, glm::mat4 parentTransform)
	{
		std::string nodeName(node->mName.data);
		aiAnimation* animation = m_Scene->mAnimations[0];
		aiNodeAnim* nodeAnim = FindNode(nodeName, animation);
		glm::mat4 transform = ConvertMatrix(node->mTransformation);


		if (nodeAnim) {

			uint32_t positionNum = nodeAnim->mNumPositionKeys;
			uint32_t rotationNum = nodeAnim->mNumRotationKeys;
			uint32_t scaleNum = nodeAnim->mNumScalingKeys;
			glm::vec3 position = CalculateInterpolatePosition(timeInDurationSecond, nodeAnim);
			glm::mat4 translateM = glm::translate(glm::mat4(1.0f), position);

			aiQuaternion quaternion = CalculateInterpolateRotation(timeInDurationSecond, nodeAnim);
			glm::quat rotation(quaternion.w, quaternion.x, quaternion.y, quaternion.z);
			glm::mat4 rotateM = glm::toMat4(rotation);

			glm::vec3 scale = CalculateInterpolateScale(timeInDurationSecond, nodeAnim);
			glm::mat4 scaleM = glm::scale(glm::mat4(1.0f), scale);



			transform = translateM * rotateM * scaleM;


		}
		glm::mat4 globalTransform = parentTransform * transform;
		if (m_BoneNameToIdex.find(nodeName) != m_BoneNameToIdex.end()) {
			int boneIdex = m_BoneNameToIdex[nodeName];
			glm::mat4 aiFinalTransform = m_GlobalInverseTransform * globalTransform * m_Bones[boneIdex].meshToBoneTranform;
			m_Bones[boneIdex].finalTransform = aiFinalTransform;

		}
		else {
			//GE_CORE_WARN("no such bone" + nodeName);
		}

		for (int i = 0; i < node->mNumChildren; i++)
		{
			ReadHierarchy(timeInDurationSecond, node->mChildren[i], globalTransform);
		}



	}

	glm::vec3 ModelLoader::CalculateInterpolatePosition(float timeInDurationSecond, aiNodeAnim* nodeAnim)
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
		GE_ASSERT(ratioToCurrentKey >= 0.0f, "ratioToCurrentKey<0.0f!");

		double ratioToNextKey = (nextKey.mTime - timeInDurationSecond) / durationOfTwoKey;
		GE_ASSERT(ratioToNextKey >= 0.0f, "ratioToNextKey<0.0f!");

		pos = (float)ratioToNextKey * glm::vec3(currentKey.mValue.x, currentKey.mValue.y, currentKey.mValue.z)
			+ (float)ratioToCurrentKey * glm::vec3(nextKey.mValue.x, nextKey.mValue.y, nextKey.mValue.z);

		return pos;
	}
	aiQuaternion ModelLoader::CalculateInterpolateRotation(float timeInDurationSecond, aiNodeAnim* nodeAnim)
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


		uint32_t RotationIndex = FindRotation(timeInDurationSecond, nodeAnim);
		uint32_t  NextRotationIndex = (RotationIndex + 1);
		GE_ASSERT(NextRotationIndex < nodeAnim->mNumRotationKeys, "NextRotationIndex >=nodeAnim->mNumRotationKeys");
		float DeltaTime = nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime;
		float Factor = (timeInDurationSecond - (float)nodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		if (Factor < 0)Factor = 0.0f;

		GE_ASSERT(Factor >= 0.0f && Factor <= 1.0f, "Factor error!");
		const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
		out = out.Normalize();
		return out;
	}
	glm::vec3 ModelLoader::CalculateInterpolateScale(float timeInDurationSecond, aiNodeAnim* nodeAnim)
	{
		/*	aiVector3D scale;
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
			scale = (float)ratioToNextKey * currentKey.mValue
				+ (float)ratioToCurrentKey * nextKey.mValue;

			return glm::vec3(scale.x,scale.y,scale.z);*/
		aiVector3D out;
		if (nodeAnim->mNumScalingKeys == 1) {
			out = nodeAnim->mScalingKeys[0].mValue;
			return glm::vec3(out.x, out.y, out.z);;
		}

		uint32_t ScalingIndex = FindScaling(timeInDurationSecond, nodeAnim);
		uint32_t NextScalingIndex = (ScalingIndex + 1);
		assert(NextScalingIndex < nodeAnim->mNumScalingKeys);
		float DeltaTime = (float)(nodeAnim->mScalingKeys[NextScalingIndex].mTime - nodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (timeInDurationSecond - (float)nodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
		if (Factor < 0)Factor = 0.0f;

		GE_ASSERT(Factor >= 0.0f && Factor <= 1.0f, "Factor error!");
		const aiVector3D& Start = nodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = nodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		out = Start + Factor * Delta;

		return glm::vec3(out.x, out.y, out.z);
	}



	aiNodeAnim* ModelLoader::FindNode(std::string nodeName, aiAnimation* animation)
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
	glm::mat4 ModelLoader::AiMatrix4ToMat4(aiMatrix4x4 aiMatrix)
	{
		glm::mat4 res(0.0);
		res[0][0] = aiMatrix.a1; res[0][1] = aiMatrix.a2; res[0][2] = aiMatrix.a3; res[0][3] = aiMatrix.a4;
		res[1][0] = aiMatrix.b1; res[1][1] = aiMatrix.b2; res[1][2] = aiMatrix.b3; res[1][3] = aiMatrix.b4;
		res[2][0] = aiMatrix.c1; res[2][1] = aiMatrix.c2; res[2][2] = aiMatrix.c3; res[2][3] = aiMatrix.c4;
		res[3][0] = aiMatrix.d1; res[3][1] = aiMatrix.d2; res[3][2] = aiMatrix.d3; res[3][3] = aiMatrix.d4;

		return res;
	}
	glm::mat4 ModelLoader::AiMatrix4ToMat4(aiMatrix3x3 aiMatrix)
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



	uint32_t ModelLoader::FindRotation(float AnimationTime, const aiNodeAnim* nodeAnim)
	{
		assert(nodeAnim->mNumRotationKeys > 0);

		for (uint32_t i = 0; i < nodeAnim->mNumRotationKeys - 1; i++) {
			if (AnimationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);
	}

	uint32_t ModelLoader::FindScaling(float AnimationTime, const aiNodeAnim* nodeAnim)
	{
		assert(nodeAnim->mNumScalingKeys > 0);

		for (uint32_t i = 0; i < nodeAnim->mNumScalingKeys - 1; i++) {
			if (AnimationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);

		return 0;
	}

	void ModelLoader::UpdateAABB(const glm::vec3& pos)
	{
		glm::vec3 min;
		glm::vec3 max;
		if (m_FirstVertex) {
			m_FirstVertex = false;
			min = pos;
			max = pos;
			return;
		}
		min = m_CurrentModel->boundingbox->GetMinP();
		max = m_CurrentModel->boundingbox->GetMaxP();

		if (pos.x < min.x) {
			min.x = pos.x;
		}
		if (pos.y < min.y) {
			min.y = pos.y;
		}
		if (pos.z < min.z) {
			min.z = pos.z;
		}
		if (pos.x > max.x) {
			max.x = pos.x;
		}
		if (pos.y > max.y) {
			max.y = pos.y;
		}
		if (pos.z > max.z) {
			max.z = pos.z;
		}

		m_CurrentModel->boundingbox->SetP(min, max);
	}
   
}