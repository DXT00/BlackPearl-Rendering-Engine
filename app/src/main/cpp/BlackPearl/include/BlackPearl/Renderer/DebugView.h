#pragma once
#include <string>
namespace BlackPearl {
    /*
    BindingCache maintains a dictionary that maps binding set descriptors
    into actual binding set objects. The binding sets are created on demand when
    GetOrCreateBindingSet(...) is called and the requested binding set does not exist.
    Created binding sets are stored for the lifetime of BindingCache, or until
    Clear() is called.

    All BindingCache methods are thread-safe.
    */
    class DebugView
    {
        
    public:
        enum Type {

            DV_Lit,
            DV_Voxel,
            DV_SDF,
            DV_Cnt

        };

        static std::string ToString(DebugView::Type type) {
            switch (type)
            {
            case DebugView::DV_Lit:
                return "ShowLit";
            case DebugView::DV_Voxel:
                return "ShowVoxel";
            case DebugView::DV_SDF:
                return "ShowSDF";
            case DebugView::DV_Cnt:
                break;
            default:
                break;
            }
            return "Unknown";
        }
        
    };

}