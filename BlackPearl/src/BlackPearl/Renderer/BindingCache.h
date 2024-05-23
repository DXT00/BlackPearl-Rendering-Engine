#pragma once
#include <shared_mutex>
#include <unordered_map>
#include "BlackPearl/RHI/RHIBindingSet.h"
#include "BlackPearl/RHI/RHIDevice.h"
namespace BlackPearl {
    /*
    BindingCache maintains a dictionary that maps binding set descriptors
    into actual binding set objects. The binding sets are created on demand when
    GetOrCreateBindingSet(...) is called and the requested binding set does not exist.
    Created binding sets are stored for the lifetime of BindingCache, or until
    Clear() is called.

    All BindingCache methods are thread-safe.
    */
    class BindingCache
    {
    private:
        DeviceHandle m_Device;
        std::unordered_map<size_t, BindingSetHandle> m_BindingSets;
        std::shared_mutex m_Mutex;

    public:
        BindingCache(IDevice* device)
            : m_Device(device)
        { }
        BindingCache()
            : m_Device(nullptr)
        { }

        BindingSetHandle GetCachedBindingSet(const BindingSetDesc& desc, IBindingLayout* layout);
        BindingSetHandle GetOrCreateBindingSet(const BindingSetDesc& desc, IBindingLayout* layout);
        void Clear();
    };

}