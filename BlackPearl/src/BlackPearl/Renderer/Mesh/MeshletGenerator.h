#pragma once
#include "Mesh.h"

namespace BlackPearl {
    class MeshletGenerator
    {
    public:
        MeshletGenerator(/* args */);
        ~MeshletGenerator();
        void Process(const std::vector<std::shared_ptr<Mesh>>& meshes, const MeshletOption& options);
    
    private:
        /* data */
        void ComputeMeshlet(const std::shared_ptr<Mesh>& mesh);
        bool ExportMeshes(const char* filename, std::vector<ExportMesh>& exportMeshes);

    };
    
   
    
}