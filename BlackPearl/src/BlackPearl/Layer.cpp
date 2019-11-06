#include "pch.h"
#include"BlackPearl/Layer.h"

#include "BlackPearl/Renderer/LightComponent/PointLight.h"
#include "BlackPearl/Renderer/LightComponent/ParallelLight.h"
#include "BlackPearl/Renderer/LightComponent/SpotLight.h"
#include "Renderer/Model/Model.h"
#include "Renderer/Shader.h"
#include "imgui.h"
namespace BlackPearl {

	Object* Layer::CreateEmpty(std::string name) {
		Entity *entity = GetEntityManager()->CreateEntity();
		Object *obj = new Object(entity->GetEntityManager(), entity->GetId(), name);
		m_EntityToObjects.insert(std::make_pair(entity->GetId().index(), obj));
		std::shared_ptr<Transform> TransformComponent(obj->AddComponent<Transform>());
		return obj;
	}
	Object* Layer::CreateLight(LightType type)
	{
		Object *Obj = CreateEmpty("Light");
		Transform *TransformComponent = Obj->GetComponent<Transform>();
		//std::shared_ptr<BlackPearl::Light> lightComponent;

		switch (type)
		{
		case LightType::ParallelLight: {
			auto lightComponent = Obj->AddComponent<ParallelLight>();
			m_LightSources->AddLight(Obj);
			break;
		}
		case LightType::PointLight: {
			std::shared_ptr<PointLight> lightComponent = Obj->AddComponent<PointLight>();
			lightComponent->SetAttenuation(PointLight::Attenuation(3250));
			m_LightSources->AddLight(Obj);
			Obj->AddComponent<MeshRenderer>(lightComponent->GetMeshes(), TransformComponent->GetTransformMatrix());

			break;
		}
		case LightType::SpotLight: {
			auto lightComponent = Obj->AddComponent<SpotLight>();
			lightComponent->SetAttenuation(SpotLight::Attenuation(3250));
			m_LightSources->AddLight(Obj);
			break;
		}
		default:
			break;
		}


		return Obj;
	}

	Object* Layer::CreateModel()
	{
		std::shared_ptr<Shader>IronManShader(new Shader("assets/shaders/IronMan.glsl"));
		IronManShader->Bind();
		std::shared_ptr<Model> IronManModel(new Model("assets/models/IronMan/IronMan.obj", IronManShader));
		Object *Obj = CreateEmpty("Model");
		Transform *TransformComponent = Obj->GetComponent<Transform>();
		TransformComponent->SetPosition({ 0.0f, -1.75f, 0.0f });
		TransformComponent->SetRotation({ 0.0,180.0,0.0 });
		TransformComponent->SetScale({ 0.01f, 0.01f, 0.01f });

		Obj->AddComponent<MeshRenderer>(IronManModel, TransformComponent->GetTransformMatrix());
		return Obj;
	}

	Object * Layer::CreateCube() //TODO:
	{
		return nullptr;
	}

	void Layer::ShowMeshRenderer(std::shared_ptr<BlackPearl::MeshRenderer> comp)
	{



	}

	void Layer::ShowTransform(std::shared_ptr<BlackPearl::Transform> comp)
	{
		static float f[] = { comp->GetPosition().x,comp->GetPosition().y,comp->GetPosition().z };
		ImGui::DragFloat3("position", f, 1.0f, -100.0f, 100.0f, "%.3f ");
		comp->SetPosition({ f[0],f[1],f[2] });

	}

	void Layer::DrawObjects()
	{
		for (auto pair : m_EntityToObjects) {
			Object* obj = pair.second;
			if (obj != nullptr &&obj->HasComponent<MeshRenderer>()) {
				auto transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
				obj->GetComponent<MeshRenderer>()->UpdateTransformMatrix(transformMatrix);

				//灯光单独处理
				if (obj->HasComponent<PointLight>() || obj->HasComponent<ParallelLight>() || obj->HasComponent<SpotLight>()) {
					obj->GetComponent<MeshRenderer>()->DrawLight();
				}
				else {
					obj->GetComponent<MeshRenderer>()->DrawMeshes();
					obj->GetComponent<MeshRenderer>()->DrawModel();
				}

			}
		}
	}
	std::vector<Object*> Layer::GetObjects()
	{
		std::vector<Object*>objs;
		for (auto pair : m_EntityToObjects) {
			Object* obj = pair.second;
			if (obj != nullptr) {
				objs.push_back(obj);
			}
		}
		return objs;
	}
	std::vector<std::string> Layer::GetObjectsName()
	{
		std::vector<std::string>objs;
		for (auto pair : m_EntityToObjects) {
			Object* obj = pair.second;
			if (obj != nullptr) {
				objs.push_back(obj->ToString());
			}
		}
		return objs;
	}
	void Layer::DestroyObjects() //TODO:删除某一个Objects的情况还没处理--》重新考虑m_EntityToObjects数据结构
	{
		for (auto pair : m_EntityToObjects) {
			Object* obj = pair.second;
			if (obj != nullptr) {
				delete obj;
				obj = nullptr;
			}
		}
	}
}