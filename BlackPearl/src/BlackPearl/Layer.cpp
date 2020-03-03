#include "pch.h"

#include"BlackPearl/Layer.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "Renderer/Model/Model.h"
#include "Renderer/Shader/Shader.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
namespace BlackPearl {

	static int buttonNum = 0;

	//bool isBackGroundObj=false;



	void Layer::OnImguiRender()
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Suqare Color", glm::value_ptr(m_BackgroundColor));
		ImGui::End();

		static BlackPearl::Object* currentObj = nullptr;//TODO::注意内存泄漏

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
					Layer::CreateLight(BlackPearl::LightType::ParallelLight);
					break;
				case 2:
					GE_CORE_INFO("Creating PointLight...");
					Layer::CreateLight(BlackPearl::LightType::PointLight);
					break;
				case 3:
					GE_CORE_INFO("Creating SpotLight ...");
					Layer::CreateLight(BlackPearl::LightType::SpotLight);
					break;
				case 4:
					GE_CORE_INFO("Creating IronMan ...");
					Layer::CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/IronMan.glsl",false);
					break;
				case 5:
					GE_CORE_INFO("Creating Deer ...");
					//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
					Layer::CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl",false);

					break;
				case 6:
					GE_CORE_INFO("Creating OldHouse ...");
					//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
					Layer::CreateModel("assets/models/OldHouse/Gost House/3D models/Gost House (5).obj", "assets/shaders/IronMan.glsl",false);

					break;
				case 7:
					GE_CORE_INFO("Creating Bunny ...");
					//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
					Layer::CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl",false);
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
				std::vector<BlackPearl::Object*> objsList = GetObjects();		//TODO::
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


		/*float pos[] = { m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection().x, m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection().y, m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection().z };
		ImGui::DragFloat3("m_LightPos", pos, 0.1f, -100.0f, 100.0f, "%.3f ");
		m_Sun->GetComponent<BlackPearl::ParallelLight>()->SetDirection({ pos[0],pos[1],pos[2] });*/

		/*
				ImGui::DragFloat("near_plane", &BlackPearl::ShadowMapRenderer::s_NearPlane, 0.5f, -50.0f, 100.0f, "%.3f ");
				ImGui::DragFloat("far_plane", &BlackPearl::ShadowMapRenderer::s_FarPlane, 0.5f, -50.0f, 100.0f, "%.3f ");*/

		if (currentObj != nullptr) {


			if (currentObj->HasComponent< BlackPearl::Transform>()) {
				ShowTransform(currentObj->GetComponent<BlackPearl::Transform>());

			}
			if (currentObj->HasComponent< BlackPearl::MeshRenderer>()) {
				ShowMeshRenderer(currentObj->GetComponent<BlackPearl::MeshRenderer>());
				bool isBackGroundObj = currentObj->GetComponent<BlackPearl::MeshRenderer>()->GetIsBackGroundObjects();
				ImGui::Checkbox("isBackGroundObj", &isBackGroundObj);
				//TODO:: 可以采用 bitset
				if (isBackGroundObj) {
					std::vector<Object*>::const_iterator it = std::find(m_BackGroundObjsList.begin(), m_BackGroundObjsList.end(), currentObj);
					if (it == m_BackGroundObjsList.end()) {
						m_BackGroundObjsList.push_back(currentObj);
						currentObj->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
					}

				}
				else {
					std::vector<Object*>::const_iterator it;// = m_BackGroundObjsList.begin();
					for (it = m_BackGroundObjsList.begin(); it != m_BackGroundObjsList.end(); it++) {
						if ((*it)->GetId() == currentObj->GetId()) {
							m_BackGroundObjsList.erase(it);
							currentObj->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(false);

							break;
						}
					}
				}

			}
			if (currentObj->HasComponent < BlackPearl::PointLight>()) {
				ShowPointLight(currentObj->GetComponent<BlackPearl::PointLight>());
			}
			if (currentObj->HasComponent< BlackPearl::PerspectiveCamera>()) {
				ShowCamera(currentObj->GetComponent<BlackPearl::PerspectiveCamera>());

			}



		}

		ImGui::End();



		m_fileDialog.Display();


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
		Object* obj = m_ObjectManager->CreateCube(shaderPath, texturePath,name);
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
	LightProbe* Layer::CreateLightProbe(const std::string& shaderPath, const std::string& texturePath, const std::string& name)
	{
		LightProbe* probe = m_ObjectManager->CreateLightProbe(shaderPath, texturePath, name);
		m_ObjectsList.push_back(probe->GetObj());
		m_ObjectsList.push_back(probe->GetCamera()->GetObj());

		return probe;
	}
	Object* Layer::CreateModel(const std::string& modelPath, const std::string& shaderPath,const bool isAnimated ,const std::string& name)
	{
		Object* obj = m_ObjectManager->CreateModel(modelPath, shaderPath, isAnimated,name);
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
		bool  imGUiIsTextureSample = (bool)imGuiProps.isTextureSample;

		ImGui::Checkbox("blinnlight", &imGUiBlinnLight);
		for (auto mesh : imGuiMeshes)
			mesh.GetMaterial()->SetBinnLight(imGUiBlinnLight);

		ImGui::DragFloat("shininess", &imGuiShininess, 0.5f, 0.0f, 1024.0f, "%.3f ");
		for (auto mesh : imGuiMeshes)
			mesh.GetMaterial()->SetShininess(imGuiShininess);


		ImGui::Checkbox("useTexture", &imGUiIsTextureSample);
		for (auto mesh : imGuiMeshes)
			mesh.GetMaterial()->SetTextureSample((int)imGUiIsTextureSample);


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





	void Layer::ShowTransform(Transform* comp)
	{
		ImGui::Text("Transform");

		float pos[] = { comp->GetPosition().x,comp->GetPosition().y,comp->GetPosition().z };
		ImGui::DragFloat3("position", pos, 0.05f, -100.0f, 100.0f, "%.3f ");
		comp->SetPosition({ pos[0],pos[1],pos[2] });

		float scale[] = { comp->GetScale().x,comp->GetScale().y,comp->GetScale().z };
		ImGui::DragFloat3("scale", scale, 0.05f, 0.001f, 100.0f, "%.3f ");
		comp->SetScale({ scale[0],scale[1],scale[2] });

		float rotate[] = { comp->GetRotation().x,comp->GetRotation().y,comp->GetRotation().z };
		ImGui::DragFloat3("rotation", rotate, 0.05f, -360.0f, 360.0f, "%.3f ");
		comp->SetRotation({ rotate[0],rotate[1],rotate[2] });

	}

	void Layer::ShowPointLight(PointLight* pointLight)
	{
		//if (comp->GetType() == LightType::PointLight) {
			//auto pointLight = std::dynamic_pointer_cast<PointLight>(comp);
		auto color = pointLight->GetMeshes().GetMaterial()->GetMaterialColor().Get();
		static  int attenuation = (int)pointLight->GetAttenuation().maxDistance;
		ImGui::ColorEdit3("ambient Color", glm::value_ptr(color.ambientColor));
		ImGui::ColorEdit3("diffuse Color", glm::value_ptr(color.diffuseColor));
		ImGui::ColorEdit3("specular Color", glm::value_ptr(color.specularColor));
		ImGui::ColorEdit3("emission Color", glm::value_ptr(color.emissionColor));
		ImGui::DragInt("attenuation", &attenuation, 0.5f, 7, 3250);
		ImGui::DragFloat("intensity", &color.intensity, 0.5f, 1, 100);

		pointLight->SetAttenuation(attenuation);

		pointLight->UpdateMesh({ color.ambientColor ,color.diffuseColor,color.specularColor,color.emissionColor });
		//	}
		/*	else if (comp->GetType() == LightType::ParallelLight) {

			}
			else if (comp->GetType() == LightType::SpotLight) {


			}*/
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