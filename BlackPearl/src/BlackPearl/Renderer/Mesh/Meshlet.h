#include "BlackPearl/Core.h"
#include "BlackPearl/Config.h"
namespace BlackPearl
{
    struct MeshletOption
    {
        
        MeshletOption() {
            max_verts = Configuration::MaxVerts;
            max_prims = Configuration::MaxPrims;
        }
        MeshletOption(unsigned int max_verts, unsigned int max_prims) {
            GE_ASSERT(max_verts <= Configuration::MaxVerts , "max vertices of a meshlet must less than" + std::to_string(Configuration::MaxVerts));
            GE_ASSERT(max_prims <= Configuration::MaxPrims , "max primitives of a meshlet must less than" + std::to_string(Configuration::MaxPrims));

            max_verts = max_verts;
            max_prims = max_prims;
        }

        unsigned int max_verts;
        unsigned int max_prims;
    };
    
    class Meshlet
    {

    public:
        Meshlet(/* args */);
        ~Meshlet();
    private:
        /* data */
    };
    
    Meshlet::Meshlet(/* args */)
    {
    }
    
    Meshlet::~Meshlet()
    {
    }
    
} // namespace BlackPearl
