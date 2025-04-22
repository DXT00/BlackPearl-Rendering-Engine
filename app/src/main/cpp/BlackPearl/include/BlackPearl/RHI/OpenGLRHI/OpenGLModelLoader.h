#include "BlackPearl/Renderer/Model/ModelLoader.h"
#include "BlackPearl/Renderer/Mesh/Meshlet.h"
#include "BlackPearl/Renderer/Mesh/MeshletGenerator.h"
//TODO::把 Model.cpp中的 assimp 模型加载移到这里
namespace BlackPearl {
	class OpenGLModelLoader : public ModelLoader
	{
	public:
		OpenGLModelLoader(bool createMeshlet, const MeshletOption& options);
		~OpenGLModelLoader();
		virtual void Load(std::vector<std::shared_ptr<Mesh>>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path);
		void LoadMeshletModel(std::vector<std::shared_ptr<Mesh>>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path);
		void LoadCommonModel(std::vector<std::shared_ptr<Mesh>>& output_meshes, const std::string& path);

	private:
		/*for mesh shader renderer, to create meshlet*/
		bool m_CreateMeshlet = false;
		std::shared_ptr<MeshletGenerator> m_MeshletGenerator;
		MeshletOption m_MeshletOptions;
	};

}