#include "D3D12ModelLoader.h"
#include "BlackPearl/Renderer/Mesh/Meshlet.h"
#include "BlackPearl/Common/CommonFunc.h"
namespace BlackPearl {

    D3D12ModelLoader::D3D12ModelLoader(/* args */)
    {
    }
    
    D3D12ModelLoader::~D3D12ModelLoader()
    {
    }

    void D3D12ModelLoader::Load(const std::string& path) {
        auto mesh = std::make_unique<WaveFrontReader<uint16_t>>();
        const wchar_t* wPath = To_WString(path).c_str();
        ThrowIfFailed(mesh->Load(wPath), L"Load obj failed");

        size_t nFaces = mesh->indices.size() / 3;
        size_t nVerts = mesh->vertices.size();

        auto pos = std::make_unique<XMFLOAT3[]>(nVerts);
        for( size_t j = 0; j < nVerts; ++j )
           pos[j] = mesh->vertices[j].position;

        std::vector<Meshlet> meshlets;
        std::vector<uint8_t> uniqueVertexIB;
        std::vector<MeshletTriangle> primitiveIndices;
        ThrowIfFailed(ComputeMeshlets(mesh->indices.data(), nFaces,
            pos.get(), nVerts,
            nullptr,
            meshlets, uniqueVertexIB, primitiveIndices))
            // Error

        auto uniqueVertexIndices = reinterpret_cast<const uint16_t*>(uniqueVertexIB.data());
        size_t vertIndices = uniqueVertexIB.size() / sizeof(uint16_t);
    }
}