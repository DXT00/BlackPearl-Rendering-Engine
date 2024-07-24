#pragma once

#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class WaterRenderLayer :public BlackPearl::Layer {
public:

	WaterRenderLayer(const std::string& name)
		: Layer(name)
	{

		//m_CameraObj = CreateCamera();
		//auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		//cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		//m_CameraPosition = cameraComponent->GetPosition();
		//m_CameraRotation.Yaw = cameraComponent->Yaw();
		//m_CameraRotation.Pitch = cameraComponent->Pitch();
		m_Scene = DBG_NEW BlackPearl::Scene();
		m_SphereObj = CreateSphere(0.5, 64, 64);
		m_QuadObj = CreateQuad();
		m_CubeObj = CreateCube();
		//m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale({ 0.2,0.2,0.2 });

		m_Scene->AddObject(m_SphereObj);
		m_Scene->AddObject(m_CubeObj);

		//Scene
		m_WaterRenderer = DBG_NEW BlackPearl::WaterRenderer();
		m_PBRRenderer = DBG_NEW BlackPearl::PBRRenderer();
		m_SkyboxRenderer = DBG_NEW BlackPearl::SkyboxRenderer();


		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_PBRRenderer->GetShader());

		BlackPearl::IDevice* device = m_DeviceManager->GetDevice();

		BlackPearl::TextureHandle StonealbedoTexture = device->createTexture({BlackPearl::TextureType::DiffuseMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_albedo.png"});
		BlackPearl::TextureHandle StoneaoTexture = device->createTexture({ BlackPearl::TextureType::AoMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_ao.png" });
		BlackPearl::TextureHandle StoneroughnessTexture = device->createTexture({ BlackPearl::TextureType::RoughnessMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_roughness.png" });
		BlackPearl::TextureHandle StonementallicTexture = device->createTexture({ BlackPearl::TextureType::MentallicMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_metallic.png" });
		BlackPearl::TextureHandle StonenormalTexture = device->createTexture({ BlackPearl::TextureType::NormalMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_normal-dx.png" });



		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonenormalTexture);

		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonealbedoTexture);
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StoneaoTexture);
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StoneroughnessTexture);
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonenormalTexture);
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1.0,0.0,0.0 });

		m_SphereObj->GetComponent<BlackPearl::Transform>()->SetPosition({
						5.0,
						5.0,
						5.0 });


		m_SkyBoxObj = CreateSkyBox(
			{ "assets/skybox/skybox1/SkyMorning_Right.png",
			 "assets/skybox/skybox1/SkyMorning_Left.png",
			 "assets/skybox/skybox1/SkyMorning_Top.png",
			 "assets/skybox/skybox1/SkyMorning_Bottom.png",
			 "assets/skybox/skybox1/SkyMorning_Front.png",
			 "assets/skybox/skybox1/SkyMorning_Back.png",
			});



		m_HDRPostProcessTexture.reset(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::None, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight, false, GL_LINEAR, GL_LINEAR, GL_RGBA16F, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));
		m_LightPassFrameBuffer.reset(DBG_NEW BlackPearl::FrameBuffer());
		m_LightPassFrameBuffer->Bind();
		m_LightPassFrameBuffer->AttachRenderBuffer(BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		m_LightPassFrameBuffer->AttachColorTexture(m_HDRPostProcessTexture, 0);
		m_LightPassFrameBuffer->BindRenderBuffer();
		m_LightPassFrameBuffer->UnBind();



		m_Bunny = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny", false);// CreateCube();
		m_Bunny->SetScale({ 3.0,3.0,3.0 });
		m_Bunny->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1.0,1.0,1.0 });

		BlackPearl::Object* light = CreateLight(BlackPearl::LightType::PointLight, "Light");
		light->SetPosition({ 0.0,12.0,5.0 });


		m_WaterRenderer->Init(m_Scene);

		BlackPearl::Renderer::Init();
	}

	virtual ~WaterRenderLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtime = currentTimeMs.count() - m_StartTimeMs.count();
		//m_PBRRenderer->GetShader()->SetUniformVec3f("u_albedo", { 0.5f, 0.0f, 0.0f });
		//m_PBRRenderer->GetShader()->SetUniform1f("u_ao", 1.0f);

		//for (int row = 0; row < m_Rows; row++) {
		//	for (int col = 0; col < m_Colums; col++) {

		//		m_SphereObj->GetComponent<BlackPearl::Transform>()->SetPosition({
		//			(col - (m_Colums / 2)) * m_Spacing,
		//			(row - (m_Rows / 2)) * m_Spacing,
		//			0.0f });
		//		m_PBRRenderer->GetShader()->SetUniform1f("u_metallic", 2.0*(float)row / (float)m_Rows);
		//		m_PBRRenderer->GetShader()->SetUniform1f("u_roughness", glm::clamp((float)col / (float)m_Colums, 0.05f, 1.0f));
		//		m_PBRRenderer->Render(m_SphereObj);
		//	}
		//}
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//1) render scene to framebuffer
		//m_LightPassFrameBuffer->Bind();

		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_PBRRenderer->Render(m_SphereObj);
		m_PBRRenderer->Render(m_Bunny);

		if (m_SkyBoxObj != nullptr) {
			glDepthFunc(GL_LEQUAL);
			//m_SkyboxRenderer->Render(m_SkyBoxObj, runtime/1000.0);
			m_SkyboxRenderer->Render(m_SkyBoxObj, 55.0);

			//m_SkyboxRenderer->Render(skyBox);
			//DrawObject(skyBox);
			glDepthFunc(GL_LESS);
		}

		//m_LightPassFrameBuffer->UnBind();
		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//2) post process to render cloud
		//glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		//glClear(GL_COLOR_BUFFER_BIT);
		//m_WaterRenderer->ShowNoise3DTexture(m_CubeObj);
		//m_WaterRenderer->RenderScene(m_MainCamera, m_QuadObj, m_HDRPostProcessTexture);
		m_WaterRenderer->Render(ts.GetSenconds(), m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>());

	}

	void OnAttach() override {


	}

private:
	//Scene
	BlackPearl::Scene* m_Scene;

	std::vector<BlackPearl::Object*> m_LightObjs;
	std::shared_ptr<BlackPearl::Texture> m_HDRPostProcessTexture;
	std::shared_ptr<BlackPearl::FrameBuffer> m_LightPassFrameBuffer;


	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_SphereObj;
	BlackPearl::Object* m_SkyBoxObj = nullptr;
	BlackPearl::Object* m_Bunny;


	BlackPearl::Object* m_Sword;

	int m_Rows = 4;
	int m_Colums = 4;
	float m_Spacing = 1.5;

	//Camera
	glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
	struct CameraRotation {
		float Yaw;
		float Pitch;

	};
	CameraRotation m_CameraRotation;
	float m_LastMouseX;
	float m_LastMouseY;
	float m_CameraMoveSpeed = 3.0f;
	float m_CameraRotateSpeed = 3.0f;

	BlackPearl::Object* m_SphereObjIron = nullptr;
	BlackPearl::Object* m_SphereObjRust = nullptr;
	BlackPearl::Object* m_SphereObjStone = nullptr;
	BlackPearl::Object* m_SphereObjPlastic = nullptr;


	//Renderer
	BlackPearl::WaterRenderer* m_WaterRenderer;
	BlackPearl::PBRRenderer* m_PBRRenderer;
	BlackPearl::SkyboxRenderer* m_SkyboxRenderer;


};
