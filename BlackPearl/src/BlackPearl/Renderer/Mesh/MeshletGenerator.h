#pragma once
#include "Mesh.h"

namespace BlackPearl {
    class MeshletGenerator
    {
    public:
        MeshletGenerator(/* args */);
        ~MeshletGenerator();
        void Process(const std::vector<Mesh>& meshes, const MeshletOption& options);
    
    private:
        /* data */
        void ComputeMeshlet(const Mesh& mesh);
        bool ExportMeshes(const char* filename, std::vector<ExportMesh>& exportMeshes);

    };
    
   
    
}