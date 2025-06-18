#include "pch.h"
#include "Renderer/MasterRenderer/SkyboxRenderer.h"
#include "Renderer/DeviceManager.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#include "RHI/RHIState.h"
#include "RHI/RHIBindingSet.h"
#include "Core.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"
#include "Timestep/SystemTime.h"
#include "Timestep/TimeCounter.h"

namespace BlackPearl {
	extern DeviceManager* g_deviceManager;
	SkyboxRenderer::SkyboxRenderer(IDevice* device)
		:BasicRenderer(device)
	{
		
		
	}

	void SkyboxRenderer::Init()
	{
		std::vector<std::string> morningBox = {
			"assets/skybox/skybox1/SkyBrightMorning_Right.png",
			"assets/skybox/skybox1/SkyBrightMorning_Left.png",
			"assets/skybox/skybox1/SkyBrightMorning_Top.png",
			"assets/skybox/skybox1/SkyBrightMorning_Bottom.png",
			"assets/skybox/skybox1/SkyBrightMorning_Front.png",
			"assets/skybox/skybox1/SkyBrightMorning_Back.png"
		};
		std::vector<std::string> sunSetBox = {
			"assets/skybox/skybox1/SkyMorning_Right.png",
			"assets/skybox/skybox1/SkyMorning_Left.png",
			"assets/skybox/skybox1/SkyMorning_Top.png",
			"assets/skybox/skybox1/SkyMorning_Bottom.png",
			"assets/skybox/skybox1/SkyMorning_Front.png",
			"assets/skybox/skybox1/SkyMorning_Back.png"
		};
		std::vector<std::string> nightBox = {
			"assets/skybox/skybox1/SkyNight_Right.png",
			"assets/skybox/skybox1/SkyNight_Left.png",
			"assets/skybox/skybox1/SkyNight_Top.png",
			"assets/skybox/skybox1/SkyNight_Bottom.png",
			"assets/skybox/skybox1/SkyNight_Front.png",
			"assets/skybox/skybox1/SkyNight_Back.png"
		};
		m_SkyboxShader = DBG_NEW MaterialShader("assets/shaders/glsl/SkyBoxMultiTexture.glsl");

		TextureDesc desc;
		desc.type = TextureType::CubeMap;
		desc.minFilter = FilterMode::Linear;
		desc.magFilter = FilterMode::Linear;
		desc.wrap = SamplerAddressMode::ClampToEdge;
		desc.format = Format::RGB8_UNORM;
		desc.faces = nightBox;
		desc.dimension = TextureDimension::TextureCube;
		m_SkyboxTexture[0] = g_deviceManager->GetDevice()->createTexture(desc);
		desc.faces = morningBox;
		m_SkyboxTexture[1] = g_deviceManager->GetDevice()->createTexture(desc);
		desc.faces = sunSetBox;
		m_SkyboxTexture[2] = g_deviceManager->GetDevice()->createTexture(desc);


		//Skybox Material
		RHIBindingLayoutDesc layoutDesc;
		layoutDesc.visibility = ShaderType::Pixel;
		layoutDesc.bindings = {
			RHIBindingLayoutItem::RT_VolatileConstantBuffer(2),
			RHIBindingLayoutItem::RT_Texture_SRV(1),
			RHIBindingLayoutItem::RT_Texture_SRV(2),
			RHIBindingLayoutItem::RT_Texture_SRV(3)

		};
		m_SkyboxBindingLayout = m_Device->createBindingLayout(layoutDesc);
		m_SkyCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(SkyConstants), "SkyConstants"));

		BindingSetDesc bindingSetDesc;
		bindingSetDesc.bindings = {
			BindingSetItem::ConstantBuffer(2, m_SkyCB),
			BindingSetItem::Texture_SRV(1, m_SkyboxTexture[0].Get(), "skyboxTexture0"),
			BindingSetItem::Texture_SRV(2, m_SkyboxTexture[1].Get(), "skyboxTexture1"),
			BindingSetItem::Texture_SRV(3, m_SkyboxTexture[2].Get(), "skyboxTexture2")

		};
		m_SkyboxBindingSet = m_Device->createBindingSet(bindingSetDesc, m_SkyboxBindingLayout);

	}
	void SkyboxRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
	{
        SCOPE_TIME_COUNTER(Skybox);

        if (!scene->GetSkyBox()) {
			GE_CORE_WARN("no skybox found");
			return;
		}
		//cmdList->beginMarker("SkyPass");

		/*FRHIRenderPassInfo RPInfo(targetFramebuffer->getDesc().colorAttachments[0].texture, ERenderTargetActions::Clear_Store);
		cmdList->beginRenderPass(RPInfo, "SkyPass");*/

		SceneData* view = Renderer::GetSceneData();
		GE_ERROR_JUDGE();

		SceneData* preView = Renderer::GetPreSceneData();
		GE_ERROR_JUDGE();

		SetupView(cmdList, view, preView);

		double timeSecond = SystemTime::GetRuntimeFromStartMs() / 1000.0f;
		float currentTimeS = fmod(timeSecond ,m_TotalTimeIntervalS);
		int state = int(currentTimeS / m_StateIntervalS);
		int nextState = state + 1;
		float stateFactor = nextState * m_StateIntervalS - currentTimeS; 
		float nextStateFactor = currentTimeS - state * m_StateIntervalS;
		nextState %= 3;
		state %= 3;
	


		DrawItem drawItem = IDrawStrategy::ObjectToDrawItem(scene->GetSkyBox())[0];

		GraphicsState graphicsPSO;
		graphicsPSO.framebuffer = targetFramebuffer;
		graphicsPSO.viewport = view->GetViewportState();
		graphicsPSO.shadingRateState = view->GetVariableRateShadingState();

		GraphicsPipelineDesc psoDesc;

		psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
		psoDesc.depthStencilState.enableDepthTest();
		psoDesc.depthStencilState.disableDepthWrite();
		psoDesc.depthStencilState.disableStencil();

		psoDesc.blendState.alphaToCoverageEnable = false;
		psoDesc.rasterState.frontCounterClockwise = true;
		psoDesc.rasterState.cullMode = RasterCullMode::None;
		psoDesc.primType = PrimitiveType::TriangleList;
		psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

		psoDesc.VS = m_SkyboxShader->GetVertexShader();
		psoDesc.PS = m_SkyboxShader->GetPixelShader();
		psoDesc.bFromPSOFileCache = false;
		psoDesc.bindingLayouts.push_back(m_SkyboxBindingLayout);
		psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


		if (!m_SkyboxPso) {
			m_SkyboxPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
		}
		graphicsPSO.pipeline = m_SkyboxPso;
		graphicsPSO.bindings.push_back(m_SkyboxBindingSet);
		graphicsPSO.bindings.push_back(m_ViewBindingset);
        graphicsPSO.inputLayout = psoDesc.inputLayout;
		/*for (int j = 0; j < shaderParms[ShaderType::Pixel].bindingLayouts.size(); ++j) {
			psoDesc.bindingLayouts.push_back(shaderParms[ShaderType::Pixel].bindingLayouts[j]);
		}

		for (int j = 0; j < shaderParms[ShaderType::Pixel].bindingSets.size(); ++j) {
			graphicsPSO.bindings.push_back(shaderParms[ShaderType::Pixel].bindingSets[j]);
		}*/

		/*GE_ERROR_JUDGE();
		SetupMaterial(drawItem.material, drawItem.cullMode, psoDesc, graphicsPSO);*/
		SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
		GE_ERROR_JUDGE();


		if (scene->GetLightSources()->GetParallelLights().empty()) {
			GE_CORE_ERROR("no direction light found in SkyPass");
			return;
		}
		Object* light = scene->GetLightSources()->GetParallelLights()[0];
		SkyConstants skyConstants{};

		FillShaderParameters(*light->GetComponent<DirectionLight>(), m_SkyParam, skyConstants);
		skyConstants.factors[state] = stateFactor / m_StateIntervalS;
		skyConstants.factors[nextState] = nextStateFactor / m_StateIntervalS;
		skyConstants.factors[3 - state - nextState] = 0;

			
		cmdList->writeBuffer(m_SkyCB, &skyConstants, sizeof(SkyConstants));
		cmdList->setGraphicsState(graphicsPSO);

		Draw(cmdList, drawItem);

		//cmdList->endRenderPass();
		//cmdList->endMarker();
	}
	
	

	void SkyboxRenderer::FillShaderParameters(const DirectionLight& light, const SkyParameters& input, SkyConstants& output)
	{


		float lightAngularSize = math::radians(math::clamp(light.GetLightProps().angularSize, 0.1f, 90.f));
		float lightSolidAngle = 4 * math::PI_f * square(sinf(lightAngularSize * 0.5f));
		float lightRadiance = light.GetLightProps().intensity / lightSolidAngle;
		if (input.maxLightRadiance > 0.f)
			lightRadiance = min(lightRadiance, input.maxLightRadiance);

		output.directionToLight = math::float3(math::normalize(-light.GetDirection()));
		output.angularSizeOfLight = lightAngularSize;
		output.lightColor = lightRadiance * light.GetLightProps().ambient;
		output.glowSize = math::radians(math::clamp(input.glowSize, 0.f, 90.f));
		output.skyColor = input.skyColor * input.brightness;
		output.glowIntensity = math::clamp(input.glowIntensity, 0.f, 1.f);
		output.horizonColor = input.horizonColor * input.brightness;
		output.horizonSize = math::radians(math::clamp(input.horizonSize, 0.f, 90.f));
		output.groundColor = input.groundColor * input.brightness;
		output.glowSharpness = math::clamp(input.glowSharpness, 1.f, 10.f);
		output.directionUp = normalize(input.directionUp);
		output.lightIntensity = light.GetLightProps().intensity;

	}

}