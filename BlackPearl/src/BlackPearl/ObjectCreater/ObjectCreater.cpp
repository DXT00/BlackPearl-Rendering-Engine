#include "pch.h"
#include "ObjectCreater.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/MeshFilterComponent/CubeMeshFilter.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"

namespace BlackPearl {
	
	////////////////////////ObjectCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object * ObjectCreater::CreateEmpty(std::string name)
	{
		Entity *entity = m_EntityManager->CreateEntity();
		Object *obj = new Object(entity->GetEntityManager(), entity->GetId(), name);

		std::shared_ptr<Transform> TransformComponent(obj->AddComponent<Transform>());
		return obj;
	}

	////////////////////////Object3DCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object * Object3DCreater::CreateCube()
	{
		Object* obj = CreateEmpty("Cube");
		std::shared_ptr<CubeMeshFilter> meshFilter = obj->AddComponent<CubeMeshFilter>();
		Transform *transformComponent = obj->GetComponent<Transform>();
		std::shared_ptr<Material> lightMaterial;
		lightMaterial.reset(DBG_NEW Material("assets/shaders/Cube.glsl", std::vector<std::shared_ptr<Texture>>(), {}, {0.2,0.5,0.6}, {}, {}));
		VertexBufferLayout layout = {
		{ElementDataType::Float3,"aPos",false}
		};
		Mesh mesh(meshFilter->GetVertices(), meshFilter->GetIndices(), lightMaterial, layout);
		obj->AddComponent<MeshRenderer>(mesh, transformComponent->GetTransformMatrix());

		return obj;
	}
	Object * Object3DCreater::CreateSphere()
	{
		return nullptr;
	}

	Object * Object3DCreater::CreateModel(std::string modelPath, std::string shaderPath)
	{
		std::shared_ptr<Shader>shader(new Shader(shaderPath));
		shader->Bind();
		std::shared_ptr<Model> model(new Model(modelPath, shader));
		Object *obj = CreateEmpty("Model");

		Transform *transformComponent = obj->GetComponent<Transform>();
		transformComponent->SetPosition({ 0.0f, -1.75f, 0.0f });
		transformComponent->SetRotation({ 0.0,180.0,0.0 });
		transformComponent->SetScale({ 0.01f, 0.01f, 0.01f });

		obj->AddComponent<MeshRenderer>(model, transformComponent->GetTransformMatrix());
		return obj;
	}

	////////////////////////LightCreater//////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object * LightCreater::CreateLight(LightType type,LightSources* lightSources)
	{
		Object *Obj = CreateEmpty("Light");
		Transform *TransformComponent = Obj->GetComponent<Transform>();
		TransformComponent->SetScale({ 0.2f,0.2f,0.2f });
		switch (type)
		{
		case LightType::ParallelLight: {
			auto lightComponent = Obj->AddComponent<ParallelLight>();
			lightSources->AddLight(Obj);
			break;
		}
		case LightType::PointLight: {
			std::shared_ptr<PointLight> lightComponent = Obj->AddComponent<PointLight>();
			lightComponent->SetAttenuation(PointLight::Attenuation(3250));
			lightSources->AddLight(Obj);
			Obj->AddComponent<MeshRenderer>(lightComponent->GetMeshes(), TransformComponent->GetTransformMatrix());

			break;
		}
		case LightType::SpotLight: {
			auto lightComponent = Obj->AddComponent<SpotLight>();
			lightComponent->SetAttenuation(SpotLight::Attenuation(3250));
			lightSources->AddLight(Obj);
			break;
		}
		default:
			break;
		}


		return Obj;
	}
	////////////////////////CameraCreater//////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object * CameraCreater::CreateCamera()
	{
		Object *obj = CreateEmpty("Camera");
		std::shared_ptr<PerspectiveCamera> cameraComponent= obj->AddComponent<PerspectiveCamera>();

		return obj;
	}
}