#pragma once
#include "pch.h"
#include "GameEngine/Renderer/Mesh.h"
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"

//#include <assimp/material.h>

//#include <assimp/cimport.h>
class Model
{
public:
	Model(const std::string& path) {
		LoadModel(path);
	
	};
	void Draw(const std::shared_ptr<Shader>& shader, const glm::mat4 & model, const LightSources& lightSources);

	~Model() = default;
	void LoadModel(const std::string& path);

	void ProcessNode(aiNode *node, const aiScene *scene);
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	void LoadMaterialTextures(
		aiMaterial * material,
		aiTextureType type,
		Texture::Type typeName,
		std::vector<std::shared_ptr<Texture >> &textures);

private:
	std:: vector<Mesh> m_Meshes;
	std::string m_Directory;
	
	//std::vector<Texture> m_Textures;//存储所有的已经load的Textures.

};

