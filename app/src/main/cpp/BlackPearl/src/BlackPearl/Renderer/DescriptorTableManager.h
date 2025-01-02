#pragma once
#include "BlackPearl/RHI/RHIBindingSet.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/RHI/RHIDescriptorTable.h"
namespace BlackPearl {
    class DescriptorTableManager;
    typedef int DescriptorIndex;

    // Stores a descriptor index in a descriptor table. Releases the descriptor when destroyed.
    class DescriptorHandle
    {
    private:
        std::weak_ptr<DescriptorTableManager> m_Manager;
        DescriptorIndex m_DescriptorIndex;

    public:
        DescriptorHandle();
        DescriptorHandle(const std::shared_ptr<DescriptorTableManager>& managerPtr, DescriptorIndex index);
        ~DescriptorHandle();

        [[nodiscard]] bool IsValid() const { return m_DescriptorIndex >= 0 && !m_Manager.expired(); }
        [[nodiscard]] DescriptorIndex Get() const { if (m_DescriptorIndex >= 0) assert(!m_Manager.expired()); return m_DescriptorIndex; }
        void Reset() { m_DescriptorIndex = -1; m_Manager.reset(); }

        // Movable but non-copyable
        DescriptorHandle(const DescriptorHandle&) = delete;
        DescriptorHandle(DescriptorHandle&&) = default;
        DescriptorHandle& operator=(const DescriptorHandle&) = delete;
        DescriptorHandle& operator=(DescriptorHandle&&) = default;
    };

    class DescriptorTableManager : public std::enable_shared_from_this<DescriptorTableManager>
    {
    protected:
        // Custom hasher that doesn't look at the binding slot
        struct BindingSetItemHasher
        {
            std::size_t operator()(const BindingSetItem& item) const
            {
                size_t hash = 0;
                hash_combine(hash, item.resourceHandle);
                hash_combine(hash, item.type);
                hash_combine(hash, item.format);
                hash_combine(hash, item.dimension);
                hash_combine(hash, item.rawData[0]);
                hash_combine(hash, item.rawData[1]);
                return hash;
            }
        };

        // Custom equality tester that doesn't look at the binding slot
        struct BindingSetItemsEqual
        {
            bool operator()(const BindingSetItem& a, const BindingSetItem& b) const
            {
                return a.resourceHandle == b.resourceHandle
                    && a.type == b.type
                    && a.format == b.format
                    && a.dimension == b.dimension
                    && a.subresources == b.subresources;
            }
        };

        DeviceHandle m_Device;
        DescriptorTableHandle m_DescriptorTable;

        std::vector<BindingSetItem> m_Descriptors;
        std::unordered_map<BindingSetItem, DescriptorIndex, BindingSetItemHasher, BindingSetItemsEqual> m_DescriptorIndexMap;
        std::vector<bool> m_AllocatedDescriptors;
        int m_SearchStart = 0;

    public:
        DescriptorTableManager(IDevice* device, IBindingLayout* layout);
        ~DescriptorTableManager();

        IDescriptorTable* GetDescriptorTable() const { return m_DescriptorTable; }

        DescriptorIndex CreateDescriptor(BindingSetItem item);
        DescriptorHandle CreateDescriptorHandle(BindingSetItem item);
        BindingSetItem GetDescriptor(DescriptorIndex index);
        void ReleaseDescriptor(DescriptorIndex index);
    };
}

