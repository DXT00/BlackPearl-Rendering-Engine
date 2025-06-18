#pragma once
#include "BlackPearl/RHI/RHIFrameBuffer.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "../DeviceManager.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Renderer/CommonRenderPass.h"
#include "Timestep/Timestep.h"

namespace BlackPearl {
    //class DeviceManager;
    class RenderGraph
    {

    public:
        explicit RenderGraph(DeviceManager* deviceManager)
            : m_DeviceManager(deviceManager)
        { }
        virtual void Init(Scene* scene) {}
        void AddPass(BasicRenderer* renderer);
       // void OnPreRender() {}
        virtual ~RenderGraph() = default;

       
        virtual void Render(Timestep ts, IFramebuffer* framebuffer, IView* View) { }
        virtual void Animate(float fElapsedTimeSeconds) { }
        virtual void BackBufferResizing() { }
        virtual void BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount) { }

        // all of these pass in GLFW constants as arguments
        // see http://www.glfw.org/docs/latest/input.html
        // return value is true if the event was consumed by this render pass, false if it should be passed on
        virtual bool KeyboardUpdate(int key, int scancode, int action, int mods) { return false; }
        virtual bool KeyboardCharInput(unsigned int unicode, int mods) { return false; }
        virtual bool MousePosUpdate(double xpos, double ypos) { return false; }
        virtual bool MouseScrollUpdate(double xoffset, double yoffset) { return false; }
        virtual bool MouseButtonUpdate(int button, int action, int mods) { return false; }
        virtual bool JoystickButtonUpdate(int button, bool pressed) { return false; }
        virtual bool JoystickAxisUpdate(int axis, float value) { return false; }

        [[nodiscard]] DeviceManager* GetDeviceManager() const { return m_DeviceManager; }
        [[nodiscard]] IDevice* GetDevice() const { return m_DeviceManager->GetDevice();
        }
        [[nodiscard]] uint32_t GetFrameIndex() const { return m_DeviceManager->GetFrameIndex(); }

        //如果支持, 优先使用 single pass
        static bool SupportSinglePass(bool msaaSample);
        //如果支持, 优先使用 PLS
        static bool SupportPLS();

    protected:
        DeviceManager* m_DeviceManager = nullptr;

        std::vector<BasicRenderer*> mRenderPasses;


    };
}


