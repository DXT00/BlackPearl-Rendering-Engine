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

		
	}
	void OnSetup() override {

		m_SphereObj = CreateSphere(0.5, 64, 64);
		//Scene
		m_PBRRenderer = DBG_NEW BlackPearl::PBRRenderer();

		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_PBRRenderer->GetShader());
		
		BlackPearl::TextureDesc texDesc;
		texDesc.format = BlackPearl::Format::RGBA8_UNORM;
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

		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1.0,0.0,0.0 });
		m_SphereObjIron = LoadStaticBackGroundObject("SphereIron");
		m_SphereObjRust = LoadStaticBackGroundObject("SphereRust");
		m_SphereObjStone = LoadStaticBackGroundObject("SphereStone");
		m_SphereObjPlastic = LoadStaticBackGroundObject("SpherePlastic");

		Object* light = CreateLight(LightType::PointLight, "Light");
		light->SetPosition({ 0.0,0.0,5.0 });
		m_Sword = LoadStaticBackGroundObject("Sword");

		BlackPearl::IDevice* device = m_DeviceManager->GetDevice();

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
