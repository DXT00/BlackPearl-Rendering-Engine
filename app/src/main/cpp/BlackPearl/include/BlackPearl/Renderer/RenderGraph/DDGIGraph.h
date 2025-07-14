#pragma once

#include "RenderGraph.h"
#include "RHI/RHITexture.h"
#include "RHI/RHIFrameBuffer.h"
#include "RHI/RHICommandList.h"
#include "Renderer/MasterRenderer/GI/DDGIRenderer.h"
#include "Renderer/Renderer.h"
#include "Renderer/DeviceManager.h"
#include "Timestep/Timestep.h"
namespace BlackPearl {
	class DDGIGraph :public RenderGraph
	{
	public:
		explicit DDGIGraph(DeviceManager* deviceManager)
			: RenderGraph(deviceManager)
		{
		}
		virtual void Init(Scene* scene) override;
		virtual void Render(Timestep ts, IFramebuffer* framebuffer, IView* View) override;

		

	private:

		// RenderTarget:



		CommandListHandle    m_CommandList;
		Scene* m_Scene = nullptr;

        GIRenderer* m_DDGIRenderer;

   


        bool bIsProbesDirty = true;
	};

}