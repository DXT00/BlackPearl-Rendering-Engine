#pragma once

enum class MaterialResource
{
    ConstantBuffer,
    Sampler,
    DiffuseTexture,
    SpecularTexture,
    NormalTexture,
    EmissiveTexture,
    OcclusionTexture,
    TransmissionTexture
};

struct MaterialResourceBinding
{
    MaterialResource resource;
    uint32_t slot; // type depends on resource
};
