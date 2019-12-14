#include "pch.h"

#include"BlackPearl/Layer.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "Renderer/Model/Model.h"
#include "Renderer/Shader/Shader.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
namespace BlackPearl {

	static int buttonNum = 0;





	void Layer::OnImguiRender()
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Suqare Color", glm::value_ptr(m_BackgroundColor));
		ImGui::End();

		static BlackPearl::Object* currentObj = nullptr;//TODO::注意内存泄漏

		if (ImGui::CollapsingHeader("Create")) {

			const char* const entityItems[] = { "Empty","ParallelLight","PointLight","SpotLight","IronMan","Deer","OldHouse","Tree","Cube","Plane" };
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
					Layer::CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/IronMan.glsl");
					break;
				case 5:
					GE_CORE_INFO("Creating Deer ...");
					//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
					Layer::CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl");

					break;
				case 6:
					GE_CORE_INFO("Creating OldHouse ...");
					//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
					Layer::CreateModel("assets/models/OldHouse/Gost House/3D models/Gost House (5).obj", "assets/shaders/IronMan.glsl");

					break;
				case 7:
					GE_CORE_INFO("Creating Tree ...");
					//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
					Layer::CreateModel("assets/models/Tree/tree.obj", "assets/shaders/IronMan.glsl");
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

			std::unordered_map<BlackPearl::BaseComponent::Family, std::shared_ptr<BlackPearl::BaseComponent>> componentList = currentObj->GetComponentList();

			for (auto pair : componentList) {
				auto component = pair.second;
				if (component != nullptr) {

					switch (component->GetType()) {
					case BlackPearl::BaseComponent::Type::MeshRenderer: {
						std::shared_ptr<BlackPearl::MeshRenderer> comp = std::dynamic_pointer_cast<BlackPearl::MeshRenderer>(component);
						ShowMeshRenderer(comp);
						break;
					}
					case BlackPearl::BaseComponent::Type::Transform: {
						std::shared_ptr<BlackPearl::Transform> comp = std::dynamic_pointer_cast<BlackPearl::Transform>(component);
						ShowTransform(comp);
						break;
					}
					case BlackPearl::BaseComponent::Type::Light: {

						std::shared_ptr<BlackPearl::Light> comp = std::dynamic_pointer_cast<BlackPearl::Light>(component);
						ShowLight(comp);
						break;
					}
					default:
						break;
					}

				}

			}
		}

		ImGui::End();



		m_fileDialog.Display();


	}

	Object* Layer::CreateEmpty(std::string name) {

		Object* obj= m_ObjectManager->CreateEmpty(name);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateLight(LightType type)
	{
		Object* obj= m_ObjectManager->CreateLight(type, m_LightSources);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object * Layer::CreateCube(const std::string& shaderPath, const std::string& texturePath) //TODO:
	{
		Object* obj= m_ObjectManager->CreateCube(shaderPath, texturePath);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object * Layer::CreatePlane(const std::string& shaderPath, const std::string& texturePath)
	{
		Object* obj= m_ObjectManager->CreatePlane(shaderPath, texturePath);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object * Layer::CreateSkyBox(const std::vector<std::string>& textureFaces)
	{
		Object* obj= m_ObjectManager->CreateSkyBox(textureFaces);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object * Layer::CreateQuad(const std::string& shaderPath, const std::string& texturePath)
	{
		Object* obj= m_ObjectManager->CreateQuad(shaderPath, texturePath);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateModel(const std::string& modelPath, const std::string& shaderPath)
	{
		Object* obj= m_ObjectManager->CreateModel(modelPath, shaderPath);
		m_ObjectsList.push_back(obj);
		return obj;
	}
	Object* Layer::CreateCamera() {

		Object* obj= m_ObjectManager->CreateCamera();
		m_ObjectsList.push_back(obj);
		return obj;
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
	void Layer::ShowShader(std::string imguiShaders,  int meshIndex, static  int &itemIndex,int offset)
	{

		std::string buttonName = "select file##" + std::to_string(meshIndex+offset);
		std::string inputTextName = "mesh" + std::to_string(meshIndex+ offset);

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


	void Layer::ShowTextures( std::string imguiShaders,  int meshIndex, static  int &itemIndex,  Texture::Type textureType,static Texture::Type &type, int offset)
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
		}
			
	
		//imguiShaders = mesh.GetMaterial()->GetShader()->GetPath();
		//ImGui::PushID(meshIndex);
		

		inputTextName += "##"+std::to_string(meshIndex);
		buttonName += "##" + std::to_string(meshIndex);

		if(imguiShaders.size()==0)
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




	void Layer::ShowMaterialProps(Material::Props & imGuiProps)
	{
	}

	void Layer::ShowMeshRenderer(std::shared_ptr<BlackPearl::MeshRenderer> comp)
	{
		

		ImGui::Text("MeshRenderer");
		std::vector<Mesh> &imGuiMeshes = comp->GetMeshes();
		if (imGuiMeshes.empty()) return;

		int offset = 0;;

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
		
				imGuiMeshes[itemIndex].GetMaterial()->SetShader( m_fileDialog.GetSelected().string());
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
				ImGui::TextColored({ 1.0,0.64,0.0,1.0 },text.c_str() );
				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->diffuseTextureMap != nullptr) {
					imguiDiffuseTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->diffuseTextureMap->GetPath();
				}
				ShowTextures(imguiDiffuseTextures[i],  i, itemIndexTexture, Texture::DiffuseMap, type, imGuiMeshes.size()*2);

				
				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->specularTextureMap != nullptr) 
					imguiSpecularTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->specularTextureMap->GetPath();
				ShowTextures(imguiSpecularTextures[i], i, itemIndexTexture, Texture::SpecularMap, type, imGuiMeshes.size()*2);

				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->emissionTextureMap != nullptr) 
					imguiEmissionTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->emissionTextureMap->GetPath();
				ShowTextures(imguiEmissionTextures[i],i, itemIndexTexture, Texture::EmissionMap, type, imGuiMeshes.size()*2);

				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->normalTextureMap != nullptr)
					imguiNormalTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->normalTextureMap->GetPath();
				ShowTextures(imguiNormalTextures[i],  i, itemIndexTexture, Texture::NormalMap, type, imGuiMeshes.size()*2);

				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->cubeTextureMap != nullptr)
					imguiCubeTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->cubeTextureMap->GetPath();
				ShowTextures(imguiCubeTextures[i], i, itemIndexTexture, Texture::CubeMap, type, imGuiMeshes.size()*2);

				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->heightTextureMap != nullptr)
					imguiHeightTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->heightTextureMap->GetPath();
				ShowTextures(imguiHeightTextures[i], i, itemIndexTexture, Texture::HeightMap, type, imGuiMeshes.size()*2);

				if (imGuiMeshes[i].GetMaterial()->GetTextureMaps()->depthTextureMap != nullptr)
					imguiDepthTextures[i] = imGuiMeshes[i].GetMaterial()->GetTextureMaps()->depthTextureMap->GetPath();
				ShowTextures(imguiDepthTextures[i], i, itemIndexTexture, Texture::DepthMap, type, imGuiMeshes.size() * 2);
			}
			if (itemIndexTexture != -1) {
				if (m_fileDialog.HasSelected()) {

					switch (type) {
					case Texture::DiffuseMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::DiffuseMap,m_fileDialog.GetSelected().string());//"assets/texture/" + 
						break;
					case Texture::SpecularMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::SpecularMap, m_fileDialog.GetSelected().string());//"assets/texture/" +
						break;
					case Texture::EmissionMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::EmissionMap, m_fileDialog.GetSelected().string());
						break;
					case Texture::NormalMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::NormalMap,m_fileDialog.GetSelected().string());
						break;
					case Texture::CubeMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::CubeMap, m_fileDialog.GetSelected().string());
						break;
					case Texture::HeightMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::HeightMap, m_fileDialog.GetSelected().string());
						break;
					case Texture::DepthMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::DepthMap,  m_fileDialog.GetSelected().string());
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
		bool  imGUiIsTextureSample =(bool)imGuiProps.isTextureSample;

		ImGui::Checkbox("blinnlight", &imGUiBlinnLight);
		for(auto mesh:imGuiMeshes)
			mesh.GetMaterial()->SetBinnLight(imGUiBlinnLight);

		ImGui::DragFloat("shininess", &imGuiShininess, 0.5f, 0.0f, 1024.0f, "%.3f ");
		for (auto mesh : imGuiMeshes)
			mesh.GetMaterial()->SetShininess(imGuiShininess);


		ImGui::Checkbox("useTexture", &imGUiIsTextureSample);
		for (auto mesh : imGuiMeshes)
			mesh.GetMaterial()->SetTextureSample((int)imGUiIsTextureSample);





	}





	void Layer::ShowTransform(std::shared_ptr<BlackPearl::Transform> comp)
	{
		ImGui::Text("Transform");

		float pos[] = { comp->GetPosition().x,comp->GetPosition().y,comp->GetPosition().z };
		ImGui::DragFloat3("position", pos, 0.1f, -100.0f, 100.0f, "%.3f ");
		comp->SetPosition({ pos[0],pos[1],pos[2] });

		float scale[] = { comp->GetScale().x,comp->GetScale().y,comp->GetScale().z };
		ImGui::DragFloat3("scale", scale, 0.5f, 0.001f, 100.0f, "%.3f ");
		comp->SetScale({ scale[0],scale[1],scale[2] });

		float rotate[] = { comp->GetRotation().x,comp->GetRotation().y,comp->GetRotation().z };
		ImGui::DragFloat3("rotation", rotate, 0.5f, -360.0f, 360.0f, "%.3f ");
		comp->SetRotation({ rotate[0],rotate[1],rotate[2] });

	}

	void Layer::ShowLight(std::shared_ptr<BlackPearl::Light> comp)
	{
		if (comp->GetType() == LightType::PointLight) {
			auto pointLight = std::dynamic_pointer_cast<PointLight>(comp);
			auto &color = pointLight->GetMeshes().GetMaterial()->GetMaterialColor().Get();
			static  int attenuation = (int)pointLight->GetAttenuation().maxDistance;
			ImGui::ColorEdit3("ambient Color", glm::value_ptr(color.ambientColor));
			ImGui::ColorEdit3("diffuse Color", glm::value_ptr(color.diffuseColor));
			ImGui::ColorEdit3("specular Color", glm::value_ptr(color.specularColor));
			ImGui::ColorEdit3("emission Color", glm::value_ptr(color.emissionColor));
			ImGui::DragInt("attenuation", &attenuation,0.5f,7,3250);

			pointLight->SetAttenuation(attenuation);
			pointLight->UpdateMesh({ color.ambientColor ,color.diffuseColor,color.specularColor,color.emissionColor });
		}
		else if (comp->GetType() == LightType::ParallelLight) {

		}
		else if (comp->GetType() == LightType::SpotLight) {


		}
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