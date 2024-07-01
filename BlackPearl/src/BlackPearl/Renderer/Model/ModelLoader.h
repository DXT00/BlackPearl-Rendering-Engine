#pragma once
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/Math/vector.h"
#include "glm/glm.hpp"
#include "assimp/Importer.hpp"	
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "BlackPearl/Animation/Bone.h"
#include "BlackPearl/Renderer/Model/Model.h"

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
		virtual Model* LoadModel(const std::string& path, const ModelDesc& desc);

		std::vector<glm::mat4>  CalculateBoneTransform(float timeInSecond, ModelDesc& desc);

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
		void ReadHierarchy(float timeInDurationSecond, aiNode* node, glm::mat4 parentTransform,  ModelDesc& desc);

		aiNodeAnim* FindNode(std::string nodeName, aiAnimation* animation);
		glm::mat4 AiMatrix4ToMat4(aiMatrix4x4 aiMatrix);
		glm::mat4 AiMatrix4ToMat4(aiMatrix3x3 aiMatrix);

		/*Interpolate*/
		math::float3    CalculateInterpolatePosition(float timeInDurationSecond, aiNodeAnim* nodeAnim);
		aiQuaternion CalculateInterpolateRotation(float timeInDurationSecond, aiNodeAnim* nodeAnim);
		math::float3    CalculateInterpolateScale(float timeInDurationSecond, aiNodeAnim* nodeAnim);
    
		uint32_t FindRotation(float AnimationTime, const aiNodeAnim* nodeAnim);
		uint32_t FindScaling(float AnimationTime, const aiNodeAnim* nodeAnim);
    
		void UpdateAABB(const math::float3& pos);

	private:
		void CreateMeshBuffers(std::shared_ptr<Mesh>& mesh);
		void LoadAnimationInfo();

	private:
		std::string m_Directory;
		Assimp::Importer m_Importer;


		
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
