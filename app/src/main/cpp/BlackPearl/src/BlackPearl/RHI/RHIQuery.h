#pragma once
#include "RHIResources.h"
namespace BlackPearl {
    class IEventQuery : public IResource { };
    typedef RefCountPtr<IEventQuery> EventQueryHandle;

    class ITimerQuery : public IResource { };
    typedef RefCountPtr<ITimerQuery> TimerQueryHandle;
}
