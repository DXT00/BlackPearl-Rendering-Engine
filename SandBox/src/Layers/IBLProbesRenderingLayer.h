#pragma once

#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class IBLProbesRenderingLayer :public BlackPearl::Layer {
public:

	IBLProbesRenderingLayer(const std::string& name, BlackPearl::ObjectManager* objectManager)
		: Layer(name, objectManager)
	{

		

		///*ProbesCamera is used to render probes'environmentMap*/
		//m_ProbesCamera = CreateCamera();
		//m_ProbesCamera->GetComponent<BlackPearl::PerspectiveCamera>()->SetFov(90.0f);
		///* make sure aspectRadio = 1 */
		//m_ProbesCamera->GetComponent<BlackPearl::PerspectiveCamera>()->SetWidth(512);
		//m_ProbesCamera->GetComponent<BlackPearl::PerspectiveCamera>()->SetHeight(512);

		

		//Shader
		m_BackGroundShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/ibl/background.glsl"));
		//Scene
		m_SphereObj = CreateSphere(0.5, 64, 64);
		m_SphereObjIron = CreateSphere(0.5, 64, 64);
		m_SphereObjRust = CreateSphere(0.5, 64, 64);
		m_SphereObjStone = CreateSphere(0.5, 64, 64);
		m_SphereObjPlastic = CreateSphere(0.5, 64, 64);

		/* probe's CubeObj and quad for BrdfLUTMap */
		m_BrdfLUTQuadObj = CreateQuad("assets/shaders/ibl/brdf.glsl","");

		/* create probes */
		BlackPearl::LightProbe *probe1 = CreateLightProbe();
		probe1->SetPosition({ 0.0,0.0,-3.0 });

		BlackPearl::LightProbe* probe2 = CreateLightProbe();
		probe2->SetPosition({3.0,0.0,0.0});
		BlackPearl::LightProbe* probe3 = CreateLightProbe();
		probe3->SetPosition({ -3.0,0.0,0.0 });

		m_LightProbes.push_back(probe1);
		m_LightProbes.push_back(probe2);
		m_LightProbes.push_back(probe3);

		BlackPearl::Renderer::Init();
		//glDepthFunc(GL_LEQUAL);
		/*Renderer*/
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();
		m_IBLProbesRenderer = DBG_NEW BlackPearl::IBLProbesRenderer();
		/*create skybox */
		/*notice: draw skybox before anything else!*/
		//m_SkyBoxObj1 = CreateSkyBox(
		//	{ "assets/skybox/skybox/right.jpg",
		//	 "assets/skybox/skybox/left.jpg",
		//	 "assets/skybox/skybox/top.jpg",
		//	 "assets/skybox/skybox/bottom.jpg",
		//	 "assets/skybox/skybox/front.jpg",
		//	 "assets/skybox/skybox/back.jpg",
		//	});

		//m_BackGroundObjsList.push_back(m_SkyBoxObj1);
		/*create model*/
		BlackPearl::Object *deer=  CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl");
		deer->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(0.005));
		deer->GetComponent<BlackPearl::Transform>()->SetPosition({0.0f,-1.0f,0.0f});
		m_BackGroundObjsList.push_back(deer);

		BlackPearl::Object* ironMan = CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/IronMan.glsl");
		ironMan->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(0.005));
		ironMan->GetComponent<BlackPearl::Transform>()->SetPosition({ 1.5f,-1.0f,0.0f });
		m_BackGroundObjsList.push_back(ironMan);


		BlackPearl::Object* cube = CreateCube();
		cube->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f,-3.0f,0.0f });
		m_BackGroundObjsList.push_back(cube);

		BlackPearl::Object* cube1 = CreateCube();
		cube1->GetComponent<BlackPearl::Transform>()->SetPosition({ -2.0f,-3.0f,0.0f });
		m_BackGroundObjsList.push_back(cube1);

		/*create pointlights*/
		BlackPearl::Object* light = CreateLight(BlackPearl::LightType::PointLight);
		light->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0,0.0,3.0 });

		//textures spheres
		std::shared_ptr<BlackPearl::Texture> RustalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/rustSphere/rustediron2_basecolor.png"));
		std::shared_ptr<BlackPearl::Texture> RustaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/rustSphere/rustediron2_ao.png"));
		std::shared_ptr<BlackPearl::Texture> RustroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/rustSphere/rustediron2_roughness.png"));
		std::shared_ptr<BlackPearl::Texture> RustmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/rustSphere/rustediron2_metallic.png"));
		std::shared_ptr<BlackPearl::Texture> RustnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/rustSphere/rustediron2_normal.png"));


		std::shared_ptr<BlackPearl::Texture> IronalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_basecolor.png"));
		std::shared_ptr<BlackPearl::Texture> IronaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_ao.png"));
		std::shared_ptr<BlackPearl::Texture> IronroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_roughness.png"));
		std::shared_ptr<BlackPearl::Texture> IronmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_metallic.png"));
		std::shared_ptr<BlackPearl::Texture> IronnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_normal.png"));

		std::shared_ptr<BlackPearl::Texture> StonealbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_albedo.png"));
		std::shared_ptr<BlackPearl::Texture> StoneaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_ao.png"));
		std::shared_ptr<BlackPearl::Texture> StoneroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_roughness.png"));
		std::shared_ptr<BlackPearl::Texture> StonementallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_metallic.png"));
		std::shared_ptr<BlackPearl::Texture> StonenormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_normal-dx.png"));

		std::shared_ptr<BlackPearl::Texture> PlasticalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/plasticSphere/scuffed-plastic4-alb.png"));
		std::shared_ptr<BlackPearl::Texture> PlasticaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-ao.png"));
		std::shared_ptr<BlackPearl::Texture> PlasticroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-rough.png"));
		std::shared_ptr<BlackPearl::Texture> PlasticmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-metal.png"));
		std::shared_ptr<BlackPearl::Texture> PlasticnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-normal.png"));

		m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronnormalTexture);
		m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronalbedoTexture);
		m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronaoTexture);
		m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronroughnessTexture);
		m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronmentallicTexture);

		m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustnormalTexture);
		m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustalbedoTexture);
		m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustaoTexture);
		m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustroughnessTexture);
		m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustmentallicTexture);

		m_SphereObjRust->GetComponent<BlackPearl::Transform>()->SetPosition({ 1.5,0,0 });

		m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonenormalTexture);
		m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonealbedoTexture);
		m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StoneaoTexture);
		m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StoneroughnessTexture);
		m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonementallicTexture);

		m_SphereObjStone->GetComponent<BlackPearl::Transform>()->SetPosition({ -1.5,0,0 });

		m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticnormalTexture);
		m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticalbedoTexture);
		m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticaoTexture);
		m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticroughnessTexture);
		m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticmentallicTexture);

		m_SphereObjPlastic->GetComponent<BlackPearl::Transform>()->SetPosition({ -3.0,0,0 });
		
		m_SphereObjsList.push_back(m_SphereObjIron);
		m_SphereObjsList.push_back(m_SphereObjStone);
		m_SphereObjsList.push_back(m_SphereObjPlastic);
		m_SphereObjsList.push_back(m_SphereObjRust);


		
		/*Draw CubeMap from hdrMap and Create environment IrrdianceMap*/
		m_IBLProbesRenderer->Init(m_BrdfLUTQuadObj,  *GetLightSources(), m_BackGroundObjsList, m_LightProbes);
		m_IBLProbesRenderer->Render(*GetLightSources(), m_BackGroundObjsList, m_LightProbes);

		//glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);

	}

	virtual ~IBLProbesRenderingLayer() {

		DestroyObjects();
		delete m_BasicRenderer;
		delete m_IBLProbesRenderer;

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		/*render scene*/
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		//
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_U)) {

			/*IBLProbes rendering*/
			m_IBLProbesRenderer->Render(*GetLightSources(), m_BackGroundObjsList, m_LightProbes);
		}
		

		//glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		//glDepthMask(GL_FALSE);
		m_BasicRenderer->RenderScene(m_BackGroundObjsList, GetLightSources());
		m_IBLProbesRenderer->RenderProbes(m_LightProbes);
		//glDepthFunc(GL_LEQUAL);

		//m_IBLProbesRenderer->DrawObject(m_SkyBoxObj1);
		////glDepthMask(GL_TRUE);
		//glDepthFunc(GL_LESS);


		
		m_IBLProbesRenderer->RenderSpecularObjects(*GetLightSources(), m_SphereObjsList, m_LightProbes);



	}

	void OnAttach() override {


	}
	
private:
	//Scene
	
	//BlackPearl::Object* m_DebugQuad = nullptr;

	BlackPearl::Object* m_SphereObj = nullptr;
	BlackPearl::Object* m_SphereObjIron = nullptr;
	BlackPearl::Object* m_SphereObjRust = nullptr;
	BlackPearl::Object* m_SphereObjStone = nullptr;
	BlackPearl::Object* m_SphereObjPlastic = nullptr;
	std::vector<BlackPearl::Object*> m_SphereObjsList;
	std::vector<BlackPearl::Object*> m_BackGroundObjsList;
	BlackPearl::Object* m_SkyBoxObj1 = nullptr;

	BlackPearl::Object* m_BrdfLUTQuadObj = nullptr;
	int m_Rows = 4;
	int m_Colums = 4;
	float m_Spacing = 1.5;

	

	//Shader
	std::shared_ptr<BlackPearl::Shader> m_BackGroundShader;
	//std::shared_ptr<BlackPearl::Shader> m_DebugQuadShader;
	//Renderer
	BlackPearl::IBLProbesRenderer* m_IBLProbesRenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;
	/* Probes */
	std::vector<BlackPearl::LightProbe*> m_LightProbes;

};
