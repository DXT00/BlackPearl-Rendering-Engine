
#include "pch.h"
#include "OpenGLModelLoader.h"

namespace BlackPearl {
	OpenGLModelLoader::OpenGLModelLoader(bool createMeshlet, const MeshletOption& options)
	{
	}

	OpenGLModelLoader::~OpenGLModelLoader()
	{
	}

	void OpenGLModelLoader::Load(std::vector<std::shared_ptr<Mesh>>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path)
	{
		if (m_CreateMeshlet) {
			LoadMeshletModel(output_meshes, bounding_sphere, path);
		}
		else {
			LoadCommonModel(output_meshes, path);
		}
	}

	void OpenGLModelLoader::LoadMeshletModel(std::vector<std::shared_ptr<Mesh>>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path)
	{
	}

	void OpenGLModelLoader::LoadCommonModel(std::vector<std::shared_ptr<Mesh>>& output_meshes, const std::string& path)
	{
	}

}
