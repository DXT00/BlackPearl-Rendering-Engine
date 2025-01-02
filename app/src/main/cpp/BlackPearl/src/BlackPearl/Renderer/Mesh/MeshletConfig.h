#pragma once
#include "pch.h"
#include "Meshlet.h"
#include <DirectXMath.h>
using namespace DirectX;
namespace BlackPearl {
    const uint32_t c_prolog = 'MSHL';

    struct Attribute
    {
        enum EType : uint32_t
        {
            Position,
            Normal,
            TexCoord,
            Tangent,
            Bitangent,
            Count
        };

        EType    Type;
        uint32_t Offset;
    };
    //struct MeshletInfo
    //{
    //    uint32_t IndexSize;
    //    uint32_t MeshletCount;

    //    uint32_t LastMeshletVertCount;
    //    uint32_t LastMeshletPrimCount;
    //};

    enum FileVersion
    {
        FILE_VERSION_INITIAL = 0,
        CURRENT_FILE_VERSION = FILE_VERSION_INITIAL
    };

    struct FileHeader
    {
        uint32_t Prolog;
        uint32_t Version;

        uint32_t MeshCount;
        uint32_t AccessorCount;
        uint32_t BufferViewCount;
        uint32_t BufferSize;
    };

    struct Subset //  a subset is consist of multiple meshlet
    {
        uint32_t Offset; //the start index of meshlet in this subset
        uint32_t Count; //meshlet count in a subset
    };

    struct MeshHeader
    {
        uint32_t Indices;
        uint32_t IndexSubsets;
        uint32_t Attributes[Attribute::Count];

        uint32_t Meshlets;
        uint32_t MeshletSubsets;
        uint32_t UniqueVertexIndices;
        uint32_t PrimitiveIndices;
        uint32_t CullData;
    };

    struct BufferView
    {
        uint32_t Offset;
        uint32_t Size;
    };

    struct Accessor
    {
        uint32_t BufferView;
        uint32_t Offset;
        uint32_t Size;
        uint32_t Stride;
        uint32_t Count;
    };
    struct PackedTriangle
    {
        uint32_t i0 : 10;
        uint32_t i1 : 10;
        uint32_t i2 : 10;
    };

    struct CullData
    {
        DirectX::XMFLOAT4 BoundingSphere; // xyz = center, w = radius
        uint8_t           NormalCone[4];  // xyz = axis, w = -cos(a + 90)
        float             ApexOffset;     // apex = center - axis * offset
    };


    struct ExportMesh
    {
        using DataStream = std::vector<uint8_t>;
        struct StreamDesc
        {
            std::vector<Attribute> Attributes;
            uint32_t Stride;
        };

        std::vector<StreamDesc>         Layout;
        std::vector<DataStream>         Vertices;
        uint32_t                        VertexCount;

        std::vector<Subset>             IndexSubsets;
        DataStream                      Indices;
        uint32_t                        IndexSize;
        uint32_t                        IndexCount;

        std::vector<Subset>             MeshletSubsets;
        std::vector<Meshlet>            Meshlets;
        std::vector<uint8_t>            UniqueVertexIndices;
        std::vector<PackedTriangle>     PrimitiveIndices;
        std::vector<CullData>           CullData;
    };
}