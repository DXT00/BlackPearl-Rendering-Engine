#pragma once
#include "BlackPearl/RHI/RHIQuery.h"
#include "VkQueue.h"

namespace BlackPearl {


    class EventQuery : public RefCounter<IEventQuery>
    {
    public:
        CommandQueue queue = CommandQueue::Graphics;
        uint64_t commandListID = 0;
    };

    //class TimerQuery : public RefCounter<ITimerQuery>
    //{
    //public:
    //    int beginQueryIndex = -1;
    //    int endQueryIndex = -1;

    //    bool started = false;
    //    bool resolved = false;
    //    float time = 0.f;

    //    explicit TimerQuery(utils::BitSetAllocator& allocator)
    //        : m_QueryAllocator(allocator)
    //    { }

    //    ~TimerQuery() override;

    //private:
    //    utils::BitSetAllocator& m_QueryAllocator;
    //};

}
