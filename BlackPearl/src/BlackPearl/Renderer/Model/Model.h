#pragma once
#include "pch.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Renderer/Material/MaterialColor.h"
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"

//#include <assimp/material.h>

//#include <assimp/cimport.h>
namespace BlackPearl {

	class Model
	{
	public:
		Model(const std::string& path,const std::shared_ptr<Shader>shader)
		:m_Shader(shader){
			LoadModel(path);

		};
		//void Draw(const glm::mat4 & model, const LightSources& lightSources);

		~Model() = default;
		void LoadModel(const std::string& path);

		void ProcessNode(aiNode *node, const aiScene *scene);
		Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
		void LoadMaterialTextures(
			aiMaterial * material,
			aiTextureType type,
			Texture::Type typeName,
			std::shared_ptr<Material::TextureMaps> &textures);

		void LoadMaterialColors(
			aiMaterial * material,
			 MaterialColor &colors);

		inline std::vector<Mesh> GetMeshes()const { return m_Meshes; }
		std::shared_ptr<Shader> GetShader()const { return m_Shader; }

	private:
		std::shared_ptr<Shader> m_Shader;//多个Mesh使用同一个shader
		std::vector<Mesh> m_Meshes;

		std::string m_Directory;

		//std::vector<Texture> m_Textures;//存储所有的已经load的Textures.

	};

}