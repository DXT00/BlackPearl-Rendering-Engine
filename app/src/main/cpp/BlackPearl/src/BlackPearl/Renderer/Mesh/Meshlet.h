#pragma once
#include "BlackPearl/Core.h"
#include "BlackPearl/Config.h"
namespace BlackPearl
{
    struct MeshletOption
    {
        
        MeshletOption() {
            Verts = Configuration::DefaultVerts;
            Prims = Configuration::DefaultPrims;
        }
        MeshletOption(unsigned int verts, unsigned int prims) {
            GE_ASSERT(verts <= Configuration::MaxVerts , "max vertices of a meshlet must less than" + std::to_string(Configuration::MaxVerts));
            GE_ASSERT(prims <= Configuration::MaxPrims , "max primitives of a meshlet must less than" + std::to_string(Configuration::MaxPrims));

            Verts = verts;
            Prims = prims;
        }
        MeshletOption& operator = (const MeshletOption& right) {
            this->Prims = right.Prims;
            this->Verts = right.Verts;
            return *this;
        }
        unsigned int Verts;
        unsigned int Prims;
    };
    

    struct Meshlet
    {
        uint32_t VertCount;
        uint32_t VertOffset;
        uint32_t PrimCount;
        uint32_t PrimOffset;
    };


} // namespace BlackPearl
