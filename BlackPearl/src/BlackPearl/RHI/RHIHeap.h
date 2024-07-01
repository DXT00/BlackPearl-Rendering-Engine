#pragma once
#include "RHIResources.h"
#include "RHIDefinitions.h"
namespace BlackPearl {

   
    class IHeap : public IResource
    {
    public:
        virtual const HeapDesc& getDesc() = 0;
    };

    typedef RefCountPtr<IHeap> HeapHandle;

}

