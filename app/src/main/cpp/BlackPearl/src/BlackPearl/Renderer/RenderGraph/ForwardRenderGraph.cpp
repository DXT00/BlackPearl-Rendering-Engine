#include "pch.h"
#include "Renderer/RenderGraph/ForwardRenderGraph.h"
#include "BlackPearl/Renderer/CullingManager.h"
#include "BlackPearl/FileSystem/FileSystem.h"
#include "BlackPearl/Renderer/MasterRenderer/BasePassRenderer.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#include "Renderer/SystemTextures.h"

namespace BlackPearl {
	extern CullingManager* g_cullingManager;
	extern RootFileSystem* g_rootFileSystem;
	void ForwardRenderGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		//m_ShaderFactory = std::make_shared<ShaderFactory>(m_DeviceManager->GetDevice(), g_rootFileSystem, "assets/shaders/spv");
		m_Scene = scene;
		
		_CreateRenderTagets();
		


		//BasePass
		//m_BasePassRenderer = DBG_NEW BasePassRenderer(m_DeviceManager->GetDevice());
        m_ForwardBasePassRenderer = DBG_NEW ForwardShadingRenderer(m_DeviceManager->GetDevice());
		m_SkyboxRenderer = DBG_NEW SkyboxRenderer(m_DeviceManager->GetDevice());


        m_ForwardBasePassRenderer->Init();
		m_SkyboxRenderer->Init();

		/*m_PostProcessRenderer = DBG_NEW PostProcessRenderer();
		m_PostProcessRenderer->Init(GetDevice(), m_ShaderFactory);*/
		
		AddPass(m_SkyboxRenderer);
		AddPass(m_ForwardBasePassRenderer);

		//AddPass(m_IndirectCullRenderer);
		//AddPass(m_BasePassRenderer);
		//AddPass(m_PostProcessRenderer);
	}

	void ForwardRenderGraph::Render(Timestep ts, IFramebuffer* framebuffer, IView* View) {

		if (SupportSinglePass(Configuration::MSAA_SAMPLES)) {
			RenderSinglePass(ts, framebuffer, View);
		}
		else {
			RenderMultiPass(ts, framebuffer, View);
		}
	}
	void ForwardRenderGraph::RenderUI(Timestep ts, IFramebuffer* framebuffer, IView* View)
	{
        //m_CommandList->open();
        //FRHIRenderPassInfo RPInfo(framebuffer->getDesc().colorAttachments[0].texture, ERenderTargetActions::Load_Store,
        //    SystemTexture::Get().SceneDepth,
        //    EDepthStencilTargetActions::LoadDepthStencil_StoreDepthStencil);
        //m_CommandList->beginRenderPass(RPInfo, "UIPass");
        //ImGui_ImplOpenGL3_NewFrame();
        //ImGui_ImplWin32_NewFrame();
        //ImGui::NewFrame();







        //ImGui::Render();
        //GE_ERROR_JUDGE();
        //ImGuiIO& io = ImGui::GetIO(); (void)io;

     
        //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        //{

        //    HDC         dc = wglGetCurrentDC();
        //    HGLRC       rc = wglGetCurrentContext();

        //    auto context = wglGetCurrentContext();
        //    ImGui::UpdatePlatformWindows();
        //    ImGui::RenderPlatformWindowsDefault();
        //    wglMakeCurrent(dc, rc);
        //    // 恢复 OpenGL 上下文
        //    //glfwMakeContextCurrent(backup_current_context);


        //}

        //m_CommandList->endRenderPass();

	}
	void ForwardRenderGraph::RenderSinglePass(Timestep ts, IFramebuffer* framebuffer, IView* View)
	{


		m_CommandList->open();

		FRHIRenderPassInfo RPInfo(framebuffer->getDesc().colorAttachments[0].texture, ERenderTargetActions::Load_Store,
            SystemTexture::Get().SceneDepth,
            EDepthStencilTargetActions::LoadDepthStencil_StoreDepthStencil);
		m_CommandList->beginRenderPass(RPInfo, "BasePass");

		m_SkyboxRenderer->Render(m_CommandList, framebuffer, m_Scene);
        m_ForwardBasePassRenderer->Render(m_CommandList, framebuffer, m_Scene);

		m_CommandList->endRenderPass();

		//PostProcessPass GI
		//voxel cone tracing

		m_CommandList->close();

		GetDevice()->executeCommandList(m_CommandList);
	}
	void ForwardRenderGraph::RenderMultiPass(Timestep ts, IFramebuffer* framebuffer, IView* View)
	{
        //todo::
        m_CommandList->open();

        FRHIRenderPassInfo RPInfo(framebuffer->getDesc().colorAttachments[0].texture, ERenderTargetActions::Load_Store);
        m_CommandList->beginRenderPass(RPInfo, "BasePass");

        m_SkyboxRenderer->Render(m_CommandList, framebuffer, m_Scene);
        m_ForwardBasePassRenderer->Render(m_CommandList, framebuffer, m_Scene);

        m_CommandList->endRenderPass();

        //PostProcessPass GI
        //voxel cone tracing

        m_CommandList->close();

        GetDevice()->executeCommandList(m_CommandList);
	}



	void ForwardRenderGraph::_CreateRenderTagets()
	{
		m_RenderTargets = std::make_unique<RenderTargets>();
		math::int2 renderSize = math::int2(Configuration::WindowWidth);
		m_RenderTargets->Init(GetDevice(), renderSize, renderSize, true, true, Configuration::SwapchainCount);
	}

}