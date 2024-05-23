#include "pch.h"
#include "BindingCache.h"
#include <cassert>

namespace BlackPearl {
    BindingSetHandle BindingCache::GetCachedBindingSet(const BindingSetDesc& desc, IBindingLayout* layout)
    {
        return BindingSetHandle();
    }
    BindingSetHandle BindingCache::GetOrCreateBindingSet(const BindingSetDesc& desc, IBindingLayout* layout)
    {
        size_t hash = 0;
        hash_combine(hash, desc);
        hash_combine(hash, layout);

        m_Mutex.lock_shared();

        BindingSetHandle result;
        auto it = m_BindingSets.find(hash);
        if (it != m_BindingSets.end())
            result = it->second;

        m_Mutex.unlock_shared();

        if (!result)
        {
            m_Mutex.lock();

            BindingSetHandle& entry = m_BindingSets[hash];
            if (!entry)
            {
                result = m_Device->createBindingSet(desc, layout);
                entry = result;
            }
            else
                result = entry;

            m_Mutex.unlock();
        }

        if (result)
        {
            assert(result->getDesc());
            assert(*result->getDesc() == desc);
        }

        return result;
    }
    void BindingCache::Clear()
    {
    }
}