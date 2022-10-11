#include "MeshletGenerator.h"

namespace BlackPearl
{
    MeshletGenerator::MeshletGenerator(/* args */)
    {
    }
    
    MeshletGenerator::~MeshletGenerator()
    {
    }
    
    void MeshletGenerator::Process(const std::vector<Mesh>& meshes, const MeshletOption& options) {
        for (size_t i = 0; i < meshes.size(); i++)
        {
            ComputeMeshlet(meshes[i]);
        }
        
    }

    void MeshletGenerator::ComputeMeshlet(const Mesh& mesh) {

    }


} // namespace BlackPearl
