#include "pch.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/LayerScene/Layer.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/LightProbeComponent/LightProbeComponent.h"
#include "BlackPearl/Renderer/Model/Model.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "BlackPearl/Renderer/MasterRenderer/IBLRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/IBLProbesRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/GBufferRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/VoxelConeTracingRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/VoxelConeTracingDeferredRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/VoxelConeTracingSVORenderer.h"
namespace BlackPearl {

	static int buttonNum = 0;

	//bool isBackGroundObj=false;


	void Layer::OnImguiRender()
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Suqare Color", glm::value_ptr(m_BackgroundColor));
		ImGui::End();
		ImGui::Begin("FPS");
		ImGui::Text("FPS = %.3lf", Application::s_AppFPS);
		ImGui::Text("AvgFPS = %.3lf", Application::s_AppAverageFPS);

		ImGui::Text("SVO voxel GI");
		ImGui::Checkbox("spp pause", &VoxelConeTracingSVORenderer::s_Pause);
		ImGui::Checkbox("svo direct light", &VoxelConeTracingSVORenderer::s_DirectLight);
		ImGui::Checkbox("svo Indirect diffuse light", &VoxelConeTracingSVORenderer::s_IndirectDiffuseLight);
		ImGui::Checkbox("svo Indirect specular light", &VoxelConeTracingSVORenderer::s_IndirectSpecularLight);
		ImGui::DragFloat("svo GICoeffs", &VoxelConeTracingSVORenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
		ImGui::DragFloat("specularAngle", &VoxelConeTracingSVORenderer::s_IndirestSpecularAngle, 0.2f, 0.01f, 45.0f, "%.4f ");
		ImGui::DragFloat("specularStep", &VoxelConeTracingSVORenderer::s_Step, 0.01f, 0.01f, 45.0f, "%.4f ");


		ImGui::Text("deferred voxel GI");
		ImGui::Checkbox("voxelize", &VoxelConeTracingDeferredRenderer::s_VoxelizeNow);

		ImGui::Checkbox("voxel Indirect diffuse", &VoxelConeTracingDeferredRenderer::s_IndirectDiffuseLight);
		ImGui::Checkbox("voxel Indirect specular", &VoxelConeTracingDeferredRenderer::s_IndirectSpecularLight);
		ImGui::Checkbox("voxel direct light", &VoxelConeTracingDeferredRenderer::s_DirectLight);
		ImGui::Checkbox("voxel shadows", &VoxelConeTracingDeferredRenderer::s_Shadows);

		ImGui::Checkbox("voxel HDR", &VoxelConeTracingDeferredRenderer::s_HDR);
		ImGui::Checkbox("voxel blur horizontal", &VoxelConeTracingDeferredRenderer::s_GuassianHorizontal);
		ImGui::Checkbox("voxel blur vertical", &VoxelConeTracingDeferredRenderer::s_GuassianVertical);
		ImGui::Checkbox("voxel blur showBlurArea", &VoxelConeTracingDeferredRenderer::s_ShowBlurArea);
		ImGui::Checkbox("voxel blur mipmap", &VoxelConeTracingDeferredRenderer::s_MipmapBlurSpecularTracing);
		ImGui::DragFloat("voxel specularBlurThreshold", &VoxelConeTracingDeferredRenderer::s_SpecularBlurThreshold, 0.2f, 0.0f, 1.0f, "%.4f ");
		ImGui::DragFloat("voxel indirectSpecularAngle", &VoxelConeTracingDeferredRenderer::s_IndirectSpecularAngle, 0.2f, 1.0f, 45.0f, "%.4f ");
		ImGui::DragFloat("voxel GICoeffs", &VoxelConeTracingDeferredRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");

		ImGui::DragInt("voxel visualization \n mipmap level", &VoxelConeTracingDeferredRenderer::s_VisualizeMipmapLevel, 1.0f, 0, 5);


	
		
		ImGui::Text("forward voxel GI");
		ImGui::Checkbox("voxel Indirect diffuse", &VoxelConeTracingRenderer::s_IndirectDiffuseLight);
		ImGui::Checkbox("voxel Indirect specular", &VoxelConeTracingRenderer::s_IndirectSpecularLight);
		ImGui::Checkbox("voxel direct light", &VoxelConeTracingRenderer::s_DirectLight);
		ImGui::DragFloat("voxel GICoeffs", &VoxelConeTracingRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
		ImGui::Checkbox("voxel HDR", &VoxelConeTracingRenderer::s_HDR);

		ImGui::Text("light probe GI");
		ImGui::DragFloat("lightprobe GICoeffs", &GBufferRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
		ImGui::Checkbox("lightprobe HDR", &GBufferRenderer::s_HDR);




		ImGui::Text("image based lighting  GI");
		ImGui::DragFloat("IBL GICoeffs", &IBLRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
		ImGui::Checkbox("IBL HDR", &GBufferRenderer::s_HDR);



		ImGui::End();

		static Object* currentObj = nullptr;//TODO::注意内存泄漏

		if (ImGui::CollapsingHeader("Create")) {

			const char* const entityItems[] = { "Empty","ParallelLight","PointLight","SpotLight","IronMan","Deer","OldHouse","Bunny","Cube","Plane" };
			static int entityIdx = -1;
			if (ImGui::Combo("CreateEntity", &entityIdx, entityItems, 10))
			{
				switch (entityIdx)
				{
				case 0:
					GE_CORE_INFO("Creating Empty...");
					Layer::CreateEmpty();
					break;
				case 1:
					GE_CORE_INFO("Creating PointLight...");
					Layer::CreateLight(LightType::ParallelLight);
					break;
				case 2:
					GE_CORE_INFO("Creating PointLight...");
					Layer::CreateLight(LightType::PointLight);
					break;
				case 3:
					GE_CORE_INFO("Creating SpotLight ...");
					Layer::CreateLight(LightType::SpotLight);
					break;
				case 4:
					GE_CORE_INFO("Creating IronMan ...");
					Layer::CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/IronMan.glsl", false, "IronMan");
					break;
				case 5:
					GE_CORE_INFO("Creating Deer ...");
					//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
					LoadStaticBackGroundObject("Deer");

					break;
				case 6:
					GE_CORE_INFO("Creating OldHouse ...");
					//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
					Layer::CreateModel("assets/models/OldHouse/Gost House/3D models/Gost House (5).obj", "assets/shaders/IronMan.glsl", false, "OldHouse");

					break;
				case 7:
					GE_CORE_INFO("Creating Bunny ...");
					LoadStaticBackGroundObject("Bunny");
					//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny");
				case 8:
					GE_CORE_INFO("Creating Cube ...");
					Layer::CreateCube();
					break;
				case 9:
					GE_CORE_INFO("Creating Plane ...");
					Layer::CreatePlane();
					break;
				}
			}
		}
		if (ImGui::BeginTabBar("TabBar 0", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Scene")) {
				std::vector<Object*> objsList = GetObjects();		//TODO::
				ImGui::ListBoxHeader("CurrentEntities", (int)objsList.size(), 10);

				for (int n = 0; n < objsList.size(); n++) {
					//ImGui::Text("%s", objsList[n].c_str());
					bool is_selected = (currentObj != nullptr && currentObj->ToString() == objsList[n]->ToString());
					if (ImGui::Selectable(objsList[n]->ToString().c_str(), is_selected)) {
						currentObj = objsList[n];
						GE_CORE_INFO(objsList[n]->ToString() + "is selected")
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::ListBoxFooter();
				ImGui::EndTabItem();
			}
			//}
		}
		ImGui::EndTabBar();

		////////////////////Inspector/////////////////////////
		ImGui::Begin("Inspector");


		/*float pos[] = { m_Sun->GetComponent<ParallelLight>()->GetDirection().x, m_Sun->GetComponent<ParallelLight>()->GetDirection().y, m_Sun->GetComponent<ParallelLight>()->GetDirection().z };
		ImGui::DragFloat3("m_LightPos", pos, 0.1f, -100.0f, 100.0f, "%.3f ");
		m_Sun->GetComponent<ParallelLight>()->SetDirection({ pos[0],pos[1],pos[2] });*/

		/*
				ImGui::DragFloat("near_plane", &ShadowMapRenderer::s_NearPlane, 0.5f, -50.0f, 100.0f, "%.3f ");
				ImGui::DragFloat("far_plane", &ShadowMapRenderer::s_FarPlane, 0.5f, -50.0f, 100.0f, "%.3f ");*/

		if (currentObj != nullptr) {


			if (currentObj->HasComponent< Transform>()) {
				ShowTransform(currentObj->GetComponent<Transform>(),currentObj);

			}
			if (currentObj->HasComponent< MeshRenderer>()) {
				ShowMeshRenderer(currentObj->GetComponent<MeshRenderer>());
				/*backGroundObj list*/

				bool isBackGroundObj = currentObj->GetComponent<MeshRenderer>()->GetIsBackGroundObjects();
				ImGui::Checkbox("isBackGroundObj", &isBackGroundObj);
				//TODO:: 可以采用 bitset
				if (isBackGroundObj) {
					std::vector<Object*>::const_iterator it = std::find(m_BackGroundObjsList.begin(), m_BackGroundObjsList.end(), currentObj);
					if (it == m_BackGroundObjsList.end()) {
						m_BackGroundObjsList.push_back(currentObj);
						currentObj->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
					}

				}
				else {
					std::vector<Object*>::const_iterator it;// = m_BackGroundObjsList.begin();
					for (it = m_BackGroundObjsList.begin(); it != m_BackGroundObjsList.end(); it++) {
						if ((*it)->GetId() == currentObj->GetId()) {
							m_BackGroundObjsList.erase(it);
							currentObj->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(false);

							break;
						}
					}
				}

				/*shadowObj list*/
				bool isShadowObj = currentObj->GetComponent<MeshRenderer>()->GetIsShadowObjects();
				ImGui::Checkbox("isShadowObj", &isShadowObj);
				//TODO:: 可以采用 bitset
				if (isShadowObj) {
					std::vector<Object*>::const_iterator it = std::find(m_ShadowObjsList.begin(), m_ShadowObjsList.end(), currentObj);
					if (it == m_ShadowObjsList.end()) {
						m_ShadowObjsList.push_back(currentObj);
						currentObj->GetComponent<MeshRenderer>()->SetIsShadowObjects(true);
					}

				}
				else {
					std::vector<Object*>::const_iterator it;// = m_BackGroundObjsList.begin();
					for (it = m_ShadowObjsList.begin(); it != m_ShadowObjsList.end(); it++) {
						if ((*it)->GetId() == currentObj->GetId()) {
							m_ShadowObjsList.erase(it);
							currentObj->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);

							break;
						}
					}
				}


			}
			if (currentObj->HasComponent < PointLight>()) {
				ShowPointLight(currentObj->GetComponent<PointLight>());
			}
			if (currentObj->HasComponent < ParallelLight>()) {
				ShowParallelLight(currentObj->GetComponent<ParallelLight>());
			}
			if (currentObj->HasComponent< PerspectiveCamera>()) {
				ShowCamera(currentObj->GetComponent<PerspectiveCamera>());

			}



		}

		ImGui::End();
		m_fileDialog.Display();


	}

	void Layer::LoadScene(const std::string demoScene)
	{
		if (demoScene == "CornellScene")
			LoadCornellScene();
		else if (demoScene == "SpheresScene")
			LoadSpheresScene();
		else if (demoScene == "CubesScene")
			LoadCubesScene();
		else if (demoScene == "SwordScene") {
			LoadSwordScene();
		}
		else if (demoScene == "Church") {
			LoadChurchScene();
		}

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

		cube1->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube2->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube3->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube4->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube5->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });

		cube1->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ 0,0.294f,1.0f });
		cube2->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ 1,0.267f,0 });
		cube3->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ 1,1,1 });
		cube4->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ 0.204f,0,1 });
		cube5->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ 1,0,0 });
		m_BackGroundObjsList.push_back(cube1);
		m_BackGroundObjsList.push_back(cube2);
		m_BackGroundObjsList.push_back(cube3);
		m_BackGroundObjsList.push_back(cube4);
		m_BackGroundObjsList.push_back(cube5);

		Object* deer = CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl", false, "Deer");
		deer->GetComponent<Transform>()->SetScale(glm::vec3(0.003));
		deer->GetComponent<Transform>()->SetInitPosition({ -0.5f,0.0f,2.5f });
		deer->GetComponent<Transform>()->SetRotation({ 0.0f,68.0f,0.0f });
		deer->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(deer);

		Object* bunny = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny");
		bunny->GetComponent<Transform>()->SetScale(glm::vec3(0.5));
		bunny->GetComponent<Transform>()->SetInitPosition({ 0.6f,0.0f,3.0f });
		bunny->GetComponent<Transform>()->SetRotation({ 0.0f,-30.0f,0.0f });
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

		Object* deer = CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl", false, "Deer");
		deer->GetComponent<Transform>()->SetScale(glm::vec3(0.003));
		//deer->GetComponent<Transform>()->SetPosition({ -0.5f,0.0f,2.5f });
		deer->GetComponent<Transform>()->SetInitPosition({ -0.5f,-1.5f,-0.5f });
		deer->GetComponent<Transform>()->SetRotation({ 0.0f,68.0f,0.0f });
		deer->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(deer);
		m_ShadowObjsList.push_back(deer);

		Object* bunny = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny");
		bunny->GetComponent<Transform>()->SetScale(glm::vec3(0.5));
		//bunny->GetComponent<Transform>()->SetPosition({ 0.6f,0.0f,3.0f });
		bunny->GetComponent<Transform>()->SetInitPosition({ 0.6f,-1.5f,-0.0f });
		bunny->GetComponent<Transform>()->SetRotation({ 0.0f,-30.0f,0.0f });
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

		cube1->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube2->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube3->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube4->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube5->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });

		cube1->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ 0,0.294f,1.0f });
		cube2->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ 1,0.267f,0 });
		cube3->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ 1,1,1 });
		cube4->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ 0.204f,0,1 });
		cube5->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ 1,0,0 });
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
		Object* church = CreateModel("assets/models/crytek-sponza/sponza.obj", "assets/shaders/IronMan.glsl", false, "Church");

		//Object* church = CreateModel("assets/models/sponza_obj/sponza.obj", "assets/shaders/IronMan.glsl", false, "Church");
		church->GetComponent<Transform>()->SetScale(glm::vec3(0.02));//0.02
		church->GetComponent<Transform>()->SetInitPosition({ 0.0f,0.0f,10.0f });
		church->GetComponent<Transform>()->SetRotation({ 0.0f,-90.0f,0.0f });
		//church->GetComponent<Transform>()->SetScale({ 0.1f,0.1f,0.1f });

		church->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		church->GetComponent<MeshRenderer>()->SetTextureHeightSamples(true);
		church->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
		church->GetComponent<MeshRenderer>()->SetTextureSpecularSamples(true);
		church->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);

		m_BackGroundObjsList.push_back(church);
		//m_ShadowObjsList.push_back(church);


		BlackPearl::Object* light = CreateLight(LightType::PointLight);
		light->GetComponent<Transform>()->SetInitPosition({ 0.0,4.0,10.0 });
		light->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);
		light->GetComponent<PointLight>()->UpdateMesh({ {0,0,0} ,{1,1,1},{1,1,1},{0,0,0},1.0});

		//for (int i = 0; i < 10; i++) {
		//	Object* light = CreateLight(LightType::PointLight);
		//	light->GetComponent<Transform>()->SetPosition({ -50+i*10,30,0.0 });
		//	light->GetComponent<Transform>()->SetLastPosition({ 0.0,-1.0,0.0 });//0.0,0.0,3.0
		//	light->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);
		//	light->GetComponent<PointLight>()->UpdateMesh({ {0,0,0} ,{1,1,1},{1,1,1},{0,0,0},40 });

		//}

		//for (int i = 0; i < 10; i++) {
		//	Object* light = CreateLight(LightType::PointLight);
		//	light->GetComponent<Transform>()->SetPosition({ -50 + i * 10,5,0.0 });
		//	light->GetComponent<Transform>()->SetLastPosition({ 0.0,-1.0,0.0 });//0.0,0.0,3.0
		//	light->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);
		//	light->GetComponent<PointLight>()->UpdateMesh({ {0,0,0} ,{1,1,1},{1,1,1},{0,0,0},40 });

		//}

		//for (int i = 0; i < 10; i++) {
		//	Object* light = CreateLight(LightType::PointLight);
		//	light->GetComponent<Transform>()->SetPosition({ -50 + i * 10,10,0.0 });
		//	light->GetComponent<Transform>()->SetLastPosition({ 0.0,-1.0,0.0 });//0.0,0.0,3.0
		//	light->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);
		//	light->GetComponent<PointLight>()->UpdateMesh({ {0,0,0} ,{1,1,1},{1,1,1},{0,0,0},40 });

		//}
	
	}

	void Layer::LoadSpheresScene()
	{
		//Scene
		Object* light = CreateLight(LightType::PointLight);
		light->GetComponent<Transform>()->SetInitPosition({ 0.0,-1.0,0.0 });
		light->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);

		


		Object* sphereObjIron =  CreateSphere(1.5, 64, 64);//CreateCube();//
		Object* sphereObjRust = CreateSphere(1.5, 64, 64);
		Object* sphereObjStone = CreateSphere(1.5, 64, 64);
		Object* sphereObjPlastic = CreateSphere(1.5, 64, 64);
		//textures spheres
		std::shared_ptr<Texture> RustalbedoTexture(DBG_NEW Texture(Texture::Type::DiffuseMap, "assets/texture/pbr/rustSphere/rustediron2_basecolor.png"));
		std::shared_ptr<Texture> RustaoTexture(DBG_NEW Texture(Texture::Type::AoMap, "assets/texture/pbr/rustSphere/rustediron2_ao.png"));
		std::shared_ptr<Texture> RustroughnessTexture(DBG_NEW Texture(Texture::Type::RoughnessMap, "assets/texture/pbr/rustSphere/rustediron2_roughness.png"));
		std::shared_ptr<Texture> RustmentallicTexture(DBG_NEW Texture(Texture::Type::MentallicMap, "assets/texture/pbr/rustSphere/rustediron2_metallic.png"));
		std::shared_ptr<Texture> RustnormalTexture(DBG_NEW Texture(Texture::Type::NormalMap, "assets/texture/pbr/rustSphere/rustediron2_normal.png"));


		std::shared_ptr<Texture> IronalbedoTexture(DBG_NEW Texture(Texture::Type::DiffuseMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_basecolor.png"));
		std::shared_ptr<Texture> IronaoTexture(DBG_NEW Texture(Texture::Type::AoMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_ao.png"));
		std::shared_ptr<Texture> IronroughnessTexture(DBG_NEW Texture(Texture::Type::RoughnessMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_roughness.png"));
		std::shared_ptr<Texture> IronmentallicTexture(DBG_NEW Texture(Texture::Type::MentallicMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_metallic.png"));
		std::shared_ptr<Texture> IronnormalTexture(DBG_NEW Texture(Texture::Type::NormalMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_normal.png"));

		std::shared_ptr<Texture> StonealbedoTexture(DBG_NEW Texture(Texture::Type::DiffuseMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_albedo.png"));
		std::shared_ptr<Texture> StoneaoTexture(DBG_NEW Texture(Texture::Type::AoMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_ao.png"));
		std::shared_ptr<Texture> StoneroughnessTexture(DBG_NEW Texture(Texture::Type::RoughnessMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_roughness.png"));
		std::shared_ptr<Texture> StonementallicTexture(DBG_NEW Texture(Texture::Type::MentallicMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_metallic.png"));
		std::shared_ptr<Texture> StonenormalTexture(DBG_NEW Texture(Texture::Type::NormalMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_normal-dx.png"));

		std::shared_ptr<Texture> PlasticalbedoTexture(DBG_NEW Texture(Texture::Type::DiffuseMap, "assets/texture/pbr/plasticSphere/scuffed-plastic4-alb.png"));
		std::shared_ptr<Texture> PlasticaoTexture(DBG_NEW Texture(Texture::Type::AoMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-ao.png"));
		std::shared_ptr<Texture> PlasticroughnessTexture(DBG_NEW Texture(Texture::Type::RoughnessMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-rough.png"));
		std::shared_ptr<Texture> PlasticmentallicTexture(DBG_NEW Texture(Texture::Type::MentallicMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-metal.png"));
		std::shared_ptr<Texture> PlasticnormalTexture(DBG_NEW Texture(Texture::Type::NormalMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-normal.png"));



		sphereObjIron->GetComponent<MeshRenderer>()->SetTextures(IronnormalTexture);
		sphereObjIron->GetComponent<MeshRenderer>()->SetTextures(IronalbedoTexture);
		sphereObjIron->GetComponent<MeshRenderer>()->SetTextures(IronaoTexture);
		sphereObjIron->GetComponent<MeshRenderer>()->SetTextures(IronroughnessTexture);
		sphereObjIron->GetComponent<MeshRenderer>()->SetTextures(IronmentallicTexture);

		sphereObjIron->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
		sphereObjIron->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
	//	sphereObjIron->GetComponent<MeshRenderer>()->SetTextureMetallicSamples(true);

		sphereObjIron->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
		sphereObjIron->GetComponent<Transform>()->SetInitPosition({ 10,0,0 });

		sphereObjRust->GetComponent<MeshRenderer>()->SetTextures(RustnormalTexture);
		sphereObjRust->GetComponent<MeshRenderer>()->SetTextures(RustalbedoTexture);
		sphereObjRust->GetComponent<MeshRenderer>()->SetTextures(RustaoTexture);
		sphereObjRust->GetComponent<MeshRenderer>()->SetTextures(RustroughnessTexture);
		sphereObjRust->GetComponent<MeshRenderer>()->SetTextures(RustmentallicTexture);

		sphereObjRust->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
		sphereObjRust->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
		//sphereObjRust->GetComponent<MeshRenderer>()->SetTextureMetallicSamples(true);

		sphereObjRust->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
		sphereObjRust->GetComponent<Transform>()->SetInitPosition({ 5,0,0 });

		sphereObjStone->GetComponent<MeshRenderer>()->SetTextures(StonenormalTexture);
		sphereObjStone->GetComponent<MeshRenderer>()->SetTextures(StonealbedoTexture);
		sphereObjStone->GetComponent<MeshRenderer>()->SetTextures(StoneaoTexture);
		sphereObjStone->GetComponent<MeshRenderer>()->SetTextures(StoneroughnessTexture);
		sphereObjStone->GetComponent<MeshRenderer>()->SetTextures(StonementallicTexture);

		sphereObjStone->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
		sphereObjStone->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
	//	sphereObjStone->GetComponent<MeshRenderer>()->SetTextureMetallicSamples(true);

		sphereObjStone->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
		sphereObjStone->GetComponent<Transform>()->SetInitPosition({ -5,0,0 });

		sphereObjPlastic->GetComponent<MeshRenderer>()->SetTextures(PlasticnormalTexture);
		sphereObjPlastic->GetComponent<MeshRenderer>()->SetTextures(PlasticalbedoTexture);
		sphereObjPlastic->GetComponent<MeshRenderer>()->SetTextures(PlasticaoTexture);
		sphereObjPlastic->GetComponent<MeshRenderer>()->SetTextures(PlasticroughnessTexture);
		sphereObjPlastic->GetComponent<MeshRenderer>()->SetTextures(PlasticmentallicTexture);


		sphereObjPlastic->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
		sphereObjPlastic->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
	//	sphereObjPlastic->GetComponent<MeshRenderer>()->SetTextureMetallicSamples(true);

		sphereObjPlastic->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
		sphereObjPlastic->GetComponent<Transform>()->SetInitPosition({ -10.0,0,0 });


		sphereObjIron->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		sphereObjRust->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		sphereObjStone->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		sphereObjPlastic->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);


		Object* cube = CreateCube();
		cube->GetComponent<Transform>()->SetInitPosition({ -2.0f,-2.5f,0.0f });
		cube->GetComponent<Transform>()->SetScale({ 16.0f,0.8f,16.0f });
		std::shared_ptr<Texture> cubeTexture(DBG_NEW Texture(Texture::Type::DiffuseMap, "assets/texture/wood.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE,true));
		std::shared_ptr<Texture> cubeKsTexture(DBG_NEW Texture(Texture::Type::SpecularMap, "assets/texture/wood.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE, true));

		cube->GetComponent<MeshRenderer>()->SetTextures(cubeTexture);
		cube->GetComponent<MeshRenderer>()->SetTextures(cubeKsTexture);

		cube->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
		cube->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
	



		m_BackGroundObjsList.push_back(sphereObjIron);
		m_BackGroundObjsList.push_back(sphereObjStone);
		m_BackGroundObjsList.push_back(sphereObjPlastic);
		m_BackGroundObjsList.push_back(sphereObjRust);
		m_BackGroundObjsList.push_back(cube);
		m_ShadowObjsList.push_back(sphereObjIron);
		m_ShadowObjsList.push_back(sphereObjStone);
		m_ShadowObjsList.push_back(sphereObjPlastic);
		m_ShadowObjsList.push_back(sphereObjRust);
		m_ShadowObjsList.push_back(cube);
	}

	void Layer::LoadSwordScene()
	{
		Object* sword = CreateModel("assets/models/sword/OBJ/Big_Sword_OBJ.obj", "assets/shaders/pbr/PbrTexture.glsl", false, "Sword");
		std::shared_ptr<Texture> SwordalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/models/sword/textures/Big Sword_Base_Color_Map.jpg"));
		std::shared_ptr<Texture> SwordaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/models/sword/textures/Big Sword_AO_Map.jpg"));
		std::shared_ptr<Texture> SwordroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/models/sword/textures/Big Sword_Roughness_Map.jpg"));
		std::shared_ptr<Texture> SwordmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/models/sword/textures/Big Sword_Metalness.jpg"));
		std::shared_ptr<Texture> SwordnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/models/sword/textures/Big Sword_Normal_Map.jpg"));
		std::shared_ptr<Texture> SwordemissionTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::EmissionMap, "assets/models/sword/textures/Big Sword_Emission_Map.jpg"));

		sword->GetComponent<MeshRenderer>()->SetTextures(SwordalbedoTexture);
		sword->GetComponent<MeshRenderer>()->SetTextures(SwordaoTexture);
		sword->GetComponent<MeshRenderer>()->SetTextures(SwordroughnessTexture);
		sword->GetComponent<MeshRenderer>()->SetTextures(SwordmentallicTexture);
		sword->GetComponent<MeshRenderer>()->SetTextures(SwordnormalTexture);
		sword->GetComponent<MeshRenderer>()->SetTextures(SwordemissionTexture);

		sword->GetComponent<MeshRenderer>()->SetPBRTextureSamples(true);
		sword->GetComponent<MeshRenderer>()->SetIsPBRObject(true);
	//	sword->GetComponent<MeshRenderer>()->SetTextureSamples(true);//TODO::
		sword->GetComponent<MeshRenderer>()->SetTextureDiffuseSamples(true);
	//	sword->GetComponent<MeshRenderer>()->SetTextureMetallicSamples(true);
		sword->GetComponent<MeshRenderer>()->SetTexturEmissionSamples(true);

		sword->GetComponent<MeshRenderer>()->SetIsPBRObject(true);

		m_BackGroundObjsList.push_back(sword);

	}

	void Layer::LoadCubesScene()
	{
		float width = 4;
		float height = width;

		float num = 16;

		float cubeSize = width / num;
		for (int i = 0; i < num; i++)
		{
			for (int j = 0; j < num; j++) {

				for (int k = 0; k < num; k++) {
					Object* cube = CreateCube();
					cube->GetComponent<Transform>()->SetInitPosition({ cubeSize *i,cubeSize *j,cubeSize*k });
					//cube->GetComponent<Transform>()->SetRotation({0.0f,45.0f, 0.0f});

					cube->GetComponent<Transform>()->SetScale({ 0.5f*cubeSize, 0.5f * cubeSize, 0.5f * cubeSize });
					//std::shared_ptr<Texture> cubeTexture(DBG_NEW Texture(Texture::Type::DiffuseMap, "assets/texture/wood.png"));
					cube->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ (1.0f/num)*i,(1.0f / num) *j,(1.0f / num)*k });
					cube->GetComponent<MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0.0f,0.0f,0.0f });

					//cube->GetComponent<MeshRenderer>()->SetTextureSamples(false);
					cube->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
					m_BackGroundObjsList.push_back(cube);
				}
			}

		}
	}

	Object* Layer::LoadStaticBackGroundObject(const std::string modelName)
	{
		Object* staticModel = nullptr;

		if (modelName == "House") {
			//house model
			staticModel = CreateModel("assets/models/Alpine/Alpine_chalet.obj", "assets/shaders/IronMan.glsl", false, "House");
			std::shared_ptr<BlackPearl::Texture> housealbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/models/Alpine/Diffuse_map.png"));
			std::shared_ptr<BlackPearl::Texture> houseroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/models/Alpine/Roughness_map.png"));
			std::shared_ptr<BlackPearl::Texture> housementallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/models/Alpine/Metallic_map.png"));
			std::shared_ptr<BlackPearl::Texture> housenormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/models/Alpine/Normal_map.png"));
			std::shared_ptr<BlackPearl::Texture> houseaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/models/Alpine/Ao_map.png"));

			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(housealbedoTexture);
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(houseroughnessTexture);
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(housementallicTexture);
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(housenormalTexture);
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(houseaoTexture);


			staticModel->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(2.0f));
			staticModel->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 0.0f,-1.5f,1.0f });
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetIsPBRObject(true);
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);


		}

		else if (modelName=="Church") {
			staticModel =  CreateModel("assets/models/sponza_obj/sponza.obj", "assets/shaders/IronMan.glsl",false,"Church");
			staticModel->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(0.001f));
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		}
		else if (modelName == "Bunny") {
			staticModel=CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny");
			staticModel->GetComponent<Transform>()->SetScale(glm::vec3(0.5));
			staticModel->GetComponent<Transform>()->SetInitPosition({ 0.6f,0.0f,3.0f });
			staticModel->GetComponent<Transform>()->SetRotation({ 0.0f,-30.0f,0.0f });
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		}
		else if (modelName == "Deer") {
			staticModel=CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl", false, "Deer");
			staticModel->GetComponent<Transform>()->SetScale(glm::vec3(0.003));
			staticModel->GetComponent<Transform>()->SetInitPosition({ -0.5f,0.0f,2.5f });
			staticModel->GetComponent<Transform>()->SetRotation({ 0.0f,68.0f,0.0f });
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);

		}
		else if (modelName == "Sphere") {
			staticModel = CreateSphere(1.5, 64, 64);
			staticModel->GetComponent<Transform>()->SetScale(glm::vec3(0.003));
			staticModel->GetComponent<Transform>()->SetInitPosition({ -0.5f,0.0f,2.5f });
			staticModel->GetComponent<Transform>()->SetRotation({ 0.0f,68.0f,0.0f });
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
			std::shared_ptr<Texture> PlasticalbedoTexture(DBG_NEW Texture(Texture::Type::DiffuseMap, "assets/texture/pbr/plasticSphere/scuffed-plastic4-alb.png"));
			std::shared_ptr<Texture> PlasticaoTexture(DBG_NEW Texture(Texture::Type::AoMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-ao.png"));
			std::shared_ptr<Texture> PlasticroughnessTexture(DBG_NEW Texture(Texture::Type::RoughnessMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-rough.png"));
			std::shared_ptr<Texture> PlasticmentallicTexture(DBG_NEW Texture(Texture::Type::MentallicMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-metal.png"));
			std::shared_ptr<Texture> PlasticnormalTexture(DBG_NEW Texture(Texture::Type::NormalMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-normal.png"));
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

			std::shared_ptr<Texture> IronalbedoTexture(DBG_NEW Texture(Texture::Type::DiffuseMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_basecolor.png"));
			std::shared_ptr<Texture> IronaoTexture(DBG_NEW Texture(Texture::Type::AoMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_ao.png"));
			std::shared_ptr<Texture> IronroughnessTexture(DBG_NEW Texture(Texture::Type::RoughnessMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_roughness.png"));
			std::shared_ptr<Texture> IronmentallicTexture(DBG_NEW Texture(Texture::Type::MentallicMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_metallic.png"));
			std::shared_ptr<Texture> IronnormalTexture(DBG_NEW Texture(Texture::Type::NormalMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_normal.png"));

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
		}
		else {
			GE_CORE_ERROR("no such name:" + modelName + "!")
		}
		m_BackGroundObjsList.push_back(staticModel);
		return staticModel;
	}

	Object* Layer::LoadDynamicObject(const std::string modelName)
	{
		Object* dynamicModel = nullptr;
		if (modelName == "Boy") {
			dynamicModel = CreateModel("assets/models-animation/people/character Texture.dae", "assets/shaders/animatedModel/animatedModel.glsl", true,"Boy");
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetScale({ 0.2f,0.2f,0.2f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetRotation({ -90.0f,0.0f,0.0f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 3.0f,-1.6f,0.0f });

		}
		else if (modelName == "Robot") {
			dynamicModel = CreateModel("assets/models-animation/56-sphere-bot-basic/Sphere-Bot Basic/Armature_001-(COLLADA_3 (COLLAborative Design Activity)).dae", "assets/shaders/animatedModel/animatedModel.glsl", true, "Robot");
			std::shared_ptr<BlackPearl::Texture> RobotAoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/models-animation/56-sphere-bot-basic/Sphere-Bot Basic/Sphere_Bot_ao.jpg"));
			std::shared_ptr<BlackPearl::Texture> RobotRoughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/models-animation/56-sphere-bot-basic/Sphere-Bot Basic/Sphere_Bot_rough.jpg"));
			dynamicModel->GetComponent<BlackPearl::MeshRenderer>()->SetTexture(1,RobotAoTexture);
			dynamicModel->GetComponent<BlackPearl::MeshRenderer>()->SetTexture(1,RobotRoughnessTexture);

		}
		else if (modelName == "Frog") {
			dynamicModel = CreateModel("assets/models-animation/frog/frog.dae.txt", "assets/shaders/animatedModel/animatedModel.glsl", true, "Frog");
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetScale({ 0.5f,0.5f,0.5f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetRotation({ 90.0f,180.0f,0.0f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitPosition({ -3.0f,-1.6f,0.0f });
		}
		else if (modelName == "Cleaner") {
			dynamicModel = CreateModel("assets/models-animation/boblampclean.md5mesh", "assets/shaders/animatedModel/animatedModel.glsl", true, "Cleaner");
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetScale({ 0.05f,0.05f,0.05f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetRotation({ 90.0f,180.0f,180.0f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 0.0f,-1.6f,0.0f });
		}
		else {
			GE_CORE_ERROR("no such name:"+modelName+"!")
		}
		m_DynamicObjsList.push_back(dynamicModel);
		return dynamicModel;
		//		m_DynamicObjsList.push_back(m_AnimatedModelRobot);

	}

	Object* Layer::CreateEmpty(std::string name) {

		Object* obj = m_ObjectManager->CreateEmpty(name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateLight(LightType type, const std::string& name)
	{
		Object* obj = m_ObjectManager->CreateLight(type, m_LightSources, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateCube(const std::string& shaderPath, const std::string& texturePath, const std::string& name) //TODO:
	{
		Object* obj = m_ObjectManager->CreateCube(shaderPath, texturePath, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string& shaderPath, const std::string& texturePath, const std::string& name)
	{
		Object* obj = m_ObjectManager->CreateSphere(radius, stackCount, sectorCount, shaderPath, texturePath, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreatePlane(const std::string& shaderPath, const std::string& texturePath, const std::string& name)
	{
		Object* obj = m_ObjectManager->CreatePlane(shaderPath, texturePath, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateSkyBox(const std::vector<std::string>& textureFaces, const std::string& shaderPath, const std::string& name)
	{
		Object* obj = m_ObjectManager->CreateSkyBox(textureFaces, shaderPath, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateQuad(const std::string& shaderPath, const std::string& texturePath, const std::string& name)
	{
		Object* obj = m_ObjectManager->CreateQuad(shaderPath, texturePath, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateLightProbe(ProbeType type,const std::string& shaderPath, const std::string& texturePath,  const std::string& name)
	{
	
		Object* probe = m_ObjectManager->CreateLightProbe(type,shaderPath, texturePath, name+(type == ProbeType::DIFFUSE_PROBE  ? "_kd" : "_ks"));
		m_ObjectsList.push_back(probe);
		//m_ObjectsList.push_back(probe->GetCamera()->GetObj());

		return probe;
	}
	Object* Layer::CreateProbeGrid(MapManager* mapManager, ProbeType type, glm::vec3 probeNums, glm::vec3 offsets, float space)
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
					unsigned int areaId = mapManager->AddProbeIdToArea(probePos, idx);
					probe->GetComponent<LightProbe>()->SetAreaId(areaId);
					idx++;
					(type == ProbeType::DIFFUSE_PROBE) ? m_DiffuseLightProbes.push_back(probe) : m_ReflectionLightProbes.push_back(probe);
					obj->AddChildObj(probe);

				}

			}

		}
		return obj;
	}
	Object* Layer::CreateModel(const std::string& modelPath, const std::string& shaderPath, const bool isAnimated, const std::string& name)
	{
		Object* obj = m_ObjectManager->CreateModel(modelPath, shaderPath, isAnimated, name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	MainCamera* Layer::CreateCamera(const std::string& name) {

		MainCamera* mainCamera = m_ObjectManager->CreateCamera(name);
		m_ObjectsList.push_back(mainCamera->GetObj());
		return mainCamera;
	}

	//void Layer::ShowShader(static std::string &imguiShaders, static char* shader, Mesh & mesh, int meshIndex, static  int &itemIndex)
	//{

	//	std::string buttonName = "select file##" + std::to_string(meshIndex);
	//	std::string inputTextName = "shader##" + std::to_string(meshIndex);

	//	imguiShaders = mesh.GetMaterial()->GetShader()->GetPath();
	//	shader = const_cast<char*>(imguiShaders.c_str());
	//	ImGui::InputText(inputTextName.c_str(), shader, IM_ARRAYSIZE(shader));
	//	ImGui::SameLine();
	//	if (ImGui::Button(buttonName.c_str())) {
	//		itemIndex = meshIndex;
	//		m_fileDialog.Open();
	//	}


	//}
	void Layer::ShowCamera(PerspectiveCamera* perspectiveCamera)
	{
		ImGui::Text("Yaw = %f,Pitch= %f", perspectiveCamera->Yaw(), perspectiveCamera->Pitch());
		ImGui::Text("ProjectionViewMatrix[0].x = %f,ProjectionViewMatrix[1].x = %f", perspectiveCamera->GetViewProjectionMatrix()[0].x
			, perspectiveCamera->GetViewProjectionMatrix()[1].x);
		ImGui::Text("ProjectionViewMatrix[2].x = %f,ProjectionViewMatrix[3].x = %f", perspectiveCamera->GetViewProjectionMatrix()[2].x, perspectiveCamera->GetViewProjectionMatrix()[3].x);
		ImGui::Text("Position.x = %f,Position.y = %f,Position.z = %f", perspectiveCamera->GetPosition().x, perspectiveCamera->GetPosition().y, perspectiveCamera->GetPosition().z);
		ImGui::Text("Front.x = %f,Front.y = %f,Front.z = %f", perspectiveCamera->Front().x, perspectiveCamera->Front().y, perspectiveCamera->Front().z);
		ImGui::Text("Up.x = %f,Up.y = %f,Up.z = %f", perspectiveCamera->Up().x, perspectiveCamera->Up().y, perspectiveCamera->Up().z);
		ImGui::Text("Znear = %f,Zfar = %f,Fov = %f", perspectiveCamera->GetZnear(), perspectiveCamera->GetZfar(), perspectiveCamera->GetFov());

		ImGui::DragFloat("CameraMoveSpeed", &m_CameraMoveSpeed, 0.1f, 0.1, 30);
		ImGui::DragFloat("CameraRotateSpeed", &m_CameraRotateSpeed, 0.1f, 0.1, 30);




	}

	void Layer::ShowShader(std::string imguiShaders, int meshIndex, static  int& itemIndex, int offset)
	{

		std::string buttonName = "select file##" + std::to_string(meshIndex + offset);
		std::string inputTextName = "mesh" + std::to_string(meshIndex + offset);

		//imguiShaders = mesh.GetMaterial()->GetShader()->GetPath();
	//	ImGui::PushID(meshIndex);

		char* shader = const_cast<char*>(imguiShaders.c_str());
		ImGui::InputText(inputTextName.c_str(), shader, IM_ARRAYSIZE(shader));
		ImGui::SameLine();
		if (ImGui::Button(buttonName.c_str())) {
			itemIndex = meshIndex;
			m_fileDialog.Open();
		}

		//	ImGui::PopID();


	}


	void Layer::ShowTextures(std::string imguiShaders, int meshIndex, static  int& itemIndex, Texture::Type textureType, static Texture::Type& type, int offset)
	{

		std::string buttonName = ""; //+std::to_string(meshIndex + offset);
		std::string inputTextName = "";// +std::to_string(meshIndex + offset);

		switch (textureType) {
		case Texture::Type::DiffuseMap:
			buttonName += "diffuseMap";
			inputTextName += "diffuseMap";
			break;
		case Texture::Type::SpecularMap:
			buttonName += "specularMap";
			inputTextName += "specularMap";
			break;
		case Texture::Type::EmissionMap:
			buttonName += "emissionMap";
			inputTextName += "emissionMap";
			break;
		case Texture::Type::HeightMap:
			buttonName += "heightMap";
			inputTextName += "heightMap";
			break;
		case Texture::Type::NormalMap:
			buttonName += "normalMap";
			inputTextName += "normalMap";
			break;
		case Texture::Type::CubeMap:
			buttonName += "cubeMap";
			inputTextName += "cubeMap";
			break;
		case Texture::Type::DepthMap:
			buttonName += "depthMap";
			inputTextName += "depthMap";
			break;
		case Texture::Type::AoMap:
			buttonName += "aoMap";
			inputTextName += "aoMap";
			break;
		case Texture::Type::RoughnessMap:
			buttonName += "roughnessMap";
			inputTextName += "roughnessMap";
			break;
		case Texture::Type::MentallicMap:
			buttonName += "mentallicMap";
			inputTextName += "mentallicMap";
			break;

		}


		//imguiShaders = mesh.GetMaterial()->GetShader()->GetPath();
		//ImGui::PushID(meshIndex);


		inputTextName += "##" + std::to_string(meshIndex);
		buttonName += "##" + std::to_string(meshIndex);

		if (imguiShaders.size() == 0)
			ImGui::Text("                    ");
		else
			ImGui::Text(imguiShaders.c_str());



		ImGui::SameLine();

		if (ImGui::Button(buttonName.c_str())) {
			itemIndex = meshIndex;
			type = textureType;
			m_fileDialog.Open();
		}




		//ImGui::PopID();

	}




	void Layer::ShowMaterialProps(Material::Props& imGuiProps)
	{
	}

	void Layer::ShowMeshRenderer(MeshRenderer* comp)
	{


		ImGui::Text("MeshRenderer");
		std::vector<Mesh>& imGuiMeshes = comp->GetMeshes();
		if (imGuiMeshes.empty()) return;

		int offset = 0;

		ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "Shader");
		std::vector<std::string> imguiShaders;
		static int itemIndex = -1;
		//std::cout << "itemIndex" << itemIndex <<std::endl;
		imguiShaders.resize(imGuiMeshes.size());
		for (int i = 0; i < imGuiMeshes.size(); i++)
		{
			imguiShaders[i] = imGuiMeshes[i].GetMaterial()->GetShader()->GetPath();
			ShowShader(imguiShaders[i], i, itemIndex, offset);
		}
		if (itemIndex != -1) {
			if (m_fileDialog.HasSelected()) {

				imGuiMeshes[itemIndex].GetMaterial()->SetShader(m_fileDialog.GetSelected().string());
				m_fileDialog.ClearSelected();
				itemIndex = -1;
			}


		}

		if (imGuiMeshes[0].GetMaterial()->GetTextureMaps() != nullptr) {

			ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "TextureMaps");
			std::vector<std::string> imguiDiffuseTextures(imGuiMeshes.size());
			std::vector<std::string> imguiSpecularTextures(imGuiMeshes.size());
			std::vector<std::string> imguiEmissionTextures(imGuiMeshes.size());
			std::vector<std::string> imguiNormalTextures(imGuiMeshes.size());
			std::vector<std::string> imguiCubeTextures(imGuiMeshes.size());
			std::vector<std::string> imguiHeightTextures(imGuiMeshes.size());
			std::vector<std::string> imguiDepthTextures(imGuiMeshes.size());


			static  int itemIndexTexture = -1;
			static Texture::Type type;
			//std::cout << "itemIndexTexture" << itemIndexTexture << std::endl;
			//GE_CORE_TRACE("itemIndexTexture:"+std::to_string(itemIndexTexture) );
			for (int i = 0; i < imGuiMeshes.size(); i++)
			{
				std::string text = "Mesh" + std::to_string(i);
				ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, text.c_str());
				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->diffuseTextureMap != nullptr) {
					imguiDiffuseTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->diffuseTextureMap->GetPath();
				}
				ShowTextures(imguiDiffuseTextures[i], i, itemIndexTexture, Texture::DiffuseMap, type, imGuiMeshes.size() * 2);


				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->specularTextureMap != nullptr)
					imguiSpecularTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->specularTextureMap->GetPath();
				ShowTextures(imguiSpecularTextures[i], i, itemIndexTexture, Texture::SpecularMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->emissionTextureMap != nullptr)
					imguiEmissionTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->emissionTextureMap->GetPath();
				ShowTextures(imguiEmissionTextures[i], i, itemIndexTexture, Texture::EmissionMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->normalTextureMap != nullptr)
					imguiNormalTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->normalTextureMap->GetPath();
				ShowTextures(imguiNormalTextures[i], i, itemIndexTexture, Texture::NormalMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->cubeTextureMap != nullptr)
					imguiCubeTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->cubeTextureMap->GetPath();
				ShowTextures(imguiCubeTextures[i], i, itemIndexTexture, Texture::CubeMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->heightTextureMap != nullptr)
					imguiHeightTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->heightTextureMap->GetPath();
				ShowTextures(imguiHeightTextures[i], i, itemIndexTexture, Texture::HeightMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->depthTextureMap != nullptr)
					imguiDepthTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->depthTextureMap->GetPath();
				ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, Texture::DepthMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->aoMap != nullptr)
					imguiDepthTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->aoMap->GetPath();
				ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, Texture::AoMap, type, imGuiMeshes.size() * 2);


				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->roughnessMap != nullptr)
					imguiDepthTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->roughnessMap->GetPath();
				ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, Texture::RoughnessMap, type, imGuiMeshes.size() * 2);


				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->mentallicMap != nullptr)
					imguiDepthTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->mentallicMap->GetPath();
				ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, Texture::MentallicMap, type, imGuiMeshes.size() * 2);
			}
			if (itemIndexTexture != -1) {
				if (m_fileDialog.HasSelected()) {

					switch (type) {
					case Texture::DiffuseMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::DiffuseMap, m_fileDialog.GetSelected().string());//"assets/texture/" + 
						break;
					case Texture::SpecularMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::SpecularMap, m_fileDialog.GetSelected().string());//"assets/texture/" +
						break;
					case Texture::EmissionMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::EmissionMap, m_fileDialog.GetSelected().string());
						break;
					case Texture::NormalMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::NormalMap, m_fileDialog.GetSelected().string());
						break;
					case Texture::CubeMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::CubeMap, m_fileDialog.GetSelected().string());
						break;
					case Texture::HeightMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::HeightMap, m_fileDialog.GetSelected().string());
						break;
					case Texture::DepthMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::DepthMap, m_fileDialog.GetSelected().string());
						break;

					case Texture::AoMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::AoMap, m_fileDialog.GetSelected().string());
						break;

					case Texture::RoughnessMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::RoughnessMap, m_fileDialog.GetSelected().string());
						break;

					case Texture::MentallicMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::MentallicMap, m_fileDialog.GetSelected().string());
						break;
					defalut:
						GE_CORE_ERROR("Unknown texture type");
					}

					//imguiTextures[itemIndexTexture] = m_fileDialog.GetSelected().string();
					////GE_CORE_INFO("Selected filename:" + m_fileDialog.GetSelected().string());
					//textures[itemIndexTexture] = const_cast<char*>(imguiTextures[itemIndexTexture].c_str());// (m_fileDialog.GetSelected().string().c_str());
					//imGuiMeshes[itemIndexTexture].GetMaterial()->SetShader("assets/texture/" + imguiTextures[itemIndexTexture]);
					m_fileDialog.ClearSelected();
					itemIndexTexture = -1;
				}

			}
		}





		ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "Material Properties");
		Material::Props& imGuiProps = imGuiMeshes[0].GetMaterial()->GetProps();//TODO::默认所有mesh 的Material::Props 是一样的
		float imGuiShininess = imGuiProps.shininess;
		bool  imGUiBlinnLight = imGuiProps.isBinnLight;
		bool  imGUiIsPBRTextureSample = (bool)imGuiProps.isPBRTextureSample;
		bool  imGUiIsDifussTextureSample = (bool)imGuiProps.isDiffuseTextureSample;
		bool  imGUiIsSpecularTextureSample = (bool)imGuiProps.isSpecularTextureSample;
		//bool  imGUiIsMetallicrTextureSample = (bool)imGuiProps.isMetallicTextureSample;

		ImGui::Checkbox("blinnlight", &imGUiBlinnLight);
		for (auto mesh : imGuiMeshes)
			mesh.GetMaterial()->SetBinnLight(imGUiBlinnLight);

		ImGui::DragFloat("shininess", &imGuiShininess, 0.5f, 0.0f, 1024.0f, "%.3f ");
		for (auto mesh : imGuiMeshes)
			mesh.GetMaterial()->SetShininess(imGuiShininess);


		ImGui::Checkbox("usePPBRTexture", &imGUiIsPBRTextureSample);
		for (auto mesh : imGuiMeshes)
			mesh.GetMaterial()->SetPBRTextureSample((int)imGUiIsPBRTextureSample);

		ImGui::Checkbox("useDiffuseTexture", &imGUiIsDifussTextureSample);
		for (auto mesh : imGuiMeshes)
			mesh.GetMaterial()->SetTextureSampleDiffuse((int)imGUiIsDifussTextureSample);

		ImGui::Checkbox("useSpecularTexture", &imGUiIsSpecularTextureSample);
		for (auto mesh : imGuiMeshes)
			mesh.GetMaterial()->SetTextureSampleSpecular((int)imGUiIsSpecularTextureSample);

		/*ImGui::Checkbox("useMetallicTexture", &imGUiIsMetallicrTextureSample);
		for (auto mesh : imGuiMeshes)
			mesh.GetMaterial()->SetTextureSampleMetallic((int)imGUiIsMetallicrTextureSample);*/

		for (auto mesh : imGuiMeshes) {
			MaterialColor::Color color = mesh.GetMaterial()->GetMaterialColor().Get();
			ImGui::ColorEdit3("diffuseColor", glm::value_ptr(color.diffuseColor));
			mesh.GetMaterial()->SetMaterialColorDiffuseColor(color.diffuseColor);

			ImGui::ColorEdit3("specularColor", glm::value_ptr(color.specularColor));
			mesh.GetMaterial()->SetMaterialColorSpecularColor(color.specularColor);

			ImGui::ColorEdit3("emissionColor", glm::value_ptr(color.emissionColor));
			mesh.GetMaterial()->SetMaterialColorEmissionColor(color.emissionColor);

		}


	}





	void Layer::ShowTransform(Transform* comp,Object* obj)
	{
		ImGui::Text("Transform");

		float pos[] = { comp->GetPosition().x,comp->GetPosition().y,comp->GetPosition().z };
		ImGui::DragFloat3("position", pos, 0.05f, -100.0f, 100.0f, "%.3f ");
		obj->SetPosition({ pos[0],pos[1],pos[2] });

		float scale[] = { comp->GetScale().x,comp->GetScale().y,comp->GetScale().z };
		ImGui::DragFloat3("scale", scale, 0.05f, 0.001f, 100.0f, "%.3f ");
		obj->SetScale({ scale[0],scale[1],scale[2] });

		float rotate[] = { comp->GetRotation().x,comp->GetRotation().y,comp->GetRotation().z };
		ImGui::DragFloat3("rotation", rotate, 0.05f, -360.0f, 360.0f, "%.3f ");
		obj->SetRotation({ rotate[0],rotate[1],rotate[2] });

	}

	void Layer::ShowPointLight(PointLight* pointLight)
	{
		//if (comp->GetType() == LightType::PointLight) {
			//auto pointLight = std::dynamic_pointer_cast<PointLight>(comp);
		//auto color = pointLight->GetMeshes().GetMaterial()->GetMaterialColor().Get();
		auto props = pointLight->GetLightProps();
		static  int attenuation = (int)pointLight->GetAttenuation().maxDistance;
		float intensity = pointLight->GetLightProps().intensity;
		ImGui::ColorEdit3("ambient Color", glm::value_ptr(props.ambient));
		ImGui::ColorEdit3("diffuse Color", glm::value_ptr(props.diffuse));
		ImGui::ColorEdit3("specular Color", glm::value_ptr(props.specular));
		ImGui::ColorEdit3("emission Color", glm::value_ptr(props.emission));
		ImGui::DragInt("attenuation", &attenuation, 0.5f, 7, 3250);
		ImGui::DragFloat("intensity", &intensity, 1.0f, 1, 100);

		pointLight->SetAttenuation(attenuation);

		pointLight->UpdateMesh({ props.ambient ,props.diffuse,props.specular,props.emission,intensity });
		//	}
		/*	else if (comp->GetType() == LightType::ParallelLight) {

			}
			else if (comp->GetType() == LightType::SpotLight) {


			}*/
	}

	void Layer::ShowParallelLight(ParallelLight* parallelLight)
	{
		auto props = parallelLight->GetLightProps();
		glm::vec3 direction = parallelLight->GetDirection();

		float dir[] = { direction.x,direction.y,direction.z };
		ImGui::DragFloat3("position", dir, 0.05f,0.0f, 1.0f, "%.3f ");
		parallelLight->SetDirection({ dir[0],dir[1],dir[2] });

		float intensity = parallelLight->GetLightProps().intensity;
		ImGui::DragFloat3("direction", glm::value_ptr(props.emission));

		ImGui::ColorEdit3("ambient Color", glm::value_ptr(props.ambient));
		ImGui::ColorEdit3("diffuse Color", glm::value_ptr(props.diffuse));
		ImGui::ColorEdit3("specular Color", glm::value_ptr(props.specular));
		ImGui::ColorEdit3("emission Color", glm::value_ptr(props.emission));
		ImGui::DragFloat("intensity", &intensity, 1.0f, 1, 100);


		parallelLight->UpdateMesh({ props.ambient ,props.diffuse,props.specular,props.emission,intensity });
	}




	std::vector<Object*> Layer::GetObjects()
	{
		return m_ObjectManager->GetObjects();
	}
	std::vector<std::string> Layer::GetObjectsName()
	{
		return m_ObjectManager->GetObjectsName();
	}
	void Layer::DestroyObjects() //TODO:删除某一个Objects的情况还没处理--》重新考虑m_EntityToObjects数据结构
	{
		m_ObjectManager->DestroyObjects();
	}
}