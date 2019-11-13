#include "pch.h"
#include"BlackPearl/Layer.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "Renderer/Model/Model.h"
#include "Renderer/Shader.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

namespace BlackPearl {

	Object* Layer::CreateEmpty(std::string name) {

		return m_ObjectManager->CreateEmpty(name);
	}
	Object* Layer::CreateLight(LightType type)
	{
		return m_ObjectManager->CreateLight(type, m_LightSources);
	}
	Object * Layer::CreateCube() //TODO:
	{
		return m_ObjectManager->CreateCube();
	}
	Object * Layer::CreatePlane()
	{
		return m_ObjectManager->CreatePlane();
	}
	Object * Layer::CreateQuad()
	{
		return m_ObjectManager->CreateQuad();
	}
	Object* Layer::CreateModel(const std::string& modelPath, const std::string& shaderPath)
	{	
		return m_ObjectManager->CreateModel(modelPath,shaderPath);
	}
	Object* Layer::CreateCamera() {

		return m_ObjectManager->CreateCamera();
	}
	

	void Layer::ShowMeshRenderer(std::shared_ptr<BlackPearl::MeshRenderer> comp)
	{



	}

	void Layer::ShowTransform(std::shared_ptr<BlackPearl::Transform> comp)
	{
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