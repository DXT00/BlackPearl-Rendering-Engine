#pragma once
#pragma once
#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class CloudRenderLayer :public BlackPearl::Layer {
public:

	CloudRenderLayer(const std::string& name)
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
		m_Cloudenderer = DBG_NEW BlackPearl::CloudRenderer();
		m_PBRRenderer = DBG_NEW BlackPearl::PBRRenderer();
		m_SkyboxRenderer = DBG_NEW BlackPearl::SkyboxRenderer();


		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_PBRRenderer->GetShader());
		std::shared_ptr<BlackPearl::Texture> albedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_albedo.png"));
		std::shared_ptr<BlackPearl::Texture> aoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_ao.png"));
		std::shared_ptr<BlackPearl::Texture> roughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_roughness.png"));
		std::shared_ptr<BlackPearl::Texture> mentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_metallic.png"));
		std::shared_ptr<BlackPearl::Texture> normalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_normal-dx.png"));

		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(normalTexture);

		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(albedoTexture);
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(aoTexture);
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(roughnessTexture);
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(mentallicTexture);
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


		
		//m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_PBRRenderer->GetShader());
		//std::shared_ptr<BlackPearl::Texture> albedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/rustSphere/rustediron2_basecolor.png"));
		//std::shared_ptr<BlackPearl::Texture> aoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/rustSphere/rustediron2_ao.png"));
		//std::shared_ptr<BlackPearl::Texture> roughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/rustSphere/rustediron2_roughness.png"));
		//std::shared_ptr<BlackPearl::Texture> mentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/rustSphere/rustediron2_metallic.png"));
		//std::shared_ptr<BlackPearl::Texture> normalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/rustSphere/rustediron2_normal.png"));

		//std::shared_ptr<BlackPearl::Texture> albedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_albedo.png"));
		//std::shared_ptr<BlackPearl::Texture> aoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_ao.png"));
		//std::shared_ptr<BlackPearl::Texture> roughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_roughness.png"));
		//std::shared_ptr<BlackPearl::Texture> mentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_metallic.png"));
		//std::shared_ptr<BlackPearl::Texture> normalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_normal-dx.png"));

		//m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(normalTexture);

		//m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(albedoTexture);
		//m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(aoTexture);
		//m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(roughnessTexture);
		//m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(mentallicTexture);

		//m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1.0,0.0,0.0 });
		//m_SphereObjIron = CreateSphere(1.5, 64, 64);
		//m_SphereObjRust = CreateSphere(1.5, 64, 64);
		//m_SphereObjStone = CreateSphere(1.5, 64, 64);
		//m_SphereObjPlastic = CreateSphere(1.5, 64, 64);


		////textures spheres
		//std::shared_ptr<BlackPearl::Texture> RustalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/rustSphere/rustediron2_basecolor.png"));
		//std::shared_ptr<BlackPearl::Texture> RustaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/rustSphere/rustediron2_ao.png"));
		//std::shared_ptr<BlackPearl::Texture> RustroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/rustSphere/rustediron2_roughness.png"));
		//std::shared_ptr<BlackPearl::Texture> RustmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/rustSphere/rustediron2_metallic.png"));
		//std::shared_ptr<BlackPearl::Texture> RustnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/rustSphere/rustediron2_normal.png"));


		//std::shared_ptr<BlackPearl::Texture> IronalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_basecolor.png"));
		//std::shared_ptr<BlackPearl::Texture> IronaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_ao.png"));
		//std::shared_ptr<BlackPearl::Texture> IronroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_roughness.png"));
		//std::shared_ptr<BlackPearl::Texture> IronmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_metallic.png"));
		//std::shared_ptr<BlackPearl::Texture> IronnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_normal.png"));

		//std::shared_ptr<BlackPearl::Texture> StonealbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_albedo.png"));
		//std::shared_ptr<BlackPearl::Texture> StoneaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_ao.png"));
		//std::shared_ptr<BlackPearl::Texture> StoneroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_roughness.png"));
		//std::shared_ptr<BlackPearl::Texture> StonementallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_metallic.png"));
		//std::shared_ptr<BlackPearl::Texture> StonenormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_normal-dx.png"));

		//std::shared_ptr<BlackPearl::Texture> PlasticalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/plasticSphere/scuffed-plastic4-alb.png"));
		//std::shared_ptr<BlackPearl::Texture> PlasticaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-ao.png"));
		//std::shared_ptr<BlackPearl::Texture> PlasticroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-rough.png"));
		//std::shared_ptr<BlackPearl::Texture> PlasticmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-metal.png"));
		//std::shared_ptr<BlackPearl::Texture> PlasticnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-normal.png"));

		//m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronnormalTexture);
		//m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronalbedoTexture);
		//m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronaoTexture);
		//m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronroughnessTexture);
		//m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronmentallicTexture);
		//m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetPBRTextureSamples(true);
		//m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetIsPBRObject(true);
		//m_SphereObjIron->GetComponent<BlackPearl::Transform>()->SetPosition({ 10,0,0 });

		//m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustnormalTexture);
		//m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustalbedoTexture);
		//m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustaoTexture);
		//m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustroughnessTexture);
		//m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustmentallicTexture);
		//m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetPBRTextureSamples(true);
		//m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetIsPBRObject(true);
		//m_SphereObjRust->GetComponent<BlackPearl::Transform>()->SetPosition({ 5,0,0 });

		//m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonenormalTexture);
		//m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonealbedoTexture);
		//m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StoneaoTexture);
		//m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StoneroughnessTexture);
		//m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonementallicTexture);
		//m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetPBRTextureSamples(true);
		//m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetIsPBRObject(true);
		//m_SphereObjStone->GetComponent<BlackPearl::Transform>()->SetPosition({ -5,0,0 });

		//m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticnormalTexture);
		//m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticalbedoTexture);
		//m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticaoTexture);
		//m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticroughnessTexture);
		//m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticmentallicTexture);
		//m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetPBRTextureSamples(true);
		//m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetIsPBRObject(true);
		//m_SphereObjPlastic->GetComponent<BlackPearl::Transform>()->SetPosition({ -10.0,0,0 });

		m_Bunny = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny", false);// CreateCube();
		m_Bunny->SetScale({ 3.0,3.0,3.0 });
		m_Bunny->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1.0,1.0,1.0 });

		BlackPearl::Object* light = CreateLight(BlackPearl::LightType::PointLight, "Light");
		light->SetPosition({ 0.0,12.0,5.0 });
		/*m_Sword = CreateModel("assets/models/sword/OBJ/Big_Sword_OBJ.obj", "assets/shaders/pbr/PbrTexture.glsl", false, "Sword");
		std::shared_ptr<BlackPearl::Texture> SwordalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/models/sword/textures/Big Sword_Base_Color_Map.jpg"));
		std::shared_ptr<BlackPearl::Texture> SwordaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/models/sword/textures/Big Sword_AO_Map.jpg"));
		std::shared_ptr<BlackPearl::Texture> SwordroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/models/sword/textures/Big Sword_Roughness_Map.jpg"));
		std::shared_ptr<BlackPearl::Texture> SwordmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/models/sword/textures/Big Sword_Metalness.jpg"));
		std::shared_ptr<BlackPearl::Texture> SwordnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/models/sword/textures/Big Sword_Normal_Map.jpg"));
		std::shared_ptr<BlackPearl::Texture> SwordemissionTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::EmissionMap, "assets/models/sword/textures/Big Sword_Emission_Map.jpg"));

		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordalbedoTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordaoTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordroughnessTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordmentallicTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordnormalTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordemissionTexture);

		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetPBRTextureSamples(true);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetIsPBRObject(true);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTexturEmissionSamples(true);*/


		m_Cloudenderer->Init(m_Scene);

		BlackPearl::Renderer::Init();
	}

	virtual ~CloudRenderLayer() {

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_Cloudenderer->RenderDepthMap();
		//1) render scene to framebuffer
		m_LightPassFrameBuffer->Bind();

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
		
		m_LightPassFrameBuffer->UnBind();
		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//2) post process to render cloud
		//glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		//glClear(GL_COLOR_BUFFER_BIT);
		//m_Cloudenderer->ShowNoise3DTexture(m_CubeObj);
		//m_Cloudenderer->RenderScene(m_MainCamera, m_QuadObj, m_HDRPostProcessTexture);
		m_Cloudenderer->Render(m_MainCamera, m_QuadObj, m_Bunny, m_HDRPostProcessTexture);
		//glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//m_Cloudenderer->RenderCombineScene(m_MainCamera, m_QuadObj, m_HDRPostProcessTexture);
		//m_Cloudenderer->DrawLightSources(GetLightSources());
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
	BlackPearl::CloudRenderer* m_Cloudenderer;
	BlackPearl::PBRRenderer* m_PBRRenderer;
	BlackPearl::SkyboxRenderer* m_SkyboxRenderer;


};
