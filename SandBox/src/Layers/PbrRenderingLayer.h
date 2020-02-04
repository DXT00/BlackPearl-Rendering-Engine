#pragma once
#pragma once
#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class PbrRenderingLayer :public BlackPearl::Layer {
public:

	PbrRenderingLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
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
		//std::shared_ptr<BlackPearl::Texture> albedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/rustSphere/rustediron2_basecolor.png"));
		//std::shared_ptr<BlackPearl::Texture> aoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/rustSphere/rustediron2_ao.png"));
		//std::shared_ptr<BlackPearl::Texture> roughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/rustSphere/rustediron2_roughness.png"));
		//std::shared_ptr<BlackPearl::Texture> mentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/rustSphere/rustediron2_metallic.png"));
		//std::shared_ptr<BlackPearl::Texture> normalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/rustSphere/rustediron2_normal.png"));

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

		BlackPearl::Renderer::Init();
	}

	virtual ~PbrRenderingLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		//m_PBRRenderer->GetShader()->SetUniformVec3f("u_albedo", { 0.5f, 0.0f, 0.0f });
	//	m_PBRRenderer->GetShader()->SetUniform1f("u_ao", 1.0f);

		for (int row = 0; row < m_Rows; row++) {
			for (int col = 0; col < m_Colums; col++) {

				m_SphereObj->GetComponent<BlackPearl::Transform>()->SetPosition({
					(col - (m_Colums / 2)) * m_Spacing,
					(row - (m_Rows / 2)) * m_Spacing,
					0.0f });
				//m_PBRRenderer->GetShader()->SetUniform1f("u_metallic", 2.0*(float)row / (float)m_Rows);
				//m_PBRRenderer->GetShader()->SetUniform1f("u_roughness", glm::clamp((float)col / (float)m_Colums, 0.05f, 1.0f));
				m_PBRRenderer->Render(m_SphereObj);
			}
		}


	}

	void OnAttach() override {


	}
	
private:
	//Scene
	std::vector<BlackPearl::Object*> m_LightObjs;

	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_SphereObj;

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



	//Renderer
	BlackPearl::PBRRenderer* m_PBRRenderer;
	
};
