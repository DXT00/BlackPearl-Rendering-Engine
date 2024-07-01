#include "pch.h"
#include "D3D12ModelLoader.h"
#include "BlackPearl/Common/CommonFunc.h"
#include "WaveFrontReader.h"
#include "DirectXMesh.h"
#include "BlackPearl/Renderer/Mesh/MeshletConfig.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12VertexBuffer.h"
using namespace DirectX;
namespace BlackPearl {
    
    const D3D12_INPUT_ELEMENT_DESC c_elementDescs[Attribute::Count] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
        { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 1 },
    };

    const uint32_t c_sizeMap[] =
    {
        12, // Position
        12, // Normal
        8,  // TexCoord
        12, // Tangent
        12, // Bitangent
    };

    D3D12ModelLoader::D3D12ModelLoader()
    {
        //m_IsMeshletModel = isMeshletModel;

    }
    
    D3D12ModelLoader::~D3D12ModelLoader()
    {
    }

    Model* D3D12ModelLoader::LoadModel(const std::string& path, const ModelDesc& desc)
    {
        m_IsMeshletModel = desc.bIsMeshletModel;
        Model* model = DBG_NEW Model(path, desc);
        std::vector<std::shared_ptr<Mesh>> meshes;
        if (desc.bCreateMeshlet && !desc.bIsMeshletModel) {
            m_MeshletGenerator = std::make_shared<MeshletGenerator>();
            m_MeshletGenerator->Process(meshes, desc.options);
        }
        //m_ModelLoader = DBG_NEW D3D12ModelLoader(desc.bIsMeshletModel);
        Load(meshes, m_BoundingSphere, path);
        model->meshes = meshes;
        return model;
    }

    void D3D12ModelLoader::Load(std::vector<std::shared_ptr<Mesh>>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path) {

        if (m_IsMeshletModel) {
            LoadMeshletModel(output_meshes, bounding_sphere, path);
        }
        else {
            LoadCommonModel(path);
        }
        //auto mesh = std::make_unique<WaveFrontReader<uint16_t>>();
        //const wchar_t* wPath = To_WString(path.c_str()).c_str();
        //ThrowIfFailed(mesh->Load(wPath), L"Load obj failed");

        //size_t nFaces = mesh->indices.size() / 3;
        //size_t nVerts = mesh->vertices.size();

        //auto pos = std::make_unique<XMFLOAT3[]>(nVerts);
        //for( size_t j = 0; j < nVerts; ++j )
        //   pos[j] = mesh->vertices[j].position;

        //std::vector<Meshlet> meshlets;
        //std::vector<uint8_t> uniqueVertexIB;
        //std::vector<MeshletTriangle> primitiveIndices;
        //ThrowIfFailed(DirectX::ComputeMeshlets(mesh->indices.data(), nFaces,
        //    pos.get(), nVerts,
        //    nullptr,
        //    meshlets, uniqueVertexIB, primitiveIndices,
        //    m_MeshletOptions.Verts, m_MeshletOptions.Prims));

        //auto uniqueVertexIndices = reinterpret_cast<const uint16_t*>(uniqueVertexIB.data());
        //size_t vertIndices = uniqueVertexIB.size() / sizeof(uint16_t);
    }


    //this fuction reference from microsoft/DirectX-Graphics-Samples MeshShader example
    // deserialize meshlet model
    // serialize mesh model method is in MeshletGenerator.cpp : MeshletGenerator::ExportMeshes
    void D3D12ModelLoader::LoadMeshletModel(std::vector<std::shared_ptr<Mesh>>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.is_open()) {
            GE_CORE_ERROR("fail to load model" + path);
            return;
        }
        std::vector<MeshHeader> meshes;
        std::vector<BufferView> bufferViews;
        std::vector<Accessor> accessors; // accessors store the indices of all meshes
        FileHeader header;
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (header.Prolog != c_prolog)
        {
            GE_CORE_ERROR("Incorrect file format.");
            return;
        }

        if (header.Version != CURRENT_FILE_VERSION)
        {
            GE_CORE_ERROR(" Version mismatch between export and import serialization code.");
            return ; 
        }

        // Read mesh metdata
        meshes.resize(header.MeshCount);
        stream.read(reinterpret_cast<char*>(meshes.data()), meshes.size() * sizeof(meshes[0]));

        accessors.resize(header.AccessorCount);
        stream.read(reinterpret_cast<char*>(accessors.data()), accessors.size() * sizeof(accessors[0]));

        bufferViews.resize(header.BufferViewCount);
        stream.read(reinterpret_cast<char*>(bufferViews.data()), bufferViews.size() * sizeof(bufferViews[0]));

        // buffer store all indices in this mesh
        m_Buffer.resize(header.BufferSize);
        stream.read(reinterpret_cast<char*>(m_Buffer.data()), header.BufferSize);

        char eofbyte;
        stream.read(&eofbyte, 1); // Read last byte to hit the eof bit

        assert(stream.eof()); // There's a problem if we didn't completely consume the file contents.

        stream.close();

        // Populate mesh data from binary data and metadata.
        output_meshes.resize(meshes.size());
        for (uint32_t i = 0; i < static_cast<uint32_t>(meshes.size()); ++i)
        {
            auto& meshView = meshes[i];
            auto& mesh = output_meshes[i];
            uint32_t indexSize; // sizeof(index)
            uint32_t indexCount; //number of indices in this mesh
            uint8_t* indices;
            Span<Subset> indexSubsets;
            VertexBufferLayout layouts = {
                {ElementDataType::Float3,"POSITION", false, 0},
                {ElementDataType::Float3,"NORMAL", false, 0},
                {ElementDataType::Float2,"TEXCOORD", false, 0},
                {ElementDataType::Float3,"TANGENT", false, 0},
                {ElementDataType::Float3,"BITANGENT", false, 0}
            };
            //std::vector<uint32_t> vertexStrides;
            //std::vector<Span<uint8_t>> vertices; //每个attribute对应一个Span<uint_8> ?
            //uint32_t vertexCount;
            //Span<Subset>               meshletSubsets;
            //Span<Meshlet>              meshlets;
            //Span<uint8_t>              uniqueVertexIndices;
            //Span<PackedTriangle>       primitiveIndices;
            //Span<CullData>             cullingData;

            // Index data
            {
                Accessor& accessor = accessors[meshView.Indices];
                BufferView& bufferView = bufferViews[accessor.BufferView];

                mesh->IndexSize_ml = accessor.Size;
               // mesh.IndexCount = accessor.Count;

                mesh->Indices_ml = MakeSpan(m_Buffer.data() + bufferView.Offset, bufferView.Size);
            }

            // Index Subset data
            {
                Accessor& accessor = accessors[meshView.IndexSubsets];
                BufferView& bufferView = bufferViews[accessor.BufferView];

                mesh->IndexSubsets = MakeSpan(reinterpret_cast<Subset*>(m_Buffer.data() + bufferView.Offset), accessor.Count);
            }

            // Vertex data & layout metadata

            // Determine the number of unique Buffer Views associated with the vertex attributes & copy vertex buffers.
            std::vector<uint32_t> vbMap;
            uint32_t numElements = 0;
           
            // vertices 存储结构
            // v0.pos,v0.normal | v1.pos,v1.normal
            for (uint32_t j = 0; j < Attribute::Count; ++j)
            {
                if (meshView.Attributes[j] == -1)
                    continue;

                Accessor& accessor = accessors[meshView.Attributes[j]];

                auto it = std::find(vbMap.begin(), vbMap.end(), accessor.BufferView);
                if (it != vbMap.end())
                {
                    continue; // Already added - continue.
                }

                // New m_Buffer view encountered; add to list and copy vertex data
                vbMap.push_back(accessor.BufferView);
                BufferView& bufferView = bufferViews[accessor.BufferView];

                Span<uint8_t> verts = MakeSpan(m_Buffer.data() + bufferView.Offset, bufferView.Size);

                mesh->VertexStrides.push_back(accessor.Stride);
                mesh->Vertices_ml.push_back(verts); //这里和openGL不一样，每个attributes分开push到vertices
                mesh->VertexCount_ml = static_cast<uint32_t>(verts.size()) / accessor.Stride;
            }

            // Populate the vertex buffer metadata from accessors.
            VertexBufferLayout mesh_layout;
            for (uint32_t j = 0; j < Attribute::Count; ++j)
            {
                if (meshView.Attributes[j] == -1)
                    continue;

                Accessor& accessor = accessors[meshView.Attributes[j]];

                // Determine which vertex buffer index holds this attribute's data
                auto it = std::find(vbMap.begin(), vbMap.end(), accessor.BufferView);

                auto& element = layouts.GetElement(j);
                element.Location = static_cast<uint32_t>(std::distance(vbMap.begin(), it));

                mesh_layout.AddElement(element);
            }
            mesh->SetVertexBufferLayout(mesh_layout);


            // Meshlet data
            {
                Accessor& accessor = accessors[meshView.Meshlets];
                BufferView& bufferView = bufferViews[accessor.BufferView];

                mesh->Meshlets = MakeSpan(reinterpret_cast<Meshlet*>(m_Buffer.data() + bufferView.Offset), accessor.Count);
            }

            // Meshlet Subset data
            {
                Accessor& accessor = accessors[meshView.MeshletSubsets];
                BufferView& bufferView = bufferViews[accessor.BufferView];

                mesh->MeshletSubsets = MakeSpan(reinterpret_cast<Subset*>(m_Buffer.data() + bufferView.Offset), accessor.Count);
            }

            // Unique Vertex Index data , UniqueVertexIndices is the indices buffer that is generated by sorting each triangular face 
            // the method to create UniqueVertexIndices can refer to https://github.com/microsoft/DirectXMesh/wiki/OptimizeFacesLRU
            //https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/Desktop/D3D12MeshShaders/src/WavefrontConverter/MeshProcessor.cpp : MeshProcessor::Finalize
            {
                Accessor& accessor = accessors[meshView.UniqueVertexIndices];
                BufferView& bufferView = bufferViews[accessor.BufferView];

                mesh->UniqueVertexIndices = MakeSpan(m_Buffer.data() + bufferView.Offset, bufferView.Size);
            }

            // Primitive Index data
            {
                Accessor& accessor = accessors[meshView.PrimitiveIndices];
                BufferView& bufferView = bufferViews[accessor.BufferView];

                mesh->PrimitiveIndices = MakeSpan(reinterpret_cast<PackedTriangle*>(m_Buffer.data() + bufferView.Offset), accessor.Count);
            }

            // Cull data
            {
                Accessor& accessor = accessors[meshView.CullData];
                BufferView& bufferView = bufferViews[accessor.BufferView];

                mesh->CullingData = MakeSpan(reinterpret_cast<CullData*>(m_Buffer.data() + bufferView.Offset), accessor.Count);
            }
        }

        // Build bounding spheres for each mesh
        for (uint32_t i = 0; i < static_cast<uint32_t>(output_meshes.size()); ++i)
        {
            auto& m = output_meshes[i];

            uint32_t vbIndexPos = 0;

            // Find the index of the vertex buffer of the position attribute
            for (uint32_t j = 1; j < m->GetVertexBufferLayout().ElementSize(); ++j)
            {
                //auto& desc = m->LayoutElems[j];
                auto& element = m->GetVertexBufferLayout().GetElement(j);
                if (strcmp(element.Name.c_str(), "POSITION") == 0)
                {
                    vbIndexPos = j;
                    break;
                }
            }

            // Find the byte offset of the position attribute with its vertex buffer
            uint32_t positionOffset = 0;

            for (uint32_t j = 0; j < m->GetVertexBufferLayout().ElementSize(); ++j)
            {
                auto& element =  m->GetVertexBufferLayout().GetElement(j);
                if (strcmp(element.Name.c_str(), "POSITION") == 0)
                {
                    break;
                }
                //找到position的下一个attribute的位置
                if (element.Location == vbIndexPos)
                {
                    positionOffset += m->GetVertexBufferLayout().GetElement(j).GetElementCount();
                }
            }
            // vertices 存储结构
            // NORMAL  : v0.normal, v1.normal, v2.normal ...
            // POSITION: v0.pos, v1.pos, v2.pos .....
            //vbIndexPos = 1
            
            XMFLOAT3* v0 = reinterpret_cast<XMFLOAT3*>(m->Vertices_ml[vbIndexPos].data() + positionOffset);
            uint32_t stride = m->VertexStrides[vbIndexPos];

            BoundingSphere::CreateFromPoints(m->BoundingSphere, m->VertexCount_ml, v0, stride);

            if (i == 0)
            {
                bounding_sphere = m->BoundingSphere;
            }
            else
            {
                BoundingSphere::CreateMerged(bounding_sphere, bounding_sphere, m->BoundingSphere);
            }
        }

        return ;
    }
    void D3D12ModelLoader::LoadCommonModel(const std::string& path)
    {
    }
}