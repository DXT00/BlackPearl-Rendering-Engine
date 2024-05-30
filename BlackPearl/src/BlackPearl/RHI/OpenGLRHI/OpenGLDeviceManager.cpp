#include "pch.h"
#include "BlackPearl/Renderer/DeviceManager.h"
namespace BlackPearl {
    class D3D12DeviceManager : public DeviceManager
    {

        [[nodiscard]] const char* GetRendererString() const override
        {
            return "";
        }

        [[nodiscard]] IDevice* GetDevice() const override
        {
            return nullptr;
        }

    
    };

    DeviceManager* DeviceManager::CreateOpenGL()
    {
        return nullptr;
    }
}