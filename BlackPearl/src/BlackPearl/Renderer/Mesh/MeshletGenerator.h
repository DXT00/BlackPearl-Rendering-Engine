#include "Mesh.h"
#include "Meshlet.h"
#include "DirectXMesh.h"

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
        std::vector<Meshlet> m_Meshlets;
    };
    
   
    
}