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
#include "BlackPearl/Component/LightProbeComponent/LightProbeComponent.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
#include "BlackPearl/Component/BasicInfoComponent/BasicInfo.h"
#include "BlackPearl/Component/BoundingBoxComponent/BoundingBox.h"
#include "BlackPearl/Component/BVHNodeComponent/BVHNode.h"
#include "BlackPearl/Component/TransformComponent/RayTracingTransform.h"
#include "BlackPearl/Component/TerrainComponent/TerrainComponent.h"
#include "BlackPearl/Scene/SceneBuilder.h"
#include "BlackPearl/RHI/DynamicRHI.h"

namespace BlackPearl {
	extern  DynamicRHI::Type g_RHIType;

	////////////////////////ObjectCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object* ObjectCreater::CreateEmpty(std::string name)
	{
		//Entity* entity = m_EntityManager->CreateEntity();
		//Entity::Id id= entity->GetId();
		Object* obj = new Object(name);
		obj->AddComponent<Transform>();
		return obj;
	}

	////////////////////////Object3DCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object* Object3DCreater::CreateCube(const std::string& shaderPath, const std::string& texturePath, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		auto info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Cube);
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
		
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(meshFilter->GetVertices(), meshFilter->GetIndices(), material, layout);
		obj->AddComponent<MeshRenderer>(mesh);
		
		return obj;
	}

	Object* Object3DCreater::CreatePlane(const std::string& shaderPath, const std::string& texturePath, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		auto info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Plane);
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
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(meshFilter->GetVertices(), meshFilter->GetIndices(), material, layout);
		obj->AddComponent<MeshRenderer>(mesh);
		return obj;
	}

	Object* Object3DCreater::CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string& shaderPath, const std::string& texturePath, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		auto info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Sphere);
		std::shared_ptr<SphereMeshFilter> meshFilter = obj->AddComponent<SphereMeshFilter>(radius, stackCount, sectorCount);
		Transform* transformComponent = obj->GetComponent<Transform>();
		transformComponent->SetInitScale({ radius,radius,radius });
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
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(meshFilter->GetVertices(), meshFilter->GetIndices(), material, layout);
		obj->AddComponent<MeshRenderer>(mesh);

		AABB box = BoundingBoxBuilder::Build(obj);
		obj->AddComponent<BoundingBox>(box);
		return obj;
	}

	Object* Object3DCreater::CreateModel(
		std::string modelPath, 
		std::string shaderPath, 
		const bool isAnimated, 
		const bool vertices_sorted, 
		const bool addBondingBox, 
		const std::string name,
		const bool createMeshlet,
		const bool isMeshletModel,
		MeshletOption options 
		)
	{
		std::shared_ptr<Shader> shader;
		if (shaderPath == "") {
			shader.reset(DBG_NEW Shader(shaderPath));
			GE_CORE_WARN("shaderPath is empty");
		}
		else {
			shader.reset(DBG_NEW Shader(shaderPath));
			shader->Bind();
		}
		std::shared_ptr<Model> model(DBG_NEW Model(modelPath, shader, isAnimated, vertices_sorted, createMeshlet, isMeshletModel,options));
		Object* obj = CreateEmpty(name);
		auto info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Model);
		Transform* transformComponent = obj->GetComponent<Transform>();
		transformComponent->SetInitPosition({ 0.0f, 0.0f, 0.0f });
		transformComponent->SetInitRotation({ 0.0,180.0,0.0 });
		obj->AddComponent<MeshRenderer>(model);
		/*if (addBondingBox) {
			obj->AddComponent<BoundingBox>(model->GetMeshes());
		}*/
		return obj;
	}



	Object* Object3DCreater::CreateSkyBox(const std::vector<std::string>& textureFaces, const std::string& shaderPath, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		auto info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_SkyBox);
		std::shared_ptr<SkyBoxMeshFilter> meshFilter = obj->AddComponent<SkyBoxMeshFilter>();
		Transform* transformComponent = obj->GetComponent<Transform>();
		std::shared_ptr<Material> material;
		std::shared_ptr<Material::TextureMaps> texture(DBG_NEW Material::TextureMaps());
		std::shared_ptr<Texture> cubeMapTexture(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, textureFaces, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE));
		texture->cubeTextureMap = cubeMapTexture;
		material.reset(DBG_NEW Material(shaderPath, texture, {}, { 0.2,0.2,0.0 }, {}, {}));
		material->SetRTXType(Material::RTXType::RTX_DIFFUSE);
		VertexBufferLayout layout = {
		{ElementDataType::Float3,"aPos",false,0},
		};
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(meshFilter->GetVertices(), meshFilter->GetIndices(), material, layout);
		obj->AddComponent<MeshRenderer>(mesh);
		AABB box(glm::vec3(10e-20f),glm::vec3(10e20f));
		obj->AddComponent<BoundingBox>(box);
		return obj;
	}
	/*注意：Cube的Transform会导致Camera的Transform*/
	Object* Object3DCreater::CreateLightProbe(ProbeType type, const std::string& shaderPath, const std::string& texturePath, const std::string name)
	{
		Object* obj = CreateCube(shaderPath, texturePath, name);
		if (!obj->HasComponent<BasicInfo>()) {
			obj->AddComponent<BasicInfo>();
		}
		obj->GetComponent<BasicInfo>()->SetObjectType(ObjectType::OT_LightProbe);
		obj->GetComponent<Transform>()->SetInitRotation({ 0.0f, 0.0f, 0.0f });
		obj->GetComponent<Transform>()->SetInitScale({ 0.3f,0.3f,0.3f });

		obj->GetComponent<MeshRenderer>()->SetIsShadowObjects(false);
		obj->GetComponent<MeshRenderer>()->SetIsBackGroundObjects(false);
		obj->AddComponent<LightProbe>(type);
		return obj;
	}

	Object* Object3DCreater::CreateGroup(const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<BasicInfo> info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Group);
		return obj;
	}

	Object* Object3DCreater::CreateBVHNode(const std::vector<Object*>& objs, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<BasicInfo> info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_BVH_Node);
		obj->AddComponent<BVHNode>(objs);
		AABB box = obj->GetComponent<BVHNode>()->GetRootBox();
		obj->AddComponent<BoundingBox>(box);
		return obj;
	}

	Object* Object3DCreater::CreateBVHNode(const std::vector<Vertex>& mesh_vertex, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<BasicInfo> info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_BVH_Node);
		obj->AddComponent<BVHNode>(mesh_vertex);
		AABB box = obj->GetComponent<BVHNode>()->GetRootBox();
		obj->AddComponent<BoundingBox>(box);
		return obj;
	}

	Object* Object3DCreater::CreateTriangle(const std::vector<Vertex>& points, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<BasicInfo> info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Triangle);
		obj->AddComponent<Triangle>(points);
		AABB box = obj->GetComponent<Triangle>()->GetBoundingBox();
		obj->AddComponent<BoundingBox>(box);

		return obj;
	}

	Object* Object3DCreater::CreateRTXTransformNode(const glm::mat4& transform_mat, Object* bvh_node, std::shared_ptr<Material> rtx_material,const std::string name)
	{
		Object* obj = CreateEmpty(name);

		std::shared_ptr<BasicInfo> info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_RTXTransformNode);
		obj->AddComponent<RTXTransformNode>(transform_mat, bvh_node);
		obj->AddComponent<MeshRenderer>(rtx_material);
		AABB box = obj->GetComponent<RTXTransformNode>()->GetBoundingBox();
		obj->AddComponent<BoundingBox>(box);

		return obj;
	}

	Object* Object3DCreater::CreateTerrain(const std::string& shaderPath, const std::string& heightMapPath, const std::string& texturePath, uint32_t chunkCntX, uint32_t chunkCntZ, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		auto info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Terrain);


		std::shared_ptr<Material> material;
		std::shared_ptr<Material::TextureMaps> texture(DBG_NEW Material::TextureMaps());
		if (!texturePath.empty())
			texture->diffuseTextureMap.reset(DBG_NEW Texture(Texture::Type::DiffuseMap, texturePath, GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));
		if (!heightMapPath.empty()) {
			texture->heightTextureMap.reset(DBG_NEW Texture(Texture::Type::HeightMap, heightMapPath, GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));
		}
		else {
			GE_CORE_ERROR("no heightmap found");
		}
		uint32_t width = texture->heightTextureMap->GetWidth();
		uint32_t height = texture->heightTextureMap->GetHeight();

		std::shared_ptr<TerrainComponent> terrain = obj->AddComponent<TerrainComponent>(width, height, chunkCntX, chunkCntZ);

		material.reset(DBG_NEW Material(shaderPath, texture, { 1.0,1.0,1.0 }, { 1.0,1.0,1.0 }, { 1.0,1.0,1.0 }, {}));
		VertexBufferLayout layout = {
		{ElementDataType::Float3,"aPos",false,0},
		{ElementDataType::Float2,"aTexCoords",false,1}
		};

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(terrain->GetVertices(),std::vector<uint32_t>(), material, layout, true/*tesselation*/, terrain->GetVertexPerChunk());
		obj->AddComponent<MeshRenderer>(mesh);

		return obj;

	}

	////////////////////////LightCreater//////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object* LightCreater::CreateLight(LightType type, LightSources* lightSources, const std::string name)
	{
		Object* Obj = CreateEmpty(name);
		Transform* TransformComponent = Obj->GetComponent<Transform>();
		TransformComponent->SetInitScale({ 0.2f,0.2f,0.2f });
		TransformComponent->SetInitPosition({ 0.0f,0.0f,0.0f });
		auto info = Obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Light);
		switch (type)
		{
		case LightType::ParallelLight: {
			auto lightComponent = Obj->AddComponent<ParallelLight>();
			lightSources->AddLight(Obj);
			break;
		}
		case LightType::PointLight: {
			//TODO:: 添加DirectX支持
			if (g_RHIType == DynamicRHI::Type::OpenGL) {

				std::shared_ptr<PointLight> lightComponent = Obj->AddComponent<PointLight>();
				lightComponent->SetAttenuation(PointLight::Attenuation(200));
				Obj->AddComponent<MeshRenderer>(lightComponent->GetMeshes());
			}
			lightSources->AddLight(Obj);

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
		auto info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Camera);
		return obj;
	}
	////////////////////////Object2DCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	Object* Object2DCreater::CreateQuad(const std::string& shaderPath, const std::string& texturePath, const std::string name)
	{

		Object* obj = CreateEmpty(name);
		auto info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Quad);
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
		std::shared_ptr<Mesh> mesh(DBG_NEW Mesh(meshFilter->GetVertices(), meshFilter->GetIndices(), material, layout));
		obj->AddComponent<MeshRenderer>(mesh);

		return obj;
	}
}