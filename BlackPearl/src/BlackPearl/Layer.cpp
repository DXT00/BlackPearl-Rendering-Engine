#include "pch.h"

#include"BlackPearl/Layer.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "Renderer/Model/Model.h"
#include "Renderer/Shader.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
namespace BlackPearl {

	static int buttonNum = 0;





	Object* Layer::CreateEmpty(std::string name) {

		return m_ObjectManager->CreateEmpty(name);
	}
	Object* Layer::CreateLight(LightType type)
	{
		return m_ObjectManager->CreateLight(type, m_LightSources);
	}
	Object * Layer::CreateCube(const std::string& shaderPath, const std::string& texturePath) //TODO:
	{
		return m_ObjectManager->CreateCube(shaderPath, texturePath);
	}
	Object * Layer::CreatePlane(const std::string& shaderPath, const std::string& texturePath)
	{
		return m_ObjectManager->CreatePlane(shaderPath, texturePath);
	}
	Object * Layer::CreateSkyBox(const std::vector<std::string>& textureFaces)
	{
		return m_ObjectManager->CreateSkyBox(textureFaces);
	}
	Object * Layer::CreateQuad(const std::string& shaderPath, const std::string& texturePath)
	{
		return m_ObjectManager->CreateQuad(shaderPath, texturePath);
	}
	Object* Layer::CreateModel(const std::string& modelPath, const std::string& shaderPath)
	{
		return m_ObjectManager->CreateModel(modelPath, shaderPath);
	}
	Object* Layer::CreateCamera() {

		return m_ObjectManager->CreateCamera();
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


		//if (imguiShaders.size()==0) {
		//	//char name[] = { 0 };
		//	//ImGui::InputText(inputTextName.c_str(), name, IM_ARRAYSIZE(name));
		//	const char* const entityItems[] = { "" };
		//	 int entityIdx = -1;

		//	ImGui::Combo(inputTextName.c_str(), &entityIdx, entityItems, 1);
		//}
		//else {
		//	//const char* const entityItems[] = { imguiShaders.c_str() };
		//	// int entityIdx = -1;
		//	 ImGui::Text(imguiShaders.c_str());
		////	ImGui::Combo(inputTextName.c_str(), &entityIdx, entityItems, 1);
		///*	std::string s = "Hello World!";

		//	char cstr[] = imguiShaders.c_str();
		//	imguiShaders.copy(cstr, imguiShaders.size() + 1);
		//	cstr[imguiShaders.size()] = '\0';*/


		//	//char* shader = &(imguiShaders[0]);
		//	///*char tab2[1024];
		//	//strncpy(tab2, imguiShaders.c_str(), sizeof(tab2));
		//	//tab2[sizeof(tab2) - 1] = 0;*/
		//	////const char *array = imguiShaders.c_str();
		//	//ImGui::InputText(inputTextName.c_str(), shader, IM_ARRAYSIZE(shader));
		//
		//	/*char* shader = const_cast<char*>(imguiShaders.c_str());
		//	ImGui::InputText(inputTextName.c_str(), shader, IM_ARRAYSIZE(shader));*/
		//}
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
		
				imGuiMeshes[itemIndex].GetMaterial()->SetShader("assets/shaders/" + m_fileDialog.GetSelected().string());
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
			GE_CORE_TRACE("itemIndexTexture:"+std::to_string(itemIndexTexture) );
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
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::DiffuseMap,"assets/texture/" + m_fileDialog.GetSelected().string());
						break;
					case Texture::SpecularMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::SpecularMap,"assets/texture/" + m_fileDialog.GetSelected().string());
						break;
					case Texture::EmissionMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::EmissionMap,"assets/texture/" + m_fileDialog.GetSelected().string());
						break;
					case Texture::NormalMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::NormalMap,"assets/texture/" + m_fileDialog.GetSelected().string());
						break;
					case Texture::CubeMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::CubeMap,"assets/texture/" + m_fileDialog.GetSelected().string());
						break;
					case Texture::HeightMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::HeightMap,"assets/texture/" + m_fileDialog.GetSelected().string());
						break;
					case Texture::DepthMap:
						imGuiMeshes[itemIndexTexture].GetMaterial()->SetTexture(Texture::DepthMap, "assets/texture/" + m_fileDialog.GetSelected().string());
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
			Light::Props props = pointLight->GetLightProps();
			static  int attenuation = (int)pointLight->GetAttenuation().maxDistance;
			ImGui::ColorEdit3("ambient Color", glm::value_ptr(props.ambient));
			ImGui::ColorEdit3("diffuse Color", glm::value_ptr(props.diffuse));
			ImGui::ColorEdit3("specular Color", glm::value_ptr(props.specular));
			ImGui::ColorEdit3("emission Color", glm::value_ptr(props.emission));
			ImGui::DragInt("attenuation", &attenuation,0.5f,7,3250);

			pointLight->SetAttenuation(attenuation);
			pointLight->UpdateMesh(props);
		}
		else if (comp->GetType() == LightType::ParallelLight) {

		}
		else if (comp->GetType() == LightType::SpotLight) {


		}
	}



	void Layer::DrawObjects()
	{
		m_ObjectManager->DrawObjects();
	}
	void Layer::DrawObject(Object * obj)
	{
		m_ObjectManager->DrawObject(obj);
	}
	void Layer::DrawObjectsExcept(std::vector<Object *>objs)
	{
		m_ObjectManager->DrawObjectsExcept(objs);

	}
	void Layer::DrawObjectsExcept(Object * obj)
	{
		m_ObjectManager->DrawObjectsExcept(obj);

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