#include "pch.h"
#include "ObjectCreater.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/MeshFilterComponent/CubeMeshFilter.h"
#include "BlackPearl/Component/MeshFilterComponent/SphereMeshFilter.h"
#include "BlackPearl/Component/MeshFilterComponent/PlaneMeshFilter.h"
#include "BlackPearl/Component/MeshFilterComponent/QuadMeshFilter.h"
#include "BlackPearl/Component/MeshFilterComponent/SkyBoxMeshFilter.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"

namespace BlackPearl {

	////////////////////////ObjectCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object* ObjectCreater::CreateEmpty(std::string name)
	{
		Entity* entity = m_EntityManager->CreateEntity();
		Object* obj = new Object(entity->GetEntityManager(), entity->GetId(), name);

		std::shared_ptr<Transform> TransformComponent(obj->AddComponent<Transform>());
		return obj;
	}

	////////////////////////Object3DCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object* Object3DCreater::CreateCube(const std::string& shaderPath, const std::string& texturePath, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<CubeMeshFilter> meshFilter = obj->AddComponent<CubeMeshFilter>();
		Transform* transformComponent = obj->GetComponent<Transform>();

		std::shared_ptr<Material> material;
		std::shared_ptr<Material::TextureMaps> texture(DBG_NEW Material::TextureMaps());
		if (texturePath != "")
			texture->diffuseTextureMap.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, texturePath, GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));

		material.reset(DBG_NEW Material(shaderPath, texture, { 1.0,1.0,1.0 }, { 1.0,1.0,1.0 }, { 1.0,1.0,1.0 }, {}));
		VertexBufferLayout layout = {
		{ElementDataType::Float3,"aPos",false,0},
		{ElementDataType::Float3,"aNormal",false,1},
		{ElementDataType::Float2,"aTexCoords",false,2}
		};
		Mesh mesh(meshFilter->GetVertices(), meshFilter->GetIndices(), material, layout);
		obj->AddComponent<MeshRenderer>(mesh, transformComponent->GetTransformMatrix());

		return obj;
	}
	Object* Object3DCreater::CreatePlane(const std::string& shaderPath, const std::string& texturePath, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<PlaneMeshFilter> meshFilter = obj->AddComponent<PlaneMeshFilter>();
		Transform* transformComponent = obj->GetComponent<Transform>();

		std::shared_ptr<Material> material;
		std::shared_ptr<Material::TextureMaps> texture(DBG_NEW Material::TextureMaps());
		if (texturePath != "")
			texture->diffuseTextureMap.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, texturePath, GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));

		material.reset(DBG_NEW Material(shaderPath, texture, {}, { 0.2,0.2,0.0 }, {}, {}));
		VertexBufferLayout layout = {
		{ElementDataType::Float3,"aPos",false,0},
		{ElementDataType::Float3,"aNormal",false,1},
		{ElementDataType::Float2,"aTexCoords",false,2}
		};
		Mesh mesh(meshFilter->GetVertices(), meshFilter->GetIndices(), material, layout);
		obj->AddComponent<MeshRenderer>(mesh, transformComponent->GetTransformMatrix());
		return obj;
	}
	Object* Object3DCreater::CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string& shaderPath, const std::string& texturePath, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<SphereMeshFilter> meshFilter = obj->AddComponent<SphereMeshFilter>(radius, stackCount, sectorCount);
		Transform* transformComponent = obj->GetComponent<Transform>();

		std::shared_ptr<Material> material;
		std::shared_ptr<Material::TextureMaps> texture(DBG_NEW Material::TextureMaps());
		if (texturePath != "")
			texture->diffuseTextureMap.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, texturePath, GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));

		material.reset(DBG_NEW Material(shaderPath, texture, { 1.0,1.0,1.0 }, { 1.0,1.0,1.0 }, { 1.0,1.0,1.0 }, {}));
		VertexBufferLayout layout = {
		{ElementDataType::Float3,"aPos",false,0},
		{ElementDataType::Float3,"aNormal",false,1},
		{ElementDataType::Float2,"aTexCoords",false,2}
		};
		Mesh mesh(meshFilter->GetVertices(), meshFilter->GetIndices(), material, layout);
		obj->AddComponent<MeshRenderer>(mesh, transformComponent->GetTransformMatrix());

		return obj;
	}

	Object* Object3DCreater::CreateModel(std::string modelPath, std::string shaderPath,const bool isAnimated, const std::string name)
	{
		std::shared_ptr<Shader>shader(new Shader(shaderPath));
		shader->Bind();
		std::shared_ptr<Model> model(new Model(modelPath, shader, isAnimated));
		Object* obj = CreateEmpty(name);

		Transform* transformComponent = obj->GetComponent<Transform>();
		transformComponent->SetPosition({ 0.0f, 0.0f, 0.0f });
		transformComponent->SetRotation({ 0.0,180.0,0.0 });
			//transformComponent->SetScale({ 0.0001f, 0.0001f, 0.0001f });

		obj->AddComponent<MeshRenderer>(model, transformComponent->GetTransformMatrix());
		//std::shared_ptr<BlackPearl::Texture> cubeMapTexture(DBG_NEW BlackPearl::CubeMapTexture(Texture::Type::CubeMap,
		//	{ "assets/skybox/skybox/right.jpg",
		//	 "assets/skybox/skybox/left.jpg",
		//	 "assets/skybox/skybox/top.jpg",
		//	 "assets/skybox/skybox/bottom.jpg",
		//	 "assets/skybox/skybox/front.jpg",
		//	 "assets/skybox/skybox/back.jpg",
		//	}));
		//obj->GetComponent<MeshRenderer>()->SetModelTextures(cubeMapTexture);
		return obj;
	}

	Object* Object3DCreater::CreateSkyBox( const std::vector<std::string>& textureFaces,const std::string& shaderPath, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<SkyBoxMeshFilter> meshFilter = obj->AddComponent<SkyBoxMeshFilter>();
		Transform* transformComponent = obj->GetComponent<Transform>();
		std::shared_ptr<Material> material;
		std::shared_ptr<Material::TextureMaps> texture(DBG_NEW Material::TextureMaps());
		std::shared_ptr<Texture> cubeMapTexture(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, textureFaces, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE));
		texture->cubeTextureMap = cubeMapTexture;
		material.reset(DBG_NEW Material(shaderPath, texture, {}, { 0.2,0.2,0.0 }, {}, {}));

		VertexBufferLayout layout = {
		{ElementDataType::Float3,"aPos",false,0},
		};
		Mesh mesh(meshFilter->GetVertices(), meshFilter->GetIndices(), material, layout);
		obj->AddComponent<MeshRenderer>(mesh, transformComponent->GetTransformMatrix());
		return obj;
	}
	/*注意：Cube的Transform会导致Camera的Transform*/
	Object* Object3DCreater::CreateLightProbe(const std::string& shaderPath, const std::string& texturePath, const std::string name)
	{
		Object* obj = CreateCube(shaderPath, texturePath, name);
		

		return obj;		
	}




	////////////////////////LightCreater//////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object* LightCreater::CreateLight(LightType type, LightSources* lightSources, const std::string name)
	{
		Object* Obj = CreateEmpty(name);
		Transform* TransformComponent = Obj->GetComponent<Transform>();
		TransformComponent->SetScale({ 0.2f,0.2f,0.2f });
		TransformComponent->SetPosition({ 0.0f,0.0f,0.0f });
		switch (type)
		{
		case LightType::ParallelLight: {
			auto lightComponent = Obj->AddComponent<ParallelLight>();
			lightSources->AddLight(Obj);
			break;
		}
		case LightType::PointLight: {
			std::shared_ptr<PointLight> lightComponent = Obj->AddComponent<PointLight>();
			lightComponent->SetAttenuation(PointLight::Attenuation(200));
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
	Object* Object3DCreater::CreateCamera(const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<PerspectiveCamera> cameraComponent = obj->AddComponent<PerspectiveCamera>();

		return obj;
	}
	////////////////////////Object2DCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object* Object2DCreater::CreateQuad(const std::string& shaderPath, const std::string& texturePath, const std::string name)
	{

		Object* obj = CreateEmpty(name);

		std::shared_ptr<QuadMeshFilter> meshFilter = obj->AddComponent<QuadMeshFilter>();
		Transform* transformComponent = obj->GetComponent<Transform>();
		std::shared_ptr<Material> material;

		std::shared_ptr<Material::TextureMaps> texture(DBG_NEW Material::TextureMaps());
		if (texturePath != "") {
			texture->diffuseTextureMap.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, texturePath, GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));

		}
		material.reset(DBG_NEW Material(shaderPath, texture, {}, { 0.2,0.5,0.6 }, {}, {}));

		VertexBufferLayout layout = {
		{ElementDataType::Float3,"aPos",false,0},
		{ElementDataType::Float3,"aNormal",false,1},
		{ElementDataType::Float2,"aTexCoords",false,2}

		};
		Mesh mesh(meshFilter->GetVertices(), meshFilter->GetIndices(), material, layout);
		obj->AddComponent<MeshRenderer>(mesh, transformComponent->GetTransformMatrix());

		return obj;
	}
}