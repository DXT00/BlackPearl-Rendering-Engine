#pragma once
/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#pragma once

//#include <donut/core/math/math.h>
//#include <donut/engine/DescriptorTableManager.h>
//#include <donut/shaders/light_types.h>
//#include <nvrhi/nvrhi.h>
#include <memory>
#include "BlackPearl/Math/vector.h"
#include "BlackPearl/Math/matrix.h"
#include "BlackPearl/Math/box.h"
#include "BlackPearl/Math/frustum.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/Renderer/DescriptorTableManager.h"
#include "BlackPearl/Renderer/Buffer/Buffer.h"
struct MaterialConstants;
struct LightConstants;
struct LightProbeConstants;

namespace Json
{
    class Value;
}

namespace BlackPearl
{

    struct InstanceData
    {
        math::uint padding;
        math::uint firstGeometryInstanceIndex; // index into global list of geometry instances. 
        // foreach (Instance)
        //     foreach(Geo) index++
        math::uint firstGeometryIndex;         // index into global list of geometries. 
        // foreach(Mesh)
        //     foreach(Geo) index++
        math::uint numGeometries;

        math::float3x4 transform;
        math::float3x4 prevTransform;
    };


    enum class TextureAlphaMode
    {
        UNKNOWN = 0,
        STRAIGHT = 1,
        PREMULTIPLIED = 2,
        OPAQUE_ = 3,
        CUSTOM = 4,
    };

    struct LoadedTexture
    {
        TextureHandle texture;
        TextureAlphaMode alphaMode = TextureAlphaMode::UNKNOWN;
        uint32_t originalBitsPerPixel = 0;
        DescriptorHandle bindlessDescriptor;
        std::string path;
        std::string mimeType;
    };

    enum class VertexAttribute
    {
        Position,
        PrevPosition,
        TexCoord1,
        TexCoord2,
        Normal,
        Tangent,
        Transform,
        PrevTransform,
        JointIndices,
        JointWeights,

        Count
    };

    VertexAttributeDesc GetVertexAttributeDesc(VertexAttribute attribute, const char* name, uint32_t bufferIndex);


    struct SceneLoadingStats
    {
        std::atomic<uint32_t> ObjectsTotal;
        std::atomic<uint32_t> ObjectsLoaded;
    };

    // NOTE regarding MaterialDomain and transparency. It may seem that the Transparent attribute
    // is orthogonal to the blending mode (opaque, alpha-tested, alpha-blended). In glTF, it is
    // indeed an independent extension, KHR_materials_transmission, that can interact with the
    // blending mode. But enabling physical transmission on an object is an important change
    // for renderers: for example, rasterizers need to render "opaque" transmissive objects in a
    // separate render pass, together with alpha bleneded materials; ray tracers also need to
    // process transmissive objects in a different way from regular opaque or alpha-tested objects.
    // Specifying the transmission option in the material domain makes these requirements explicit.

    enum class MaterialDomain : uint8_t
    {
        Opaque,
        AlphaTested,
        AlphaBlended,
        Transmissive,
        TransmissiveAlphaTested,
        TransmissiveAlphaBlended,

        Count
    };

    const char* MaterialDomainToString(MaterialDomain domain);

    //struct EMaterial
    //{
    //    std::string name;
    //    MaterialDomain domain = MaterialDomain::Opaque;
    //    std::shared_ptr<LoadedTexture> baseOrDiffuseTexture; // metal-rough: base color; spec-gloss: diffuse color; .a = opacity (both modes)
    //    std::shared_ptr<LoadedTexture> metalRoughOrSpecularTexture; // metal-rough: ORM map; spec-gloss: specular color, .a = glossiness
    //    std::shared_ptr<LoadedTexture> normalTexture;
    //    std::shared_ptr<LoadedTexture> emissiveTexture;
    //    std::shared_ptr<LoadedTexture> occlusionTexture;
    //    std::shared_ptr<LoadedTexture> transmissionTexture; // see KHR_materials_transmission; undefined on specular-gloss materials
    //    // std::shared_ptr<LoadedTexture> thicknessTexture; // see KHR_materials_volume (not implemented yet)
    //    BufferHandle materialConstants;
    //    math::float3 baseOrDiffuseColor = 1.f; // metal-rough: base color, spec-gloss: diffuse color (if no texture present)
    //    math::float3 specularColor = 0.f; // spec-gloss: specular color
    //    math::float3 emissiveColor = 0.f;
    //    float emissiveIntensity = 1.f; // additional multiplier for emissiveColor
    //    float metalness = 0.f; // metal-rough only
    //    float roughness = 0.f; // both metal-rough and spec-gloss
    //    float opacity = 1.f; // for transparent materials; multiplied by diffuse.a if present
    //    float alphaCutoff = 0.5f; // for alpha tested materials
    //    float transmissionFactor = 0.f; // see KHR_materials_transmission; undefined on specular-gloss materials
    //    float diffuseTransmissionFactor = 0.f; // like specularTransmissionFactor, except using diffuse transmission lobe (roughness ignored)
    //    float normalTextureScale = 1.f;
    //    float occlusionStrength = 1.f;
    //    float ior = 1.5f; // index of refraction, see KHR_materials_ior

    //    // Toggle between two PBR models: metal-rough and specular-gloss.
    //    // See the comments on the other fields here.
    //    bool useSpecularGlossModel = false;

    //    // Toggles for the textures. Only effective if the corresponding texture is non-null.
    //    bool enableBaseOrDiffuseTexture = true;
    //    bool enableMetalRoughOrSpecularTexture = true;
    //    bool enableNormalTexture = true;
    //    bool enableEmissiveTexture = true;
    //    bool enableOcclusionTexture = true;
    //    bool enableTransmissionTexture = true;

    //    bool doubleSided = false;

    //    bool thinSurface = false;   // As per Falcor/pt_sdk convention, ray hitting a material with the thin surface is assumed to enter and leave surface in the same bounce and it makes most sense when used with doubleSided; it skips all volume logic.

    //    bool excludeFromNEE = false; // The mesh will not be part of NEE.

    //    bool psdExclude = false; // will not propagate dominant stable plane when doing path space decomposition
    //    int psdDominantDeltaLobe = -1; // for path space decomposition: -1 means no dominant; 0 usually means transmission, 1 usually means reflection, 2 usually means clearcoat reflection - must match corresponding BSDFSample::getDeltaLobeIndex()!

    //    static constexpr int kMaterialMaxNestedPriority = 14;
    //    int nestedPriority = kMaterialMaxNestedPriority;     // When volume meshes overlap, will cause higher nestedPriority mesh to 'carve out' the volumes with lower nestedPriority (see https://www.sidefx.com/docs/houdini/render/nested.html)

    //    // KHR_materials_volume - see https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_volume#properties
    //    float volumeThicknessFactor = 0.0f; // in path tracing this is only used to set "thinSurface == thicknessFactor != 0" with value otherwise ignored (since we get exact thickness)
    //    float volumeAttenuationDistance = FLT_MAX;
    //    math::float3 volumeAttenuationColor = 1.f;

    //    // Low tessellation geometry often has triangle (flat) normals that differ significantly from shading normals. This causes shading vs shadow discrepancy that exposes triangle edges. 
    //    // One way to mitigate this (other than having more detailed mesh) is to add additional shadowing falloff to hide the seam. 
    //    // This setting is not physically correct and adds bias. Setting of 0 means no fadeout (default).
    //    float shadowNoLFadeout = 0.0f;

    //    int materialID = 0;
    //    bool dirty = true; // set this to true to make Scene update the material data

    //    virtual ~EMaterial() = default;
    //    void FillConstantBuffer(struct MaterialConstants& constants) const;
    //    bool SetProperty(const std::string& name, const math::float4& value);
    //};


    struct InputAssemblerBindings
    {
        VertexAttribute vertexBuffers[16];
        uint32_t numVertexBuffers;
    };

    struct BufferGroup
    {
        BufferHandle indexBuffer;
        BufferHandle vertexBuffer;
        // 存储 transform 信息，包括scale, translate, rotate
        BufferHandle instanceBuffer;

        BufferDesc indexBufferDesc;
        BufferDesc vertexBufferDesc;
        BufferDesc instanceBufferDesc;

        std::shared_ptr<DescriptorHandle> indexBufferDescriptor;
        std::shared_ptr<DescriptorHandle> vertexBufferDescriptor;
        std::shared_ptr<DescriptorHandle> instnaceBufferDescriptor;
        std::array<BufferRange, size_t(VertexAttribute::Count)> vertexBufferRanges;

        VertexBufferLayout vertexBufferLayout;

        std::vector<uint32_t> indexData;
        //std::vector<float> vertexData; 不需要vertexData, 如果有骨骼的话 vector类型会不一样
        //最后通过writeBuffer 来写
        std::vector<math::float3> positionData;
        std::vector<math::float3> prePositionData;

        std::vector<math::float2> texcoord1Data;
        std::vector<math::float2> texcoord2Data;
        std::vector<math::float3> normalData;
        std::vector<math::float3> tangentData;
        std::vector<math::float3> bitangentData;

        std::vector<uint32_t> jointIdData;
        std::vector<uint32_t> jointId1Data;
        std::vector<uint32_t> jointId2Data;

        std::vector<float> jointWeightData;
        std::vector<float> jointWeight1Data;
        std::vector<float> jointWeight2Data;

        //Transform Matrix
        InstanceData instanceData;



       // std::vector<math::vector<uint16_t, 4>> jointData;
        //std::vector<math::float4> weightData;

        [[nodiscard]] bool hasAttribute(VertexAttribute attr) const { return vertexBufferRanges[int(attr)].byteSize != 0; }
        BufferRange& getVertexBufferRange(VertexAttribute attr) { return vertexBufferRanges[int(attr)]; }
        [[nodiscard]] const BufferRange& getVertexBufferRange(VertexAttribute attr) const { return vertexBufferRanges[int(attr)]; }
    };

    struct MeshGeometryDebugData
    {
        uint32_t ommArrayDataOffset = 0xFFFFFFFF; // for use by applications
        uint32_t ommDescBufferOffset = 0xFFFFFFFF; // for use by applications
        uint32_t ommIndexBufferOffset = 0xFFFFFFFF; // for use by applications
        Format ommIndexBufferFormat = Format::R32_UINT; // for use by applications
        uint64_t ommStatsTotalKnown = 0;
        uint64_t ommStatsTotalUnknown = 0;
    };

    struct MeshDebugData
    {
        std::shared_ptr<DescriptorHandle> ommArrayDataBufferDescriptor;
        std::shared_ptr<DescriptorHandle> ommDescBufferDescriptor;
        std::shared_ptr<DescriptorHandle> ommIndexBufferDescriptor;
        BufferHandle ommArrayDataBuffer; // for use by applications
        BufferHandle ommDescBuffer; // for use by applications
        BufferHandle ommIndexBuffer; // for use by applications
    };

    //struct MeshGeometry
    //{
    //    std::shared_ptr<Material> material;
    //    math::box3 objectSpaceBounds;
    //    uint32_t indexOffsetInMesh = 0;
    //    uint32_t vertexOffsetInMesh = 0;
    //    uint32_t numIndices = 0;
    //    uint32_t numVertices = 0;
    //    int globalGeometryIndex = 0;

    //    // (Debug) OMM buffers.
    //    MeshGeometryDebugData debugData;
    //    virtual ~MeshGeometry() = default;
    //};



    //class MeshInstance 
    //{
    //private:
    //   // friend class SceneGraph;
    //    int m_InstanceIndex = -1;
    //    int m_GeometryInstanceIndex = -1;

    //protected:
    //    std::shared_ptr<MeshInfo> m_Mesh;

    //public:
    //    explicit MeshInstance(std::shared_ptr<MeshInfo> mesh)
    //        : m_Mesh(std::move(mesh))
    //    { }

    //    [[nodiscard]] const std::shared_ptr<MeshInfo>& GetMesh() const { return m_Mesh; }
    //    [[nodiscard]] int GetInstanceIndex() const { return m_InstanceIndex; }
    //    [[nodiscard]] int GetGeometryInstanceIndex() const { return m_GeometryInstanceIndex; }
    //    [[nodiscard]] math::box3 GetLocalBoundingBox() { return m_Mesh->objectSpaceBounds; }
    // /*   [[nodiscard]] std::shared_ptr<SceneGraphLeaf> Clone() override;
    //    [[nodiscard]] SceneContentFlags GetContentFlags() const override;
    //    bool SetProperty(const std::string& name, const dm::float4& value) override;*/
    //};

    //struct LightProbe
    //{
    //    std::string name;
    //    TextureHandle diffuseMap;
    //    TextureHandle specularMap;
    //    TextureHandle environmentBrdf;
    //    uint32_t diffuseArrayIndex = 0;
    //    uint32_t specularArrayIndex = 0;
    //    float diffuseScale = 1.f;
    //    float specularScale = 1.f;
    //    bool enabled = true;
    //    math::frustum bounds = math::frustum::infinite();

    //    [[nodiscard]] bool IsActive() const;
    //    void FillLightProbeConstants(LightProbeConstants& lightProbeConstants) const;
    //};

    inline IBuffer* BufferOrFallback(IBuffer* primary, IBuffer* secondary)
    {
        return primary ? primary : secondary;
    }
}
