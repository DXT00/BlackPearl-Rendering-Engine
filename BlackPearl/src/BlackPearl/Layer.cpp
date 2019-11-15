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
	Object * Layer::CreatePlane()
	{
		return m_ObjectManager->CreatePlane();
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

	void Layer::ShowShader(static std::string &imguiShaders, static char* shader, Mesh & mesh, int meshIndex, static  int &itemIndex)
	{

		std::string buttonName = "select file##" + std::to_string(meshIndex);
		std::string inputTextName = "shader##" + std::to_string(meshIndex);

		imguiShaders = mesh.GetMaterial()->GetShader()->GetPath();
		shader = const_cast<char*>(imguiShaders.c_str());
		ImGui::InputText(inputTextName.c_str(), shader, IM_ARRAYSIZE(shader));
		ImGui::SameLine();
		if (ImGui::Button(buttonName.c_str())) {
			itemIndex = meshIndex;
			m_fileDialog.Open();
		}


	}

	void Layer::ShowMeshRenderer(std::shared_ptr<BlackPearl::MeshRenderer> comp)
	{





		ImGui::Text("MeshRenderer");


		if (!comp->GetMeshes().empty()) {
			ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "Material");
			static std::vector<std::string> imguiShaders(comp->GetMeshes().size());
			static std::vector<char*> shader(comp->GetMeshes().size());
			static  int itemIndex = -1;

			for (int i = 0; i < comp->GetMeshes().size(); i++)
			{
				ShowShader(imguiShaders[i], shader[i], comp->GetMeshes()[i], i, itemIndex);
			}
			if (itemIndex != -1) {
			

				if (m_fileDialog.HasSelected()) {
					imguiShaders[itemIndex] = m_fileDialog.GetSelected().string();
					std::cout << "Selected filename" << m_fileDialog.GetSelected().string() << std::endl;
					shader[itemIndex] = const_cast<char*>(imguiShaders[itemIndex].c_str());// (m_fileDialog.GetSelected().string().c_str());
					comp->GetMeshes()[itemIndex].GetMaterial()->SetShader("assets/shaders/" + imguiShaders[itemIndex]);

					m_fileDialog.ClearSelected();
					itemIndex = -1;
				}
				



			}

		}

		if (comp->GetModel() != nullptr) {
			ImGui::TextColored({ 1.0,0.64,0.0,1.0 }, "Model");
			static std::vector< std::string> imguiModelShaders(comp->GetModel()->GetMeshes().size());
			static std::vector<char*> Modelshader(comp->GetModel()->GetMeshes().size());
			static  int itemIndex = -1;
			

			for (int i = 0; i < comp->GetModel()->GetMeshes().size(); i++) {

				ShowShader(imguiModelShaders[i], Modelshader[i], comp->GetModel()->GetMeshes()[i], i, itemIndex);

			}
			std::cout << "itemIndex:" << itemIndex << std::endl;
			if (itemIndex != -1) {


				if (m_fileDialog.HasSelected()) {
					imguiModelShaders[itemIndex] = m_fileDialog.GetSelected().string();
					std::cout << "Selected filename" << m_fileDialog.GetSelected().string() << std::endl;
					Modelshader[itemIndex] = const_cast<char*>(imguiModelShaders[itemIndex].c_str());// (m_fileDialog.GetSelected().string().c_str());
					comp->GetModel()->GetMeshes()[itemIndex].GetMaterial()->SetShader("assets/shaders/" + imguiModelShaders[itemIndex]);

					m_fileDialog.ClearSelected();
					itemIndex = -1;
				}
				



			}

		}










	}





	void Layer::ShowTransform(std::shared_ptr<BlackPearl::Transform> comp)
	{
		//ImGui::Text("Transform");
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
			ImGui::ColorEdit3("ambient Color", glm::value_ptr(props.ambient));
			ImGui::ColorEdit3("diffuse Color", glm::value_ptr(props.diffuse));
			ImGui::ColorEdit3("specular Color", glm::value_ptr(props.specular));
			ImGui::ColorEdit3("emission Color", glm::value_ptr(props.emission));

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