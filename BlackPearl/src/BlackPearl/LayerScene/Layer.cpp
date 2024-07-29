#include "pch.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/LayerScene/Layer.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/LightProbeComponent/LightProbeComponent.h"
#include "BlackPearl/Component/TerrainComponent/TerrainComponent.h"
#include "BlackPearl/Renderer/Model/Model.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "BlackPearl/Renderer/MasterRenderer/IBLRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/IBLProbesRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/ShadowMapPointLightRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/GBufferRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/VoxelConeTracingRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/VoxelConeTracingDeferredRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/VoxelConeTracingSVORenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/CloudRenderer.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Renderer/Buffer/D3D12Buffer/D3D12Buffer.h"

namespace BlackPearl {
	static int buttonNum = 0;
	void Layer::OnImguiRender()
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Suqare Color", (m_BackgroundColor));
		ImGui::End();
		ImGui::Begin("GI Settings");
		//ImGui::Text("FPS = %.3lf", Application::s_AppFPS);
		//ImGui::Text("AvgFPS = %.3lf", Application::s_AppAverageFPS);
	//	ImGui::Separator();
		ImGui::Text("SVO voxel GI");
		ImGui::Checkbox("spp pause", &VoxelConeTracingSVORenderer::s_Pause);
		ImGui::Checkbox("svo direct light", &VoxelConeTracingSVORenderer::s_DirectLight);
		ImGui::Checkbox("svo Indirect diffuse light", &VoxelConeTracingSVORenderer::s_IndirectDiffuseLight);
		ImGui::Checkbox("svo Indirect specular light", &VoxelConeTracingSVORenderer::s_IndirectSpecularLight);
		ImGui::DragFloat("svo GICoeffs", &VoxelConeTracingSVORenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
		ImGui::DragFloat("specularAngle", &VoxelConeTracingSVORenderer::s_IndirestSpecularAngle, 0.2f, 0.01f, 45.0f, "%.4f ");
		ImGui::DragFloat("specularStep", &VoxelConeTracingSVORenderer::s_Step, 0.01f, 0.01f, 45.0f, "%.4f ");
		ImGui::Separator();

		ImGui::Text("Deferred voxel GI");
		ImGui::Checkbox("Voxelize", &VoxelConeTracingDeferredRenderer::s_VoxelizeNow);

		ImGui::Checkbox("Indirect diffuse", &VoxelConeTracingDeferredRenderer::s_IndirectDiffuseLight);
		ImGui::Checkbox("Indirect specular", &VoxelConeTracingDeferredRenderer::s_IndirectSpecularLight);
		ImGui::Checkbox("Direct light", &VoxelConeTracingDeferredRenderer::s_DirectLight);
		ImGui::Checkbox("Shadows", &VoxelConeTracingDeferredRenderer::s_Shadows);

		ImGui::Checkbox("HDR", &VoxelConeTracingDeferredRenderer::s_HDR);
		/*ImGui::Checkbox("voxel blur horizontal", &VoxelConeTracingDeferredRenderer::s_GuassianHorizontal);
		ImGui::Checkbox("voxel blur vertical", &VoxelConeTracingDeferredRenderer::s_GuassianVertical);
		ImGui::Checkbox("voxel blur showBlurArea", &VoxelConeTracingDeferredRenderer::s_ShowBlurArea);*/
		ImGui::Checkbox("Blur mipmap", &VoxelConeTracingDeferredRenderer::s_MipmapBlurSpecularTracing);
		ImGui::DragFloat("Specular\nBlur\nThreshold", &VoxelConeTracingDeferredRenderer::s_SpecularBlurThreshold, 0.2f, 0.0f, 1.0f, "%.4f ");
		ImGui::DragFloat("Indirect\nSpecular\nAngle", &VoxelConeTracingDeferredRenderer::s_IndirectSpecularAngle, 0.2f, 1.0f, 45.0f, "%.4f ");
		ImGui::DragFloat("GICoeffs", &VoxelConeTracingDeferredRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
		ImGui::DragInt("Visualization \nmipmap\nlevel", &VoxelConeTracingDeferredRenderer::s_VisualizeMipmapLevel, 1.0f, 0, 5);



		ImGui::Separator();
		ImGui::Text("forward voxel GI");
		ImGui::Checkbox("voxel Indirect diffuse", &VoxelConeTracingRenderer::s_IndirectDiffuseLight);
		ImGui::Checkbox("voxel Indirect specular", &VoxelConeTracingRenderer::s_IndirectSpecularLight);
		ImGui::Checkbox("voxel direct light", &VoxelConeTracingRenderer::s_DirectLight);
		ImGui::DragFloat("voxel GICoeffs", &VoxelConeTracingRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
		ImGui::Checkbox("voxel HDR", &VoxelConeTracingRenderer::s_HDR);
		ImGui::Separator();
		ImGui::Text("light probe GI");
		ImGui::DragFloat("GICoeffs", &GBufferRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
		ImGui::Checkbox("HDR", &GBufferRenderer::s_HDR);

		ImGui::Separator();



		ImGui::Text("image based lighting  GI");
		ImGui::DragFloat("IBL GICoeffs", &IBLRenderer::s_GICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");
		ImGui::Checkbox("IBL HDR", &GBufferRenderer::s_HDR);

		ImGui::Text("SSR GI");
		ImGui::DragFloat("SSRGICoeffs", &GBufferRenderer::s_SSRGICoeffs, 0.2f, 0.0f, 1.0f, "%.3f ");


		ImGui::End();

		ImGui::Begin("Cloud Settings");
		//ImGui::Text("FPS = %.3lf", Application::s_AppFPS);
		//ImGui::Text("AvgFPS = %.3lf", Application::s_AppAverageFPS);
	//	ImGui::Separator();
	
		ImGui::DragFloat("rayStep", &CloudRenderer::s_rayStep, 0.2f, 0.0f, 1.0f, "%.4f ");
		ImGui::DragFloat("step", &CloudRenderer::s_step, 0.2f, 1.0f, 45.0f, "%.4f ");
		ImGui::DragFloat("colorOffset1", &CloudRenderer::s_colorOffset1, 0.2f, 0.0f, 1.0f, "%.3f ");
		ImGui::DragFloat("colorOffset2", &CloudRenderer::s_colorOffset2, 0.0f, 0, 1.0f, "%.3f ");
		ImGui::DragFloat("s_densityOffset", &CloudRenderer::s_densityOffset, 0.0f, -1.0f, 1.0f, "%.3f ");
		ImGui::DragFloat("s_lightAbsorptionTowardSun", &CloudRenderer::s_lightAbsorptionTowardSun, 1.0f, 0.0f, 100.0f, "%.3f ");
		ImGui::DragFloat("s_densityMultiplier", &CloudRenderer::s_densityMultiplier, 1.0f, 0.0f, 100.0f, "%.3f ");

		float boxMax[] = { CloudRenderer::s_boundsMax.x, CloudRenderer::s_boundsMax.y, CloudRenderer::s_boundsMax.z };
		float boxMin[] = { CloudRenderer::s_boundsMin.x, CloudRenderer::s_boundsMin.y, CloudRenderer::s_boundsMin.z };

		ImGui::DragFloat3("s_boundsMax", boxMax, 0.05f, 0.001f, 100.0f, "%.3f ");
		CloudRenderer::s_boundsMax = math::float3(boxMax[0], boxMax[1], boxMax[2]);
		CloudRenderer::s_boundsMin = math::float3(boxMin[0], boxMin[1], boxMin[2]);


	

		ImGui::End();



		ImGui::Begin("Performance");
		ImGui::Text("FPS = %.3lf", Application::s_AppFPS);
		ImGui::Text("AvgFPS = %.3lf", Application::s_AppAverageFPS);
		ImGui::Separator();
		ImGui::Text("s_TotalFrameNum = %d", Application::s_TotalFrameNum);
		ImGui::Text("DrawCalls per frame = %.3lf", (double)BasicRenderer::s_DrawCallCnt);

		//ImGui::Text("DrawCalls per frame = %.3lf", (double)Application::s_TotalFrameNum/BasicRenderer::s_DrawCallCnt);
		ImGui::Text("Objs num = %d", (int)m_ObjectsList.size());
		ImGui::Text("BackGround Objs num = %d", (int)m_BackGroundObjsList.size());

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
					bool is_selected = (currentObj != nullptr && currentObj->GetName() == objsList[n]->GetName());
					if (ImGui::Selectable(objsList[n]->GetName().c_str(), is_selected)) {
						currentObj = objsList[n];
						GE_CORE_INFO(objsList[n]->GetName() + "is selected")
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
				if (currentObj->GetComponent<BasicInfo>()->GetType() == OT_BatchNode) {

				}
				else {
					ShowTransform(currentObj->GetComponent<Transform>(), currentObj);

				}

			}
			if (currentObj->HasComponent< LightProbe>()) {
				ShowLightProbe(currentObj->GetComponent<LightProbe>(), currentObj);

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
			if (currentObj->HasComponent<PerspectiveCamera>()) {
				if (currentObj == m_MainCamera->GetObj()) {
					ShowCamera(m_MainCamera);
				}
				else {
					ShowCamera(currentObj->GetComponent<PerspectiveCamera>());

				}

			}
			if (currentObj->HasComponent<TerrainComponent>()) {
				ShowTerrian(currentObj);

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

		cube1->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube2->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube3->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube4->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube5->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });

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

		Object* deer = CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl", false, "Deer");
		deer->GetComponent<Transform>()->SetInitScale(glm::vec3(0.003));
		deer->GetComponent<Transform>()->SetInitPosition({ -0.5f,0.0f,2.5f });
		deer->GetComponent<Transform>()->SetInitRotation({ 0.0f,68.0f,0.0f });
		deer->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(deer);

		Object* bunny = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny");
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

		Object* deer = CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl", false, "Deer");
		deer->GetComponent<Transform>()->SetInitScale(glm::vec3(0.003));
		//deer->GetComponent<Transform>()->SetPosition({ -0.5f,0.0f,2.5f });
		deer->GetComponent<Transform>()->SetInitPosition({ -0.5f,-1.5f,-0.5f });
		deer->GetComponent<Transform>()->SetInitRotation({ 0.0f,68.0f,0.0f });
		deer->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(deer);
		m_ShadowObjsList.push_back(deer);

		Object* bunny = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny");
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

		cube1->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube2->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube3->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube4->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		cube5->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });

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
		Object* church = CreateModel("assets/models/crytek-sponza/sponza.obj", "assets/shaders/IronMan.glsl", false, "Church");

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
		Object* cube = LoadStaticBackGroundObject("WoodCube");
		//textures spheres
		IDevice* device = m_DeviceManager->GetDevice();
		
		sphereObjIron->GetComponent<Transform>()->SetInitPosition({ 10,0,0 });

		sphereObjRust->GetComponent<Transform>()->SetInitPosition({ 5,0,0 });
		
		sphereObjStone->GetComponent<Transform>()->SetInitPosition({ -5,0,0 });

		sphereObjPlastic->GetComponent<Transform>()->SetInitPosition({ -10.0,0,0 });
		
		cube->GetComponent<Transform>()->SetInitPosition({ -2.0f,-2.5f,0.0f });
		cube->GetComponent<Transform>()->SetInitScale({ 16.0f,0.5f,16.0f });
		
		m_ShadowObjsList.push_back(sphereObjIron);
		m_ShadowObjsList.push_back(sphereObjStone);
		m_ShadowObjsList.push_back(sphereObjPlastic);
		m_ShadowObjsList.push_back(sphereObjRust);
		m_ShadowObjsList.push_back(cube);

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
					cube->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0.0f,0.0f,0.0f });

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
					cube->GetComponent<MeshRenderer>()->GetMeshes()[0]->GetMaterial()->SetMaterialColorSpecularColor({ 0.0f,0.0f,0.0f });

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
			staticModel = CreateModel("assets/models/Alpine/Alpine_chalet.obj", "assets/shaders/IronMan.glsl", false, "House");
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
			staticModel = CreateModel("assets/models/sponza_obj/sponza.obj", "assets/shaders/IronMan.glsl", false, "Church");
			staticModel->GetComponent<BlackPearl::Transform>()->SetInitScale(glm::vec3(0.001f));
			staticModel->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		}
		else if (modelName == "Bunny") {
			staticModel = CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false, "Bunny");
			staticModel->GetComponent<Transform>()->SetInitScale(glm::vec3(0.5));
			staticModel->GetComponent<Transform>()->SetInitPosition({ 0.6f,0.0f,3.0f });
			staticModel->GetComponent<Transform>()->SetInitRotation({ 0.0f,-30.0f,0.0f });
			staticModel->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(true);
		}
		else if (modelName == "Deer") {
			staticModel = CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl", false, "Deer");
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
			Object* sword = CreateModel("assets/models/sword/OBJ/Big_Sword_OBJ.obj", "assets/shaders/pbr/PbrTexture.glsl", false, "Sword");
			TextureHandle SwordalbedoTexture = device->createTexture(TextureDesc(TextureType::DiffuseMap, "assets/models/sword/textures/Big Sword_Base_Color_Map.jpg"));
			TextureHandle SwordaoTexture = device->createTexture(TextureDesc(TextureType::AoMap, "assets/models/sword/textures/Big Sword_AO_Map.jpg"));
			TextureHandle SwordroughnessTexture = device->createTexture(TextureDesc(TextureType::RoughnessMap, "assets/models/sword/textures/Big Sword_Roughness_Map.jpg"));
			TextureHandle SwordmentallicTexture = device->createTexture(TextureDesc(TextureType::MentallicMap, "assets/models/sword/textures/Big Sword_Metalness.jpg"));
			TextureHandle SwordnormalTexture = device->createTexture(TextureDesc(TextureType::NormalMap, "assets/models/sword/textures/Big Sword_Normal_Map.jpg"));
			TextureHandle SwordemissionTexture = device->createTexture(TextureDesc(TextureType::EmissionMap, "assets/models/sword/textures/Big Sword_Emission_Map.jpg"));

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
			GE_CORE_ERROR("no such name:" + modelName + "!")
		}
		m_BackGroundObjsList.push_back(staticModel);
		return staticModel;
	}

	Object* Layer::LoadDynamicObject(const std::string modelName)
	{
		Object* dynamicModel = nullptr;
		if (modelName == "Boy") {
			dynamicModel = CreateModel("assets/models-animation/people/character Texture.dae", "assets/shaders/animatedModel/animatedModel.glsl", true, "Boy");
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitScale({ 0.2f,0.2f,0.2f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitRotation({ -90.0f,0.0f,0.0f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 3.0f,-1.6f,0.0f });

		}
		else if (modelName == "Robot") {

			auto device = m_DeviceManager->GetDevice();
			dynamicModel = CreateModel("assets/models-animation/56-sphere-bot-basic/Sphere-Bot Basic/Armature_001-(COLLADA_3 (COLLAborative Design Activity)).dae", "assets/shaders/animatedModel/animatedModel.glsl", true, "Robot");
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
			dynamicModel = CreateModel("assets/models-animation/frog/frog.dae.txt", "assets/shaders/animatedModel/animatedModel.glsl", true, "Frog");
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitScale({ 0.5f,0.5f,0.5f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitRotation({ 90.0f,180.0f,0.0f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitPosition({ -3.0f,-1.6f,0.0f });
		}
		else if (modelName == "Cleaner") {
			dynamicModel = CreateModel("assets/models-animation/boblampclean.md5mesh", "assets/shaders/animatedModel/animatedModel.glsl", true, "Cleaner");
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitScale({ 0.05f,0.05f,0.05f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitRotation({ 90.0f,180.0f,180.0f });
			dynamicModel->GetComponent<BlackPearl::Transform>()->SetInitPosition({ 0.0f,-1.6f,0.0f });
		}
		else {
			GE_CORE_ERROR("no such name:" + modelName + "!")
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
		(type == ProbeType::DIFFUSE_PROBE) ? m_DiffuseLightProbes.push_back(probe) : m_ReflectionLightProbes.push_back(probe);
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

	void Layer::ShowCamera(PerspectiveCamera* perspectiveCamera)
	{
		ImGui::Text("Yaw = %f,Pitch= %f", perspectiveCamera->Yaw(), perspectiveCamera->Pitch());
		ImGui::Text("ProjectionViewMatrix[0].x = %f,ProjectionViewMatrix[1].x = %f", perspectiveCamera->GetProjectionViewMatrix()[0].x
			, perspectiveCamera->GetProjectionViewMatrix()[1].x);
		ImGui::Text("ProjectionViewMatrix[2].x = %f,ProjectionViewMatrix[3].x = %f", perspectiveCamera->GetProjectionViewMatrix()[2].x, perspectiveCamera->GetProjectionViewMatrix()[3].x);
		ImGui::Text("Position.x = %f,Position.y = %f,Position.z = %f", perspectiveCamera->GetPosition().x, perspectiveCamera->GetPosition().y, perspectiveCamera->GetPosition().z);
		ImGui::Text("Front.x = %f,Front.y = %f,Front.z = %f", perspectiveCamera->Front().x, perspectiveCamera->Front().y, perspectiveCamera->Front().z);
		ImGui::Text("Up.x = %f,Up.y = %f,Up.z = %f", perspectiveCamera->Up().x, perspectiveCamera->Up().y, perspectiveCamera->Up().z);
		ImGui::Text("Znear = %f,Zfar = %f,Fov = %f", perspectiveCamera->GetZnear(), perspectiveCamera->GetZfar(), perspectiveCamera->GetFov());

		float moveSpeed = perspectiveCamera->GetMoveSpeed(), rotSpeed = perspectiveCamera->GetRotateSpeed();
		ImGui::DragFloat("CameraMoveSpeed", &moveSpeed, perspectiveCamera->GetMoveSpeed(), 0.1, 500, "%.3f ");
		perspectiveCamera->SetMoveSpeed(moveSpeed);
		ImGui::DragFloat("CameraRotateSpeed", &rotSpeed, perspectiveCamera->GetRotateSpeed(), 0.1, 500, "%.3f ");
		perspectiveCamera->SetRotateSpeed(rotSpeed);

	}

	void Layer::ShowCamera(MainCamera* mainCamera)
	{
		ImGui::Text("Yaw = %f,Pitch= %f", mainCamera->Yaw(), mainCamera->Pitch());
		ImGui::Text("Position.x = %f,Position.y = %f,Position.z = %f", mainCamera->GetPosition().x, mainCamera->GetPosition().y, mainCamera->GetPosition().z);
		ImGui::Text("Front.x = %f,Front.y = %f,Front.z = %f", mainCamera->Front().x, mainCamera->Front().y, mainCamera->Front().z);
		ImGui::Text("Up.x = %f,Up.y = %f,Up.z = %f", mainCamera->Up().x, mainCamera->Up().y, mainCamera->Up().z);
		ImGui::Text("Znear = %f,Zfar = %f,Fov = %f", mainCamera->ZNear(), mainCamera->ZFar(), mainCamera->Fov());

		float moveSpeed = mainCamera->GetMoveSpeed(), rotSpeed = mainCamera->GetRotateSpeed();
		ImGui::DragFloat("CameraMoveSpeed", &moveSpeed, mainCamera->GetMoveSpeed(), 0.1, 500, "%.3f ");
		mainCamera->SetMoveSpeed(moveSpeed);
		ImGui::DragFloat("CameraRotateSpeed", &rotSpeed, mainCamera->GetRotateSpeed(), 0.1, 500, "%.3f ");
		mainCamera->SetRotateSpeed(rotSpeed);

	}

	void Layer::ShowTerrian(Object* obj)
	{

		ImGui::Text("Terrian");
		bool dynamicTessLevel = obj->GetComponent<TerrainComponent>()->GetDynamicTess();
		ImGui::Checkbox("dynamicTessLevel", &dynamicTessLevel);
		obj->GetComponent<TerrainComponent>()->SetDynamicTess(dynamicTessLevel);
	

		float staticTessLevel = obj->GetComponent<TerrainComponent>()->GetStaticTessLevel();
		ImGui::DragFloat("staticTessLevel", &staticTessLevel, obj->GetComponent<TerrainComponent>()->GetStaticTessLevel(), 8.0f, 200.0f, "%.3f ");
		obj->GetComponent<TerrainComponent>()->SetTessLevel(staticTessLevel);

	}

	void Layer::ShowShader(std::string imguiShaders, int meshIndex, static  int& itemIndex, int offset)
	{
		std::string buttonName = "select file##" + std::to_string(meshIndex + offset);
		std::string inputTextName = "mesh" + std::to_string(meshIndex + offset);

		//imguiShaders = mesh->GetMaterial()->GetShader()->getDesc().path;
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

	void Layer::ShowTextures(std::string imguiShaders, int meshIndex, static  int& itemIndex, TextureType textureType, static TextureType& type, int offset)
	{

		std::string buttonName = ""; //+std::to_string(meshIndex + offset);
		std::string inputTextName = "";// +std::to_string(meshIndex + offset);

		switch (textureType) {
		case TextureType::DiffuseMap:
			buttonName += "diffuseMap";
			inputTextName += "diffuseMap";
			break;
		case TextureType::SpecularMap:
			buttonName += "specularMap";
			inputTextName += "specularMap";
			break;
		case TextureType::EmissionMap:
			buttonName += "emissionMap";
			inputTextName += "emissionMap";
			break;
		case TextureType::HeightMap:
			buttonName += "heightMap";
			inputTextName += "heightMap";
			break;
		case TextureType::NormalMap:
			buttonName += "normalMap";
			inputTextName += "normalMap";
			break;
		case TextureType::CubeMap:
			buttonName += "cubeMap";
			inputTextName += "cubeMap";
			break;
		case TextureType::DepthMap:
			buttonName += "depthMap";
			inputTextName += "depthMap";
			break;
		case TextureType::AoMap:
			buttonName += "aoMap";
			inputTextName += "aoMap";
			break;
		case TextureType::RoughnessMap:
			buttonName += "roughnessMap";
			inputTextName += "roughnessMap";
			break;
		case TextureType::MentallicMap:
			buttonName += "mentallicMap";
			inputTextName += "mentallicMap";
			break;

		}
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
		std::vector<std::shared_ptr<Mesh>>& imGuiMeshes = comp->GetMeshes();
		if (imGuiMeshes.empty()) return;

		int offset = 0;

		ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "Shader");
		std::vector<std::string> imguiShaders;
		static int itemIndex = -1;
		//std::cout << "itemIndex" << itemIndex <<std::endl;
		imguiShaders.resize(imGuiMeshes.size());
		for (int i = 0; i < imGuiMeshes.size(); i++)
		{
			if (imGuiMeshes[i]->GetMaterial()->GetShader() == nullptr)
				continue;
			imguiShaders[i] = imGuiMeshes[i]->GetMaterial()->GetShader()->GetPath();
			ShowShader(imguiShaders[i], i, itemIndex, offset);
		}
		if (itemIndex != -1) {
			if (m_fileDialog.HasSelected()) {

				imGuiMeshes[itemIndex]->GetMaterial()->SetShader(m_fileDialog.GetSelected().string());
				m_fileDialog.ClearSelected();
				itemIndex = -1;
			}
		}

		if (imGuiMeshes[0]->GetMaterial()->GetTextureMaps() != nullptr) {

			ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "TextureMaps");
			std::vector<std::string> imguiDiffuseTextures(imGuiMeshes.size());
			std::vector<std::string> imguiSpecularTextures(imGuiMeshes.size());
			std::vector<std::string> imguiEmissionTextures(imGuiMeshes.size());
			std::vector<std::string> imguiNormalTextures(imGuiMeshes.size());
			std::vector<std::string> imguiCubeTextures(imGuiMeshes.size());
			std::vector<std::string> imguiHeightTextures(imGuiMeshes.size());
			std::vector<std::string> imguiDepthTextures(imGuiMeshes.size());


			static  int itemIndexTexture = -1;
			static TextureType type;
			//std::cout << "itemIndexTexture" << itemIndexTexture << std::endl;
			//GE_CORE_TRACE("itemIndexTexture:"+std::to_string(itemIndexTexture) );
			for (int i = 0; i < imGuiMeshes.size(); i++)
			{
				std::string text = "Mesh" + std::to_string(i);
				ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, text.c_str());
				if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->diffuseTextureMap != nullptr) {
					imguiDiffuseTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->diffuseTextureMap->getDesc().path;
				}
				ShowTextures(imguiDiffuseTextures[i], i, itemIndexTexture, TextureType::DiffuseMap, type, imGuiMeshes.size() * 2);


				if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->specularTextureMap != nullptr)
					imguiSpecularTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->specularTextureMap->getDesc().path;
				ShowTextures(imguiSpecularTextures[i], i, itemIndexTexture, TextureType::SpecularMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->emissionTextureMap != nullptr)
					imguiEmissionTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->emissionTextureMap->getDesc().path;
				ShowTextures(imguiEmissionTextures[i], i, itemIndexTexture, TextureType::EmissionMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->normalTextureMap != nullptr)
					imguiNormalTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->normalTextureMap->getDesc().path;
				ShowTextures(imguiNormalTextures[i], i, itemIndexTexture, TextureType::NormalMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->cubeTextureMap != nullptr)
					imguiCubeTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->cubeTextureMap->getDesc().path;
				ShowTextures(imguiCubeTextures[i], i, itemIndexTexture, TextureType::CubeMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->heightTextureMap != nullptr)
					imguiHeightTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->heightTextureMap->getDesc().path;
				ShowTextures(imguiHeightTextures[i], i, itemIndexTexture, TextureType::HeightMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->depthTextureMap != nullptr)
					imguiDepthTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->depthTextureMap->getDesc().path;
				ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, TextureType::DepthMap, type, imGuiMeshes.size() * 2);

				if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->aoMap != nullptr)
					imguiDepthTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->aoMap->getDesc().path;
				ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, TextureType::AoMap, type, imGuiMeshes.size() * 2);


				if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->roughnessMap != nullptr)
					imguiDepthTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->roughnessMap->getDesc().path;
				ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, TextureType::RoughnessMap, type, imGuiMeshes.size() * 2);


				if (imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->mentallicMap != nullptr)
					imguiDepthTextures[i] = imGuiMeshes[i]->GetMaterial()->GetTextureMaps()->mentallicMap->getDesc().path;
				ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, TextureType::MentallicMap, type, imGuiMeshes.size() * 2);
			}
			if (itemIndexTexture != -1) {
				if (m_fileDialog.HasSelected()) {
					TextureDesc desc;
					desc.type = type;
					desc.path = m_fileDialog.GetSelected().string();
					TextureHandle texture = m_DeviceManager->GetDevice()->createTexture(desc);

					imGuiMeshes[itemIndexTexture]->GetMaterial()->SetTexture(texture);
	

					
					m_fileDialog.ClearSelected();
					itemIndexTexture = -1;
				}

			}
		}

		ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "Material Properties");
		Material::Props imGuiProps = imGuiMeshes[0]->GetMaterial()->GetProps();//TODO::默认所有mesh 的Material::Props 是一样的
		float imGuiShininess = imGuiProps.shininess;
		bool  imGUiBlinnLight = imGuiProps.isBinnLight;
		bool  imGUiIsPBRTextureSample = (bool)imGuiProps.isPBRTextureSample;
		bool  imGUiIsDifussTextureSample = (bool)imGuiProps.isDiffuseTextureSample;
		bool  imGUiIsSpecularTextureSample = (bool)imGuiProps.isSpecularTextureSample;
		//bool  imGUiIsMetallicrTextureSample = (bool)imGuiProps.isMetallicTextureSample;

		ImGui::Checkbox("blinnlight", &imGUiBlinnLight);
		for (auto mesh : imGuiMeshes)
			mesh->GetMaterial()->SetBinnLight(imGUiBlinnLight);

		ImGui::DragFloat("shininess", &imGuiShininess, 0.5f, 0.0f, 1024.0f, "%.3f ");
		for (auto mesh : imGuiMeshes)
			mesh->GetMaterial()->SetShininess(imGuiShininess);


		ImGui::Checkbox("usePPBRTexture", &imGUiIsPBRTextureSample);
		for (auto mesh : imGuiMeshes)
			mesh->GetMaterial()->SetPBRTextureSample((int)imGUiIsPBRTextureSample);

		ImGui::Checkbox("useDiffuseTexture", &imGUiIsDifussTextureSample);
		for (auto mesh : imGuiMeshes)
			mesh->GetMaterial()->SetTextureSampleDiffuse((int)imGUiIsDifussTextureSample);

		ImGui::Checkbox("useSpecularTexture", &imGUiIsSpecularTextureSample);
		for (auto mesh : imGuiMeshes)
			mesh->GetMaterial()->SetTextureSampleSpecular((int)imGUiIsSpecularTextureSample);

		/*ImGui::Checkbox("useMetallicTexture", &imGUiIsMetallicrTextureSample);
		for (auto mesh : imGuiMeshes)
			mesh->GetMaterial()->SetTextureSampleMetallic((int)imGUiIsMetallicrTextureSample);*/

		for (int i = 0; i < imGuiMeshes.size(); i++) {
			MaterialColor::Color color = imGuiMeshes[i]->GetMaterial()->GetMaterialColor().Get();
			ImGui::ColorEdit3("diffuseColor", color.diffuseColor);
			//mesh->GetMaterial()->SetMaterialColorDiffuseColor(color.diffuseColor);

			ImGui::ColorEdit3("specularColor", color.specularColor);
			//mesh->GetMaterial()->SetMaterialColorSpecularColor(color.specularColor);

			ImGui::ColorEdit3("emissionColor", color.emissionColor);
			//mesh->GetMaterial()->SetMaterialColorEmissionColor(color.emissionColor);
			imGuiMeshes[i]->SetMaterialColor(color);

		}


	}

	void Layer::ShowTransform(Transform* comp, Object* obj)
	{
		ImGui::Text("Transform");

		float pos[] = { comp->GetPosition().x,comp->GetPosition().y,comp->GetPosition().z };
		ImGui::DragFloat3("position", pos, 0.2f, -100.0f, 100.0f, "%.3f ");
		obj->SetPosition({ pos[0],pos[1],pos[2] });

		float scale[] = { comp->GetScale().x,comp->GetScale().y,comp->GetScale().z };
		ImGui::DragFloat3("scale", scale, 0.05f, 0.001f, 100.0f, "%.3f ");
		obj->SetScale({ scale[0],scale[1],scale[2] });

		float rotate[] = { comp->GetRotation().x,comp->GetRotation().y,comp->GetRotation().z };
		ImGui::DragFloat3("rotation", rotate, 1.0f, -360.0f, 360.0f, "%.3f ");
		obj->SetRotation({ rotate[0],rotate[1],rotate[2] });

	}

	void Layer::ShowLightProbe(LightProbe* probe, Object* obj)
	{
		ImGui::Text("LightProbe");

		float zFar = probe->GetZfar();
		ImGui::DragFloat("zFar", &zFar, 0.5f, 1.0, 100.0);
		probe->SetZfar(zFar);

		bool dynamicSpecularMap = probe->GetDynamicSpecularMap();
		ImGui::Checkbox("dynamicSpecularLight", &dynamicSpecularMap);
		probe->SetDynamicSpecularMap(dynamicSpecularMap);


	}

	void Layer::ShowPointLight(PointLight* pointLight)
	{
		//if (comp->GetType() == LightType::PointLight) {
			//auto pointLight = std::dynamic_pointer_cast<PointLight>(comp);
		//auto color = pointLight->GetMeshes()->GetMaterial()->GetMaterialColor().Get();
		auto props = pointLight->GetLightProps();
		static  int attenuation = (int)pointLight->GetAttenuation().maxDistance;
		float intensity = pointLight->GetLightProps().intensity;
		float area = pointLight->GetLightProps().area;
		float bias = pointLight->GetLightProps().shadowBias;

		ImGui::ColorEdit3("ambient Color", props.ambient);
		ImGui::ColorEdit3("diffuse Color", props.diffuse);
		ImGui::ColorEdit3("specular Color", props.specular);
		ImGui::ColorEdit3("emission Color", props.emission);
		ImGui::DragInt("attenuation", &attenuation, 0.5f, 7, 3250);
		ImGui::DragFloat("intensity", &intensity, 0.1f, 0.1, 100);
		ImGui::DragFloat("lightSize", &area, 0.1f, 0.1, 100);
		ImGui::DragFloat("shadowBias", &bias, 0.001f, 0.001, 100);
		ImGui::DragInt("pcfSamplesCnt", &ShadowMapPointLightRenderer::s_PCFSamplesCnt, 1, 2, 60);

		pointLight->SetAttenuation(attenuation);
		Light::Props pros = { props.ambient ,props.diffuse,props.specular,props.emission,intensity };
		pros.area = area;
		pros.shadowBias = bias;

		pointLight->UpdateMesh(pros);

	}

	void Layer::ShowParallelLight(ParallelLight* parallelLight)
	{
		auto props = parallelLight->GetLightProps();
		math::float3 direction = parallelLight->GetDirection();

		float dir[] = { direction.x,direction.y,direction.z };
		ImGui::DragFloat3("position", dir, 0.05f, 0.0f, 1.0f, "%.3f ");
		parallelLight->SetDirection({ dir[0],dir[1],dir[2] });

		float intensity = parallelLight->GetLightProps().intensity;
		ImGui::DragFloat3("direction", (props.emission));

		ImGui::ColorEdit3("ambient Color", (props.ambient));
		ImGui::ColorEdit3("diffuse Color", (props.diffuse));
		ImGui::ColorEdit3("specular Color", (props.specular));
		ImGui::ColorEdit3("emission Color", (props.emission));
		ImGui::DragFloat("intensity", &intensity, 1.0f, 1, 100);


		parallelLight->UpdateMesh({ props.ambient ,props.diffuse,props.specular,props.emission,intensity });
	}

	std::vector<Object*> Layer::GetObjects()
	{
		return g_objectManager->GetObjects();
	}
	//std::vector<std::string> Layer::GetObjectsName()
	//{
	//	return g_objectManager->GetObjectsName();
	//}
	void Layer::DestroyObjects() //TODO:删除某一个Objects的情况还没处理--》重新考虑m_EntityToObjects数据结构
	{
		g_objectManager->DestroyObjects();
	}
}