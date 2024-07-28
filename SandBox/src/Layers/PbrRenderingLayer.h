#pragma once
#pragma once
#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
using namespace BlackPearl;

class PbrRenderingLayer :public BlackPearl::Layer {
public:

	PbrRenderingLayer(const std::string& name)
		: Layer(name)
	{

		//m_CameraObj = CreateCamera();
		//auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		//cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		//m_CameraPosition = cameraComponent->GetPosition();
		//m_CameraRotation.Yaw = cameraComponent->Yaw();
		//m_CameraRotation.Pitch = cameraComponent->Pitch();
		m_SphereObj = CreateSphere(0.5, 64, 64);
		//Scene
		m_PBRRenderer = DBG_NEW BlackPearl::PBRRenderer();
		
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_PBRRenderer->GetShader());
		//std::shared_ptr<BlackPearl::Texture> albedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::TextureType::DiffuseMap, "assets/texture/pbr/rustSphere/rustediron2_basecolor.png"));
		//std::shared_ptr<BlackPearl::Texture> aoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::TextureType::AoMap, "assets/texture/pbr/rustSphere/rustediron2_ao.png"));
		//std::shared_ptr<BlackPearl::Texture> roughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::TextureType::RoughnessMap, "assets/texture/pbr/rustSphere/rustediron2_roughness.png"));
		//std::shared_ptr<BlackPearl::Texture> mentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::TextureType::MentallicMap, "assets/texture/pbr/rustSphere/rustediron2_metallic.png"));
		//std::shared_ptr<BlackPearl::Texture> normalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::TextureType::NormalMap, "assets/texture/pbr/rustSphere/rustediron2_normal.png"));
		BlackPearl::TextureDesc texDesc;
		texDesc.type = BlackPearl::TextureType::DiffuseMap;
		texDesc.path = "assets/texture/pbr/cobblestone/cobblestone-curved_2_albedo.png";
		TextureHandle albedoTexture = m_DeviceManager->GetDevice()->createTexture(texDesc);

		texDesc.type = BlackPearl::TextureType::AoMap;
		texDesc.path = "assets/texture/pbr/cobblestone/cobblestone-curved_2_ao.png";
		TextureHandle aoTexture = m_DeviceManager->GetDevice()->createTexture(texDesc);

		texDesc.type = BlackPearl::TextureType::RoughnessMap;
		texDesc.path = "assets/texture/pbr/cobblestone/cobblestone-curved_2_roughness.png";
		TextureHandle roughnessTexture = m_DeviceManager->GetDevice()->createTexture(texDesc);
		
		texDesc.type = BlackPearl::TextureType::MentallicMap;
		texDesc.path = "assets/texture/pbr/cobblestone/cobblestone-curved_2_metallic.png";
		TextureHandle mentallicTexture = m_DeviceManager->GetDevice()->createTexture(texDesc);
		
		texDesc.type = BlackPearl::TextureType::NormalMap;
		texDesc.path = "assets/texture/pbr/cobblestone/cobblestone-curved_2_normal-dx.png";
		TextureHandle normalTexture = m_DeviceManager->GetDevice()->createTexture(texDesc);

		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(albedoTexture);
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(normalTexture);
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(aoTexture);
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(roughnessTexture);
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(mentallicTexture);

		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1.0,0.0,0.0});
		m_SphereObjIron = CreateSphere(1.5, 64, 64);
		m_SphereObjRust = CreateSphere(1.5, 64, 64);
		m_SphereObjStone = CreateSphere(1.5, 64, 64);
		m_SphereObjPlastic = CreateSphere(1.5, 64, 64);


		//textures spheres

		TextureHandle RustalbedoTexture(DBG_NEW Texture({ TextureType::DiffuseMap, "assets/texture/pbr/rustSphere/rustediron2_basecolor.png" }));
		TextureHandle RustaoTexture(DBG_NEW Texture({ TextureType::AoMap, "assets/texture/pbr/rustSphere/rustediron2_ao.png" }));
		TextureHandle RustroughnessTexture(DBG_NEW Texture({ TextureType::RoughnessMap, "assets/texture/pbr/rustSphere/rustediron2_roughness.png" }));
		TextureHandle RustmentallicTexture(DBG_NEW Texture({ TextureType::MentallicMap, "assets/texture/pbr/rustSphere/rustediron2_metallic.png" }));
		TextureHandle RustnormalTexture(DBG_NEW Texture({ TextureType::NormalMap, "assets/texture/pbr/rustSphere/rustediron2_normal.png" }));

		TextureHandle IronalbedoTexture(DBG_NEW Texture({ TextureType::DiffuseMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_basecolor.png" }));
		TextureHandle IronaoTexture(DBG_NEW Texture({ TextureType::AoMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_ao.png" }));
		TextureHandle IronroughnessTexture(DBG_NEW Texture({ TextureType::RoughnessMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_roughness.png" }));
		TextureHandle IronmentallicTexture(DBG_NEW Texture({ TextureType::MentallicMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_metallic.png" }));
		TextureHandle IronnormalTexture(DBG_NEW Texture({ TextureType::NormalMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_normal.png" }));

		TextureHandle StonealbedoTexture(DBG_NEW Texture({ TextureType::DiffuseMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_albedo.png" }));
		TextureHandle StoneaoTexture(DBG_NEW Texture({ TextureType::AoMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_ao.png" }));
		TextureHandle StoneroughnessTexture(DBG_NEW Texture({ TextureType::RoughnessMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_roughness.png" }));
		TextureHandle StonementallicTexture(DBG_NEW Texture({ TextureType::MentallicMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_metallic.png" }));
		TextureHandle StonenormalTexture(DBG_NEW Texture({ TextureType::NormalMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_normal-dx.png" }));
		
		TextureHandle PlasticalbedoTexture(DBG_NEW Texture({ TextureType::DiffuseMap, "assets/texture/pbr/plasticSphere/scuffed-plastic4-alb.png" }));
		TextureHandle PlasticaoTexture(DBG_NEW Texture({ TextureType::AoMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-ao.png" }));
		TextureHandle PlasticroughnessTexture(DBG_NEW Texture({ TextureType::RoughnessMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-rough.png" }));
		TextureHandle PlasticmentallicTexture(DBG_NEW Texture({ TextureType::MentallicMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-metal.png" }));
		TextureHandle PlasticnormalTexture(DBG_NEW Texture({ TextureType::NormalMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-normal.png" }));

		m_SphereObjIron->GetComponent<MeshRenderer>()->SetTextures(IronnormalTexture);
		m_SphereObjIron->GetComponent<MeshRenderer>()->SetTextures(IronalbedoTexture);
		m_SphereObjIron->GetComponent<MeshRenderer>()->SetTextures(IronaoTexture);
		m_SphereObjIron->GetComponent<MeshRenderer>()->SetTextures(IronroughnessTexture);
		m_SphereObjIron->GetComponent<MeshRenderer>()->SetTextures(IronmentallicTexture);
		m_SphereObjIron->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
		m_SphereObjIron->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
		m_SphereObjIron->GetComponent<Transform>()->SetPosition({ 10,0,0 });

		m_SphereObjRust->GetComponent<MeshRenderer>()->SetTextures(RustnormalTexture);
		m_SphereObjRust->GetComponent<MeshRenderer>()->SetTextures(RustalbedoTexture);
		m_SphereObjRust->GetComponent<MeshRenderer>()->SetTextures(RustaoTexture);
		m_SphereObjRust->GetComponent<MeshRenderer>()->SetTextures(RustroughnessTexture);
		m_SphereObjRust->GetComponent<MeshRenderer>()->SetTextures(RustmentallicTexture);
		m_SphereObjRust->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
		m_SphereObjRust->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
		m_SphereObjRust->GetComponent<Transform>()->SetPosition({ 5,0,0 });

		m_SphereObjStone->GetComponent<MeshRenderer>()->SetTextures(StonenormalTexture);
		m_SphereObjStone->GetComponent<MeshRenderer>()->SetTextures(StonealbedoTexture);
		m_SphereObjStone->GetComponent<MeshRenderer>()->SetTextures(StoneaoTexture);
		m_SphereObjStone->GetComponent<MeshRenderer>()->SetTextures(StoneroughnessTexture);
		m_SphereObjStone->GetComponent<MeshRenderer>()->SetTextures(StonementallicTexture);
		m_SphereObjStone->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
		m_SphereObjStone->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
		m_SphereObjStone->GetComponent<Transform>()->SetPosition({ -5,0,0 });

		m_SphereObjPlastic->GetComponent<MeshRenderer>()->SetTextures(PlasticnormalTexture);
		m_SphereObjPlastic->GetComponent<MeshRenderer>()->SetTextures(PlasticalbedoTexture);
		m_SphereObjPlastic->GetComponent<MeshRenderer>()->SetTextures(PlasticaoTexture);
		m_SphereObjPlastic->GetComponent<MeshRenderer>()->SetTextures(PlasticroughnessTexture);
		m_SphereObjPlastic->GetComponent<MeshRenderer>()->SetTextures(PlasticmentallicTexture);
		m_SphereObjPlastic->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
		m_SphereObjPlastic->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
		m_SphereObjPlastic->GetComponent<Transform>()->SetPosition({ -10.0,0,0 });


		Object* light = CreateLight(LightType::PointLight, "Light");
		light->SetPosition({ 0.0,0.0,5.0 });
		m_Sword = CreateModel("assets/models/sword/OBJ/Big_Sword_OBJ.obj", "assets/shaders/pbr/PbrTexture.glsl", false, "Sword");

		BlackPearl::IDevice* device = m_DeviceManager->GetDevice();

		BlackPearl::TextureHandle SwordalbedoTexture = device->createTexture({ BlackPearl::DiffuseMap, "assets/models/sword/textures/Big Sword_Base_Color_Map.jpg" });
		BlackPearl::TextureHandle SwordaoTexture = device->createTexture({ BlackPearl::TextureType::AoMap, "assets/models/sword/textures/Big Sword_AO_Map.jpg" });
		BlackPearl::TextureHandle SwordroughnessTexture = device->createTexture({ BlackPearl::TextureType::RoughnessMap, "assets/models/sword/textures/Big Sword_Roughness_Map.jpg" });
		BlackPearl::TextureHandle SwordmentallicTexture = device->createTexture({ BlackPearl::TextureType::MentallicMap, "assets/models/sword/textures/Big Sword_Metalness.jpg" });
		BlackPearl::TextureHandle SwordnormalTexture = device->createTexture({ BlackPearl::TextureType::NormalMap, "assets/models/sword/textures/Big Sword_Normal_Map.jpg" });
		BlackPearl::TextureHandle SwordemissionTexture = device->createTexture({ BlackPearl::TextureType::EmissionMap, "assets/models/sword/textures/Big Sword_Emission_Map.jpg" });

		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordalbedoTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordaoTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordroughnessTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordmentallicTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordnormalTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordemissionTexture);

		m_Sword->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
		m_Sword->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
		m_Sword->GetComponent<MeshRenderer>()->SetTexturEmissionSamples(true);




		Renderer::Init();
	}

	virtual ~PbrRenderingLayer() {

		DestroyObjects();

	}
	void OnUpdate(Timestep ts) override {


		InputCheck(ts);

		// render
		RenderCommand::SetClearColor(m_BackgroundColor);
		Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<PerspectiveCamera>()), *GetLightSources());

		//m_PBRRenderer->GetShader()->SetUniformVec3f("u_albedo", { 0.5f, 0.0f, 0.0f });
		//m_PBRRenderer->GetShader()->SetUniform1f("u_ao", 1.0f);

		//for (int row = 0; row < m_Rows; row++) {
		//	for (int col = 0; col < m_Colums; col++) {

		//		m_SphereObj->GetComponent<Transform>()->SetPosition({
		//			(col - (m_Colums / 2)) * m_Spacing,
		//			(row - (m_Rows / 2)) * m_Spacing,
		//			0.0f });
		//		m_PBRRenderer->GetShader()->SetUniform1f("u_metallic", 2.0*(float)row / (float)m_Rows);
		//		m_PBRRenderer->GetShader()->SetUniform1f("u_roughness", glm::clamp((float)col / (float)m_Colums, 0.05f, 1.0f));
		//		m_PBRRenderer->Render(m_SphereObj);
		//	}
		//}
		m_PBRRenderer->Render(m_Sword);

		m_PBRRenderer->DrawLightSources(GetLightSources());
	}

	void OnAttach() override {


	}
	
private:
	//Scene
	std::vector<Object*> m_LightObjs;

	Object* m_IronManObj;
	Object* m_QuadObj;
	Object* m_PlaneObj;
	Object* m_CubeObj;
	Object* m_SphereObj;

	Object* m_Sword;

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

	Object* m_SphereObjIron = nullptr;
	Object* m_SphereObjRust = nullptr;
	Object* m_SphereObjStone = nullptr;
	Object* m_SphereObjPlastic = nullptr;


	//Renderer
	PBRRenderer* m_PBRRenderer;
	
};
