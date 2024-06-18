#pragma once
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/DeviceManager.h"

#include "assimp/Importer.hpp"	
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "BlackPearl/Animation/Bone.h"
namespace BlackPearl
{
    class ModelLoader
    {
    protected:
        DeviceManager* m_DeviceManager;
        /* data */
    public:
        ModelLoader(/* args */);

        virtual ~ModelLoader();

        virtual void RegisterDeviceManager(DeviceManager* deviceManager);
        virtual void Load(std::vector<std::shared_ptr<Mesh>>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path) {};
		Model* LoadModel(const std::string& path, const ModelDesc& desc);

    
    private:
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
    
		uint32_t FindRotation(float AnimationTime, const aiNodeAnim* nodeAnim);
		uint32_t FindScaling(float AnimationTime, const aiNodeAnim* nodeAnim);
    
		void UpdateAABB(const glm::vec3& pos);
		std::string m_Directory;
		Assimp::Importer m_Importer;

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


		/* Material */
		std::map<int, std::shared_ptr<Material>> m_CurentModelMaterials;


		Model* m_CurrentModel;

		bool m_FirstVertex = true;
    };
    

    
} // namespace BlackPearl
