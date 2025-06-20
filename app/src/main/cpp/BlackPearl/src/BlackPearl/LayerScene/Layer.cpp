#include "pch.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/LayerScene/Layer.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/LightProbeComponent/LightProbeComponent.h"
#include "BlackPearl/Component/TerrainComponent/TerrainComponent.h"
#include "BlackPearl/Renderer/Model/Model.h"
//#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl\ImGui\imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
//#include "BlackPearl/Renderer/MasterRenderer/IBLRenderer.h"
//#include "BlackPearl/Renderer/MasterRenderer/IBLProbesRenderer.h"
//#include "BlackPearl/Renderer/MasterRenderer/ShadowMapPointLightRenderer.h"
//#include "BlackPearl/Renderer/MasterRenderer/GBufferRenderer.h"
//#include "BlackPearl/Renderer/MasterRenderer/VoxelConeTracingRenderer.h"
//#include "BlackPearl/Renderer/MasterRenderer/VoxelConeTracingDeferredRenderer.h"
//#include "BlackPearl/Renderer/MasterRenderer/VoxelConeTracingSVORenderer.h"
//#include "BlackPearl/Renderer/MasterRenderer/CloudRenderer.h"
#include "BlackPearl/Application.h"
#ifdef GE_API_D3D12
#include "BlackPearl/Renderer/Buffer/D3D12Buffer/D3D12Buffer.h"
#endif
using namespace BlackPearl::math;

#include "hlsl/core/material_cb.h"
namespace BlackPearl {
	static int buttonNum = 0;
	void Layer::OnImguiRender()
	{


	}

	void Layer::LoadScene(const std::string demoScene)
	{
		if (demoScene == "CornellScene")
			LoadCornellScene();
		else if (demoScene == "SpheresScene")
			LoadSpheresScene();
		else if (demoScene == "SpheresSpecularProbeScene")
			LoadSpheresSpecularProbeScene();
		else if (demoScene == "CubesScene")
			LoadCubesScene();
		else if (demoScene == "SwordScene")
			LoadSwordScene();
		else if (demoScene == "Church")
			LoadChurchScene();
		else if (demoScene == "D3D12Models")
			LoadD3D12ModelScene();


	}

	void Layer::LoadCornellScene()
	{
		/*create pointlights*/
		Object* light = CreateLight(LightType::PointLight);
		light->GetComponent<Transform>()->SetInitPosition({ 0.0,0.6,3.6 });
		light->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);

		Object* cube1 = CreateCube();
		Object* cube2 = CreateCube();
		Object* cube3 = CreateCube();
		Object* cube4 = CreateCube();
		Object* cube5 = CreateCube();

		//cube1->GetComponent<Transform>()->SetScale({ 20.0f,20.0f,20.0f });

		cube1->GetComponent<Transform>()->SetInitPosition({ 2.0f,1.0f,3.0f });
		cube2->GetComponent<Transform>()->SetInitPosition({ 0.0f,1.0f,1.0f });
		cube3->GetComponent<Transform>()->SetInitPosition({ -2.0f,1.0f,3.0f });
		cube4->GetComponent<Transform>()->SetInitPosition({ 0.0f,3.0f,3.0f });
		cube5->GetComponent<Transform>()->SetInitPosition({ 0.0f,-1.0f,3.0f });


		cube1->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		cube2->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		cube3->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		cube4->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		cube5->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);

		//cube1->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		//cube2->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		//cube3->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		//cube4->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		//cube5->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });

		cube1->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 0,0.294f,1.0f });
		cube2->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1,0.267f,0 });
		cube3->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1,1,1 });
		cube4->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 0.204f,0,1 });
		cube5->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1,0,0 });
		m_BackGroundObjsList.push_back(cube1);
		m_BackGroundObjsList.push_back(cube2);
		m_BackGroundObjsList.push_back(cube3);
		m_BackGroundObjsList.push_back(cube4);
		m_BackGroundObjsList.push_back(cube5);

		Object* deer = CreateModel("assets/models/deer/Deer.obj", "assets/shaders/glsl/IronMan.glsl", false, "Deer");
		deer->GetComponent<Transform>()->SetInitScale(glm::vec3(0.003));
		deer->GetComponent<Transform>()->SetInitPosition({ -0.5f,0.0f,2.5f });
		deer->GetComponent<Transform>()->SetInitRotation({ 0.0f,68.0f,0.0f });
		deer->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(deer);

		Object* bunny = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/glsl/IronMan.glsl", false, "Bunny");
		bunny->GetComponent<Transform>()->SetInitScale(glm::vec3(0.5));
		bunny->GetComponent<Transform>()->SetInitPosition({ 0.6f,0.0f,3.0f });
		bunny->GetComponent<Transform>()->SetInitRotation({ 0.0f,-30.0f,0.0f });
		bunny->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(bunny);

		m_ShadowObjsList.push_back(deer);
		m_ShadowObjsList.push_back(bunny);
		m_ShadowObjsList.push_back(cube1);
		m_ShadowObjsList.push_back(cube2);
		m_ShadowObjsList.push_back(cube3);
		m_ShadowObjsList.push_back(cube4);
		m_ShadowObjsList.push_back(cube5);

	}
	
	void Layer::LoadCornellScene1()
	{
		/*create pointlights*/
		Object* light = CreateLight(LightType::PointLight);
		light->GetComponent<Transform>()->SetInitPosition({ 0.0,1.25,9.0 });
		light->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);

		Object* deer = CreateModel("assets/models/deer/Deer.obj", "assets/shaders/glsl/IronMan.glsl", false, "Deer");
		deer->GetComponent<Transform>()->SetInitScale(glm::vec3(0.003));
		//deer->GetComponent<Transform>()->SetPosition({ -0.5f,0.0f,2.5f });
		deer->GetComponent<Transform>()->SetInitPosition({ -0.5f,-1.5f,-0.5f });
		deer->GetComponent<Transform>()->SetInitRotation({ 0.0f,68.0f,0.0f });
		deer->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(deer);
		m_ShadowObjsList.push_back(deer);

		Object* bunny = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/glsl/IronMan.glsl", false, "Bunny");
		bunny->GetComponent<Transform>()->SetInitScale(glm::vec3(0.5));
		//bunny->GetComponent<Transform>()->SetPosition({ 0.6f,0.0f,3.0f });
		bunny->GetComponent<Transform>()->SetInitPosition({ 0.6f,-1.5f,-0.0f });
		bunny->GetComponent<Transform>()->SetInitRotation({ 0.0f,-30.0f,0.0f });
		bunny->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(bunny);
		m_ShadowObjsList.push_back(bunny);

		Object* cube1 = CreateCube();
		Object* cube2 = CreateCube();
		Object* cube3 = CreateCube();
		Object* cube4 = CreateCube();
		Object* cube5 = CreateCube();

		//cube1->GetComponent<Transform>()->SetScale({ 20.0f,20.0f,20.0f });

		//cube1->GetComponent<Transform>()->SetPosition({ 2.0f,1.0f,3.0f });
		//cube2->GetComponent<Transform>()->SetPosition({ 0.0f,1.0f,1.0f });
		//cube3->GetComponent<Transform>()->SetPosition({ -2.0f,1.0f,3.0f });
		//cube4->GetComponent<Transform>()->SetPosition({ 0.0f,3.0f,3.0f });
		//cube5->GetComponent<Transform>()->SetPosition({ 0.0f,-1.0f,3.0f });
		cube1->GetComponent<Transform>()->SetInitPosition({ 2.0f,-0.5f,-0.0f });
		cube2->GetComponent<Transform>()->SetInitPosition({ 0.0f,-0.5f,-2.0f });
		cube3->GetComponent<Transform>()->SetInitPosition({ -2.0f,-0.5f,-0.0f });
		cube4->GetComponent<Transform>()->SetInitPosition({ 0.0f,1.5f,-0.0f });
		cube5->GetComponent<Transform>()->SetInitPosition({ 0.0f,-2.5f,-0.0f });

		cube1->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		cube2->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		cube3->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		cube4->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		cube5->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);

		/*cube1->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube2->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube3->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube4->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube5->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });*/

		cube1->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 0,0.294f,1.0f });
		cube2->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1,0.267f,0 });
		cube3->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1,1,1 });
		cube4->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 0.204f,0,1 });
		cube5->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ 1,0,0 });
		m_BackGroundObjsList.push_back(cube1);
		m_BackGroundObjsList.push_back(cube2);
		m_BackGroundObjsList.push_back(cube3);
		m_BackGroundObjsList.push_back(cube4);
		m_BackGroundObjsList.push_back(cube5);

		m_ShadowObjsList.push_back(cube1);
		m_ShadowObjsList.push_back(cube2);
		m_ShadowObjsList.push_back(cube3);
		m_ShadowObjsList.push_back(cube4);
		m_ShadowObjsList.push_back(cube5);


	}
	
	void Layer::LoadChurchScene()
	{
		Object* church = CreateModel("assets/models/crytek-sponza/sponza.obj", "assets/shaders/glsl/IronMan.glsl", false, "Church");

		//Object* church = CreateModel("assets/models/sponza_obj/sponza.obj", "assets/shaders/IronMan.glsl", false, "Church");
		church->GetComponent<Transform>()->SetInitScale(glm::vec3(0.006));//0.02
		church->GetComponent<Transform>()->SetInitPosition({ 0.0f,0.0f,10.0f });
		church->GetComponent<Transform>()->SetInitRotation({ 0.0f,-90.0f,0.0f });
		//church->GetComponent<Transform>()->SetScale({ 0.1f,0.1f,0.1f });

		church->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		church->GetComponent<MeshRenderer>()->SetTextureHeightSamples(true);
		church->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
		church->GetComponent<MeshRenderer>()->SetTextureSpecularSamples(true);
		church->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);

		m_BackGroundObjsList.push_back(church);


		BlackPearl::Object* light = CreateLight(LightType::PointLight);
		light->GetComponent<Transform>()->SetInitPosition({ 0.0,4.0,10.0 });
		light->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);
		light->GetComponent<PointLight>()->UpdateMesh({ {0,0,0} ,{1,1,1},{0,0,0},{0,0,0},1.0 });


	}

	void Layer::LoadSpheresScene()
	{
		//Scene
		Object* light = CreateLight(LightType::PointLight);
		light->GetComponent<Transform>()->SetInitPosition({ 0.0,-1.0,0.0 });
		light->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);

		Object* sphereObjIron = LoadStaticBackGroundObject("SphereIron");
		Object* sphereObjRust = LoadStaticBackGroundObject("SphereRust");
		Object* sphereObjStone = LoadStaticBackGroundObject("SphereStone");
		Object* sphereObjPlastic = LoadStaticBackGroundObject("SpherePlastic");
		//Object* cube = LoadStaticBackGroundObject("WoodCube");
		//textures spheres
		IDevice* device = m_DeviceManager->GetDevice();
		
		sphereObjIron->GetComponent<Transform>()->SetInitPosition({ 10,0,0 });

		sphereObjRust->GetComponent<Transform>()->SetInitPosition({ 5,0,0 });
		
		sphereObjStone->GetComponent<Transform>()->SetInitPosition({ -5,0,0 });

		sphereObjPlastic->GetComponent<Transform>()->SetInitPosition({ -10.0,0,0 });
		
		/*cube->GetComponent<Transform>()->SetInitPosition({ -2.0f,-2.5f,0.0f });
		cube->GetComponent<Transform>()->SetInitScale({ 16.0f,0.5f,16.0f });*/
		
		m_ShadowObjsList.push_back(sphereObjIron);
		m_ShadowObjsList.push_back(sphereObjStone);
		m_ShadowObjsList.push_back(sphereObjPlastic);
		m_ShadowObjsList.push_back(sphereObjRust);
		//m_ShadowObjsList.push_back(cube);

	}

	void Layer::LoadSpheresSpecularProbeScene()
	{
		//Scene
		Object* light = CreateLight(LightType::PointLight);
		light->GetComponent<Transform>()->SetInitPosition({ 0.0,-1.0,0.0 });
		light->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);

		IDevice* device = m_DeviceManager->GetDevice();

		Object* sphereObjIron = LoadStaticBackGroundObject("SphereIron");
		Object* sphereObjRust = LoadStaticBackGroundObject("SphereRust");
		Object* sphereObjStone = LoadStaticBackGroundObject("SphereStone");
		Object* sphereObjPlastic = LoadStaticBackGroundObject("SpherePlastic");
		Object* cube = LoadStaticBackGroundObject("WoodCube");
		
		sphereObjIron->GetComponent<Transform>()->SetInitPosition({ 10,0,0 });

		sphereObjRust->GetComponent<Transform>()->SetInitPosition({ 5,0,0 });

		sphereObjStone->GetComponent<Transform>()->SetInitPosition({ -5,0,0 });

		sphereObjPlastic->GetComponent<Transform>()->SetInitPosition({ -10.0,0,0 });

		cube->GetComponent<Transform>()->SetInitPosition({ -2.0f,-2.5f,0.0f });
		cube->GetComponent<Transform>()->SetInitScale({ 16.0f,0.5f,16.0f });


		BlackPearl::Object* specularProbe = CreateLightProbe(BlackPearl::ProbeType::REFLECTION_PROBE);
		cube->AddChildObj(specularProbe);
		specularProbe->GetComponent<Transform>()->SetInitPosition(cube->GetComponent<Transform>()->GetPosition());
		specularProbe->GetComponent<BlackPearl::LightProbe>()->AddExcludeObjectId(cube->GetId().id);

		BlackPearl::Object* specularProbe1 = CreateLightProbe(BlackPearl::ProbeType::REFLECTION_PROBE);
		specularProbe1->GetComponent<Transform>()->SetInitPosition(sphereObjIron->GetComponent<Transform>()->GetPosition());
		sphereObjIron->AddChildObj(specularProbe1);
		specularProbe1->GetComponent<BlackPearl::LightProbe>()->AddExcludeObjectId(sphereObjIron->GetId().id);

		BlackPearl::Object* specularProbe2 = CreateLightProbe(BlackPearl::ProbeType::REFLECTION_PROBE);
		specularProbe2->GetComponent<Transform>()->SetInitPosition(sphereObjRust->GetComponent<Transform>()->GetPosition());
		sphereObjRust->AddChildObj(specularProbe2);
		specularProbe2->GetComponent<BlackPearl::LightProbe>()->AddExcludeObjectId(sphereObjRust->GetId().id);

		BlackPearl::Object* specularProbe3 = CreateLightProbe(BlackPearl::ProbeType::REFLECTION_PROBE);
		specularProbe3->GetComponent<Transform>()->SetInitPosition(sphereObjStone->GetComponent<Transform>()->GetPosition());
		sphereObjStone->AddChildObj(specularProbe3);
		specularProbe3->GetComponent<BlackPearl::LightProbe>()->AddExcludeObjectId(sphereObjStone->GetId().id);


		m_ShadowObjsList.push_back(sphereObjIron);
		m_ShadowObjsList.push_back(sphereObjStone);
		m_ShadowObjsList.push_back(sphereObjPlastic);
		m_ShadowObjsList.push_back(sphereObjRust);
		m_ShadowObjsList.push_back(cube);
	}

	void Layer::LoadSwordScene()
	{
		IDevice* device = m_DeviceManager->GetDevice();
		Object* sword = LoadStaticBackGroundObject("Sword");
		m_ShadowObjsList.push_back(sword);


	}

	void Layer::LoadCubesScene()
	{
		float width = 16;
		float height = width;

		float num = 6;

		float cubeSize = width / num;
		for (int i = 0; i < num; i++)
		{
			for (int j = 0; j < num; j++) {

				for (int k = 0; k < num; k++) {
					Object* cube = CreateCube();
					cube->GetComponent<Transform>()->SetInitPosition({ cubeSize * i,cubeSize * j,cubeSize * k });
					//cube->GetComponent<Transform>()->SetRotation({0.0f,45.0f, 0.0f});

					cube->GetComponent<Transform>()->SetInitScale({ 0.5f * cubeSize, 0.5f * cubeSize, 0.5f * cubeSize });
					//std::shared_ptr<Texture> cubeTexture(DBG_NEW Texture(TextureType::DiffuseMap, "assets/texture/wood.png"));
					cube->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ (1.0f / num) * i,(1.0f / num) * j,(1.0f / num) * k });

					//cube->GetComponent<MeshRenderer>()->SetTextureSamples(false);
					cube->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
					m_BackGroundObjsList.push_back(cube);
				}
			}

		}
	}

	std::vector<Object*> Layer::LoadCubesScene1(int cubeNum, glm::vec3 pos)
	{
		std::vector<Object*> objs;
		float width = 16;
		float height = width;

		float num = cubeNum;

		float cubeSize = width / num;
		for (int i = 0; i < num; i++)
		{
			for (int j = 0; j < num; j++) {

				for (int k = 0; k < num; k++) {
					Object* cube = CreateCube();
					cube->GetComponent<Transform>()->SetInitPosition(pos + glm::vec3(cubeSize * i, cubeSize * j, cubeSize * k));
					//cube->GetComponent<Transform>()->SetRotation({0.0f,45.0f, 0.0f});

					cube->GetComponent<Transform>()->SetInitScale({ 0.5f * cubeSize, 0.5f * cubeSize, 0.5f * cubeSize });
					//std::shared_ptr<Texture> cubeTexture(DBG_NEW Texture(Texture::Type::DiffuseMap, "assets/texture/wood.png"));
					cube->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorDiffuseColor({ (1.0f / num) * i,(1.0f / num) * j,(1.0f / num) * k });

					//cube->GetComponent<MeshRenderer>()->SetTextureSamples(false);
					cube->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
					objs.push_back(cube);
					m_BackGroundObjsList.push_back(cube);

				}
			}
		}
		return objs;
	}

	void Layer::LoadD3D12ModelScene()
	{

	}

	Object* Layer::LoadStaticBackGroundObject(const std::string modelName)
	{
		Object* staticModel = nullptr;
		IDevice* device = m_DeviceManager->GetDevice();

		if (modelName == "House") {
			//house model
			staticModel = CreateModel("assets/models/Alpine/Alpine_chalet.obj", "assets/shaders/glsl/IronMan.glsl", false, "House");
			TextureHandle housealbedoTexture = device->createTexture(TextureDesc(TextureType::DiffuseMap, "assets/models/Alpine/Diffuse_map.png"));
			TextureHandle houseroughnessTexture = device->createTexture(TextureDesc(TextureType::RoughnessMap, "assets/models/Alpine/Roughness_map.png"));
			TextureHandle housementallicTexture = device->createTexture(TextureDesc(TextureType::MentallicMap, "assets/models/Alpine/Metallic_map.png"));
			TextureHandle housenormalTexture = device->createTexture(TextureDesc(TextureType::NormalMap, "assets/models/Alpine/Normal_map.png"));
			TextureHandle houseaoTexture = device->createTexture(TextureDesc(TextureType::AoMap, "assets/models/Alpine/Ao_map.png"));

			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(housealbedoTexture);
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(houseroughnessTexture);
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(housementallicTexture);
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(housenormalTexture);
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(houseaoTexture);


			staticModel->GetComponent<BlackPearl::Transform>()->SetInitScale(glm::vec3(2.0f));
			staticModel->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 0.0f,-1.5f,1.0f });
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetIsPBRObject(true);
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		}

		else if (modelName == "Church") {
			staticModel = CreateModel("assets/models/sponza_obj/sponza.obj", "assets/shaders/glsl/IronMan.glsl", false, "Church");
			staticModel->GetComponent<BlackPearl::Transform>()->SetInitScale(glm::vec3(0.001f));
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		}
		else if (modelName == "Bunny") {
			staticModel = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/glsl/IronMan.glsl", false, "Bunny");
			staticModel->GetComponent<Transform>()->SetInitScale(glm::vec3(0.5));
			staticModel->GetComponent<Transform>()->SetInitPosition({ 0.6f,0.0f,3.0f });
			staticModel->GetComponent<Transform>()->SetInitRotation({ 0.0f,-30.0f,0.0f });
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		}
		else if (modelName == "Deer") {
			staticModel = CreateModel("assets/models/deer/Deer.obj", "assets/shaders/glsl/IronMan.glsl", false, "Deer");
			staticModel->GetComponent<Transform>()->SetInitScale(glm::vec3(0.003));
			staticModel->GetComponent<Transform>()->SetInitPosition({ -0.5f,0.0f,2.5f });
			staticModel->GetComponent<Transform>()->SetInitRotation({ 0.0f,68.0f,0.0f });
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);

		}
		if (modelName == "WoodCube") {
			staticModel = CreateCube();
			TextureDesc desc;
			desc.type = TextureType::DiffuseMap;
			desc.path = "assets/texture/wood.png";
			desc.minFilter = FilterMode::Linear_Mip_Linear;
			desc.magFilter = FilterMode::Linear;
			desc.wrap = SamplerAddressMode::ClampToEdge;
			desc.format = Format::RGBA8_UNORM;
			desc.generateMipmap = true;


			TextureHandle cubeTexture = device->createTexture(desc);
			desc.type = TextureType::SpecularMap;
			TextureHandle cubeKsTexture = device->createTexture(desc);

			staticModel->GetComponent<MeshRenderer>()->SetTextures(cubeTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(cubeKsTexture);

			staticModel->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);

		}if (modelName == "Sword") {
			staticModel = CreateModel("assets/models/sword/OBJ/Big_Sword_OBJ.obj", "assets/shaders/pbr/glsl/PbrTexture.glsl", false, "Sword");
			TextureHandle SwordalbedoTexture = device->createTexture(TextureDesc(TextureType::DiffuseMap, "assets/models/sword/textures/Big Sword_Base_Color_Map.jpg"));
			TextureHandle SwordaoTexture = device->createTexture(TextureDesc(TextureType::AoMap, "assets/models/sword/textures/Big Sword_AO_Map.jpg"));
			TextureHandle SwordroughnessTexture = device->createTexture(TextureDesc(TextureType::RoughnessMap, "assets/models/sword/textures/Big Sword_Roughness_Map.jpg"));
			TextureHandle SwordmentallicTexture = device->createTexture(TextureDesc(TextureType::MentallicMap, "assets/models/sword/textures/Big Sword_Metalness.jpg"));
			TextureHandle SwordnormalTexture = device->createTexture(TextureDesc(TextureType::NormalMap, "assets/models/sword/textures/Big Sword_Normal_Map.jpg"));
			TextureHandle SwordemissionTexture = device->createTexture(TextureDesc(TextureType::EmissionMap, "assets/models/sword/textures/Big Sword_Emission_Map.jpg"));

			staticModel->GetComponent<MeshRenderer>()->SetTextures(SwordalbedoTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(SwordaoTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(SwordroughnessTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(SwordmentallicTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(SwordnormalTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(SwordemissionTexture);

			staticModel->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
			staticModel->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
			//	sword->GetComponent<MeshRenderer>()->SetTextureSamples(true);//TODO::
			staticModel->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
			//	sword->GetComponent<MeshRenderer>()->SetTextureMetallicSamples(true);
			staticModel->GetComponent<MeshRenderer>()->SetTexturEmissionSamples(true);

			staticModel->GetComponent<MeshRenderer>()->SetIsPBRObject(true);

		}
		else if (modelName == "Sphere") {
			staticModel = CreateSphere(1.5, 64, 64);
			staticModel->GetComponent<Transform>()->SetInitScale(glm::vec3(0.003));
			staticModel->GetComponent<Transform>()->SetInitPosition({ -0.5f,0.0f,2.5f });
			staticModel->GetComponent<Transform>()->SetInitRotation({ 0.0f,68.0f,0.0f });
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
			TextureHandle PlasticalbedoTexture = device->createTexture(TextureDesc(TextureType::DiffuseMap, "assets/texture/pbr/plasticSphere/scuffed-plastic4-alb.png"));
			TextureHandle PlasticaoTexture = device->createTexture(TextureDesc(TextureType::AoMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-ao.png"));
			TextureHandle PlasticroughnessTexture = device->createTexture(TextureDesc(TextureType::RoughnessMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-rough.png"));
			TextureHandle PlasticmentallicTexture = device->createTexture(TextureDesc(TextureType::MentallicMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-metal.png"));
			TextureHandle PlasticnormalTexture = device->createTexture(TextureDesc(TextureType::NormalMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-normal.png"));
			staticModel->GetComponent<MeshRenderer>()->SetTextures(PlasticnormalTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(PlasticalbedoTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(PlasticaoTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(PlasticroughnessTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(PlasticmentallicTexture);

			staticModel->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
			staticModel->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
			staticModel->GetComponent<Transform>()->SetInitPosition({ -10.0,0,0 });
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		}
		else if (modelName == "SphereIron") {
			staticModel = CreateSphere(1.5, 64, 64);

			TextureHandle IronalbedoTexture = device->createTexture(TextureDesc(TextureType::DiffuseMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_basecolor.png"));
			TextureHandle IronaoTexture = device->createTexture(TextureDesc(TextureType::AoMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_ao.png"));
			TextureHandle IronroughnessTexture = device->createTexture(TextureDesc(TextureType::RoughnessMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_roughness.png"));
			TextureHandle IronmentallicTexture = device->createTexture(TextureDesc(TextureType::MentallicMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_metallic.png"));
			TextureHandle IronnormalTexture = device->createTexture(TextureDesc(TextureType::NormalMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_normal.png"));

			staticModel->GetComponent<MeshRenderer>()->SetTextures(IronnormalTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(IronalbedoTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(IronaoTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(IronroughnessTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(IronmentallicTexture);

			staticModel->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
			staticModel->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
			//	sphereObjIron->GetComponent<MeshRenderer>()->SetTextureMetallicSamples(true);
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);

			staticModel->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
			staticModel->GetComponent<Transform>()->SetInitPosition({ 0,0,0 });

			BlackPearl::Object* specularProbe = CreateLightProbe(BlackPearl::ProbeType::REFLECTION_PROBE);
			staticModel->AddChildObj(specularProbe);
			specularProbe->GetComponent<BlackPearl::LightProbe>()->AddExcludeObjectId(staticModel->GetId().id);
		}
		else if (modelName == "SphereRust") {
			staticModel = CreateSphere(1.5, 64, 64);
			TextureHandle RustalbedoTexture = device->createTexture(TextureDesc(TextureType::DiffuseMap, "assets/texture/pbr/rustSphere/rustediron2_basecolor.png"));
			TextureHandle RustaoTexture = device->createTexture(TextureDesc(TextureType::AoMap, "assets/texture/pbr/rustSphere/rustediron2_ao.png"));
			TextureHandle RustroughnessTexture = device->createTexture(TextureDesc(TextureType::RoughnessMap, "assets/texture/pbr/rustSphere/rustediron2_roughness.png"));
			TextureHandle RustmentallicTexture = device->createTexture(TextureDesc(TextureType::MentallicMap, "assets/texture/pbr/rustSphere/rustediron2_metallic.png"));
			TextureHandle RustnormalTexture = device->createTexture(TextureDesc(TextureType::NormalMap, "assets/texture/pbr/rustSphere/rustediron2_normal.png"));
			staticModel->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
			staticModel->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
			//sphereObjRust->GetComponent<MeshRenderer>()->SetTextureMetallicSamples(true);

			staticModel->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
			staticModel->GetComponent<Transform>()->SetInitPosition({ 5,0,0 });
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);

		}
		else if (modelName == "SphereStone") {
			staticModel = CreateSphere(1.5, 64, 64);

			TextureHandle StonealbedoTexture = device->createTexture(TextureDesc(TextureType::DiffuseMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_albedo.png"));
			TextureHandle StoneaoTexture = device->createTexture(TextureDesc(TextureType::AoMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_ao.png"));
			TextureHandle StoneroughnessTexture = device->createTexture(TextureDesc(TextureType::RoughnessMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_roughness.png"));
			TextureHandle StonementallicTexture = device->createTexture(TextureDesc(TextureType::MentallicMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_metallic.png"));
			TextureHandle StonenormalTexture = device->createTexture(TextureDesc(TextureType::NormalMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_normal-dx.png"));

			staticModel->GetComponent<MeshRenderer>()->SetTextures(StonenormalTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(StonealbedoTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(StoneaoTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(StoneroughnessTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(StonementallicTexture);

			staticModel->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
			staticModel->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
			//	sphereObjStone->GetComponent<MeshRenderer>()->SetTextureMetallicSamples(true);

			staticModel->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		}
		else if (modelName == "SpherePlastic") {
			staticModel = CreateSphere(1.5, 64, 64);
			TextureHandle PlasticalbedoTexture = device->createTexture(TextureDesc(TextureType::DiffuseMap, "assets/texture/pbr/plasticSphere/scuffed-plastic4-alb.png"));
			TextureHandle PlasticaoTexture = device->createTexture(TextureDesc(TextureType::AoMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-ao.png"));
			TextureHandle PlasticroughnessTexture = device->createTexture(TextureDesc(TextureType::RoughnessMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-rough.png"));
			TextureHandle PlasticmentallicTexture = device->createTexture(TextureDesc(TextureType::MentallicMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-metal.png"));
			TextureHandle PlasticnormalTexture = device->createTexture(TextureDesc(TextureType::NormalMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-normal.png"));
			staticModel->GetComponent<MeshRenderer>()->SetTextures(PlasticnormalTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(PlasticalbedoTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(PlasticaoTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(PlasticroughnessTexture);
			staticModel->GetComponent<MeshRenderer>()->SetTextures(PlasticmentallicTexture);


			staticModel->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
			staticModel->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
			//	sphereObjPlastic->GetComponent<MeshRenderer>()->SetTextureMetallicSamples(true);

			staticModel->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);

		}
		else {
			GE_CORE_ERROR("no such name: %s !" , modelName.c_str())
		}
		m_BackGroundObjsList.push_back(staticModel);
		return staticModel;
	}

	Object* Layer::LoadDynamicObject(const std::string modelName)
	{
		Object* dynamicModel = nullptr;
		if (modelName == "Boy") {
			dynamicModel = CreateModel("assets/models-animation/people/character Texture.dae", "assets/shaders/glsl/animatedModel/animatedModel.glsl", true, "Boy");
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitScale({ 0.2f,0.2f,0.2f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitRotation({ -90.0f,0.0f,0.0f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 3.0f,-1.6f,0.0f });

		}
		else if (modelName == "Robot") {

			auto device = m_DeviceManager->GetDevice();
			dynamicModel = CreateModel("assets/models-animation/56-sphere-bot-basic/Sphere-Bot Basic/Armature_001-(COLLADA_3 (COLLAborative Design Activity)).dae", "assets/shaders/glsl/animatedModel/animatedModel.glsl", true, "Robot");
			TextureHandle RobotAoTexture = device->createTexture(TextureDesc(TextureType::AoMap, "assets/models-animation/56-sphere-bot-basic/Sphere-Bot Basic/Sphere_Bot_ao.jpg"));
			TextureHandle RobotRoughnessTexture = device->createTexture(TextureDesc(TextureType::RoughnessMap, "assets/models-animation/56-sphere-bot-basic/Sphere-Bot Basic/Sphere_Bot_rough.jpg"));
			TextureHandle RobotMentallicTexture = device->createTexture(TextureDesc(TextureType::MentallicMap, "assets/models-animation/56-sphere-bot-basic/Sphere-Bot Basic/Sphere_Bot_metalness.jpg"));
			TextureHandle RobotNormalTexture = device->createTexture(TextureDesc(TextureType::NormalMap, "assets/models-animation/56-sphere-bot-basic/Sphere-Bot Basic/Sphere_Bot_nmap_2.jpg"));

			dynamicModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RobotNormalTexture);
			dynamicModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RobotRoughnessTexture);
			dynamicModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RobotAoTexture);
			dynamicModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RobotMentallicTexture);

			dynamicModel->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
		}
		else if (modelName == "Frog") {
			dynamicModel = CreateModel("assets/models-animation/frog/frog.dae.txt", "assets/shaders/glsl/animatedModel/animatedModel.glsl", true, "Frog");
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitScale({ 0.5f,0.5f,0.5f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitRotation({ 90.0f,180.0f,0.0f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitPosition({ -3.0f,-1.6f,0.0f });
		}
		else if (modelName == "Cleaner") {
			dynamicModel = CreateModel("assets/models-animation/boblampclean.md5mesh", "assets/shaders/glsl/animatedModel/animatedModel.glsl", true, "Cleaner");
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitScale({ 0.05f,0.05f,0.05f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitRotation({ 90.0f,180.0f,180.0f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 0.0f,-1.6f,0.0f });
		}
		else {
			GE_CORE_ERROR("no such name: %s" , modelName.c_str() )
		}
		m_DynamicObjsList.push_back(dynamicModel);
		return dynamicModel;
		//		m_DynamicObjsList.push_back(m_AnimatedModelRobot);

	}

	Object* Layer::CreateEmpty(std::string name) {

		Object* obj = g_objectManager->CreateEmpty(name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateGroup(const std::string name)
	{
		Object* obj = g_objectManager->CreateGroup(name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateBVHNode(const std::vector<Object*>& objs, const std::string name)
	{
		Object* obj = g_objectManager->CreateBVHNode(objs, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateTerrain(const std::string& heightMapPath, const std::string& shaderPath, const std::string& texturePath, uint32_t chunkCntX, uint32_t chunkCntZ, const std::string name)
	{
		Object* obj = g_objectManager->CreateTerrain(shaderPath, heightMapPath, texturePath, chunkCntX, chunkCntZ, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateLight(LightType type, const std::string& name)
	{
		Object* obj = g_objectManager->CreateLight(type, m_LightSources, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateCube(const std::string& shaderPath, const std::string& texturePath, const std::string& name) //TODO:
	{
		Object* obj = g_objectManager->CreateCube(shaderPath, texturePath, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string& shaderPath, const std::string& texturePath, const std::string& name)
	{
		Object* obj = g_objectManager->CreateSphere(radius, stackCount, sectorCount, shaderPath, texturePath, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	
    Object* Layer::CreatePlane(const std::string& shaderPath, const std::string& texturePath, const std::string& name)
	{
		Object* obj = g_objectManager->CreatePlane(shaderPath, texturePath, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}

	Object* Layer::CreateSkyBox(const std::vector<std::string>& textureFaces, const std::string& shaderPath, const std::string& name)
	{
		Object* obj = g_objectManager->CreateSkyBox(textureFaces, shaderPath, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}

	Object* Layer::CreateQuad(const std::string& shaderPath, const std::string& texturePath, const std::string& name)
	{
		Object* obj = g_objectManager->CreateQuad(shaderPath, texturePath, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}

	Object* Layer::CreateLightProbe(ProbeType type, const std::string& shaderPath, const std::string& texturePath, const std::string& name)
	{

		Object* probe = g_objectManager->CreateLightProbe(type, shaderPath, texturePath, name + (type == ProbeType::DIFFUSE_PROBE ? "_kd" : "_ks"));
		m_ObjectsList.push_back(probe);
		//m_ObjectsList.push_back(probe->GetCamera()->GetObj());
        if (type == ProbeType::DIFFUSE_PROBE)
            m_DiffuseLightProbes.push_back(probe);
        else
            m_ReflectionLightProbes.push_back(probe);
		return probe;
	}

	Object* Layer::CreateProbeGrid(MapManager* mapManager, ProbeType type, math::float3 probeNums, math::float3 offsets, float space)
	{
		std::string objName = (type == ProbeType::DIFFUSE_PROBE) ? "Kd ProbesGrid" : "Ks ProbeGrid";
		Object* obj = CreateEmpty(objName);
		unsigned int idx = 0;
		for (unsigned int x = 0; x < probeNums.x; x++)
		{
			for (unsigned int y = 0; y < probeNums.y; y++)
			{
				for (unsigned int z = 0; z < probeNums.z; z++)
				{
					Object* probe = CreateLightProbe(type);
					int xx = (x - probeNums.x / 2) * space, yy = (y - probeNums.y / 2) * space, zz = (z - probeNums.z / 2) * space;
					glm::vec3 probePos = { offsets.x + xx,offsets.y + yy,offsets.z + zz };
					probe->GetComponent<Transform>()->SetInitPosition(probePos);
					if (type == ProbeType::DIFFUSE_PROBE) {
						unsigned int areaId = mapManager->AddProbeIdToArea(probePos, idx);
						probe->GetComponent<LightProbe>()->SetAreaId(areaId);
					}

					idx++;
					obj->AddChildObj(probe);

				}

			}
		}
		return obj;
	}
	Object* Layer::CreateModel(
		const std::string& modelPath,
		const std::string& shaderPath,
		const bool isAnimated,
		const std::string& name,
		const bool vertices_sorted,
		const bool createMeshlet,
		const bool isMeshletModel,
		MeshletOption options)
	{
		Object* obj = g_objectManager->CreateModel(modelPath, shaderPath, isAnimated, vertices_sorted, false, name, createMeshlet, isMeshletModel, options);
		m_ObjectsList.push_back(obj);
		m_BackGroundObjsList.push_back(obj);
		return obj;
	}
	MainCamera* Layer::CreateCamera(const std::string& name) {

		MainCamera* mainCamera = g_objectManager->CreateCamera(name);
		m_ObjectsList.push_back(mainCamera->GetObj());
		return mainCamera;
	}

	BatchNode* Layer::CreateBatchNode(std::vector<Object*> objs, bool dynamic, const std::string& name)
	{
		BatchNode* batchNode = g_objectManager->CreateBatchNode(objs, dynamic, name);
		m_ObjectsList.push_back(batchNode->GetSelfObj());

		return batchNode;
	}

	std::vector<Object*> Layer::GetObjects()
	{
		return g_objectManager->GetObjects();
	}
	void Layer::DestroyObjects() //TODO:ɾ��ĳһ��Objects�������û����--�����¿���m_EntityToObjects���ݽṹ
	{
		g_objectManager->DestroyObjects();
	}
}