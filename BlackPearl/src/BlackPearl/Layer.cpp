#include "pch.h"
#include"BlackPearl/Layer.h"
#include "Renderer/TransformComponent/Transform.h"
#include "Renderer/RendererComponent/MeshRenderer.h"
#include "BlackPearl/Renderer/LightComponent/PointLight.h"
#include "BlackPearl/Renderer/LightComponent/ParallelLight.h"
#include "BlackPearl/Renderer/LightComponent/SpotLight.h"
#include "Renderer/Model/Model.h"
#include "Renderer/Shader.h"
namespace BlackPearl {

	Object* Layer::CreateEmpty() {
		Entity *entity = GetEntityManager()->CreateEntity();
		Object *obj = new Object(entity->GetEntityManager(), entity->GetId());
		m_EntityToObjects.insert(std::make_pair(entity->GetId().index(), obj));

		std::shared_ptr<Transform> TransformComponent(obj->AddComponent<Transform>());
		return obj;
	}
	Object* Layer::CreateLight(LightType type)
	{
		Object *Obj = CreateEmpty();
		std::shared_ptr<BlackPearl::Light> lightComponent;
		switch (type)
		{
		case LightType::ParallelLight:
			lightComponent = Obj->AddComponent<ParallelLight>();
			m_LightSources->AddLight(std::move(lightComponent));
			break;
		case LightType::PointLight:
			lightComponent = Obj->AddComponent<PointLight>();
			std::dynamic_pointer_cast<PointLight>(lightComponent)->SetAttenuation(PointLight::Attenuation(3250));
			m_LightSources->AddLight(std::move(lightComponent));
			break;
		case LightType::SpotLight:
			lightComponent = Obj->AddComponent<SpotLight>();
			std::dynamic_pointer_cast<SpotLight>(lightComponent)->SetAttenuation(SpotLight::Attenuation(3250));
			m_LightSources->AddLight(std::move(lightComponent));

			break;
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

		Object *Obj = CreateEmpty();

		//std::shared_ptr<Transform> TransformComponent(Obj->AddComponent<Transform>());
		Transform *TransformComponent = Obj->GetComponent<Transform>();
		TransformComponent->SetPosition({ 0.0f, -1.75f, 0.0f });
		TransformComponent->SetRotation({ 0.0,180.0,0.0 });
		TransformComponent->SetScale({ 0.01f, 0.01f, 0.01f });

		std::shared_ptr<MeshRenderer> meshRendererComponent(Obj->AddComponent<MeshRenderer>(IronManModel, TransformComponent->GetTransformMatrix()));

		return Obj;

	}

	Object * Layer::CreateCube() //TODO:
	{
		return nullptr;
	}

	void Layer::DrawObjects()
	{
		for (auto pair : m_EntityToObjects) {
			Object* obj = pair.second;
			if (obj != nullptr &&obj->HasComponent<MeshRenderer>()) {
				obj->GetComponent<MeshRenderer>()->DrawMeshes();
				obj->GetComponent<MeshRenderer>()->DrawModel();
			}
		}
	}
	void Layer::DestroyObjects()
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