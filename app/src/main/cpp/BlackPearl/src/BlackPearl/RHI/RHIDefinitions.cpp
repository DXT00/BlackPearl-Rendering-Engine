#include "pch.h"
#include "RHIDefinitions.h"

namespace BlackPearl {
	bool BlendState::usesConstantColor(uint32_t numTargets) const
	{
        for (uint32_t rt = 0; rt < numTargets; rt++)
        {
            if (targets[rt].usesConstantColor())
                return true;
        }

        return false;
	}
    bool BlendState::RenderTarget::usesConstantColor() const
    {
        return false;
    }
}

