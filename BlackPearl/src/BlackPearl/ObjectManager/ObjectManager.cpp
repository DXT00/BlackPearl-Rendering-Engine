#include "pch.h"
#include "ObjectManager.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"
#include "BlackPearl/Component/LightProbeComponent/LightProbeComponent.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
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
#include "BlackPearl/Math/Math.h"
#include "BlackPearl/Renderer/Model/ModelLoader.h"
namespace BlackPearl {

	extern ModelLoader* g_modelLoader;
	Object* ObjectManager::CreateEmpty(std::string name)
	{
		Object* obj = new Object(name);
		obj->AddComponent<Transform>();
		return obj;
	}



	Object* ObjectManager::CreateLight(LightType type, LightSources* lightSources, const std::string& name)
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
			if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL) {

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
		m_Objs.push_back(Obj);
		return Obj;

	}

	Object* ObjectManager::CreateModel(
		const std::string& modelPath, 
		const std::string& shaderPath, 
		const bool isAnimated, 
		const bool vertices_sorted, 
		const bool addBondingBox, 
		const std::string& name,
		const bool createMeshlet,
		const bool isMeshletModel,
		MeshletOption options)
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
		ModelDesc desc;
		desc.bIsAnimated = isAnimated;
		desc.bSortVerticces = vertices_sorted;
		desc.bCreateMeshlet = createMeshlet;
		desc.options = options;
		desc.shader = shader;

		Model* pModel = g_modelLoader->LoadModel(modelPath, desc);
		std::shared_ptr<Model> model(pModel);// = std::shared_ptr<Model>(pModel);
		Object* obj = CreateEmpty(name);
		std::shared_ptr<BasicInfo> info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Model);
		Transform* transformComponent = obj->GetComponent<Transform>();
		transformComponent->SetInitPosition({ 0.0f, 0.0f, 0.0f });
		transformComponent->SetInitRotation({ 0.0,180.0,0.0 });
		obj->AddComponent<MeshRenderer>(model);
		if (model->GetAABB() && model->GetAABB()->IsValid()) {
			obj->AddComponent<BoundingBox>(*(model->GetAABB()));
		}
		m_Objs.push_back(obj);
		return obj;

	}

	Object* ObjectManager::CreateCube(const std::string& shaderPath, const std::string& texturePath, const std::string& name)
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
		AABB box = BoundingBoxBuilder::Build(obj);
		obj->AddComponent<BoundingBox>(box);
		m_Objs.push_back(obj);
		return obj;

	}

	Object* ObjectManager::CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string& shaderPath, const std::string& texturePath, const std::string& name)
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
		m_Objs.push_back(obj);
		return obj;
	}

	Object* ObjectManager::CreatePlane(const std::string& shaderPath, const std::string& texturePath, const std::string& name)
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
		m_Objs.push_back(obj);
		return obj;
	}

	Object* ObjectManager::CreateSkyBox(const std::vector<std::string>& textureFaces, const std::string& shaderPath, const std::string& name)
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
		AABB box(math::float3(10e-20f), math::float3(10e20f), true);
		obj->AddComponent<BoundingBox>(box);		
		m_Objs.push_back(obj);
		return obj;
	}
	///////////////////////Blending Object///////////////////////////////

	Object* ObjectManager::CreateLightProbe(ProbeType type, const std::string& shaderPath, const std::string& texturePath, const std::string& name)
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
		m_Objs.push_back(obj);
		return obj;
	}

	MainCamera* ObjectManager::CreateCamera(const std::string& name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<PerspectiveCamera> cameraComponent = obj->AddComponent<PerspectiveCamera>();
		auto info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Camera);

		MainCamera* mainCamera = DBG_NEW MainCamera(obj);
		m_Objs.push_back(obj);

		return mainCamera;

	}

	Object* ObjectManager::CreateGroup(const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<BasicInfo> info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Group);		m_Objs.push_back(obj);
		return obj;
	}
	Object* ObjectManager::CreateBVHNode(const std::vector<Object*>& objs, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<BasicInfo> info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_BVH_Node);
		obj->AddComponent<BVHNode>(objs);
		AABB box = obj->GetComponent<BVHNode>()->GetRootBox();
		obj->AddComponent<BoundingBox>(box);
		m_Objs.push_back(obj);
		return obj;
	}
	Object* ObjectManager::CreateBVHNode(const std::vector<Vertex>& mesh_vertex, const std::string name) {
		Object* obj = CreateEmpty(name);
		std::shared_ptr<BasicInfo> info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_BVH_Node);
		obj->AddComponent<BVHNode>(mesh_vertex);
		AABB box = obj->GetComponent<BVHNode>()->GetRootBox();
		obj->AddComponent<BoundingBox>(box);
		m_Objs.push_back(obj);
		return obj;

	}

	Object* ObjectManager::CreateTriangle(const std::vector<Vertex>& points, const std::string name)
	{
		Object* obj = CreateEmpty(name);
		std::shared_ptr<BasicInfo> info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_Triangle);
		obj->AddComponent<Triangle>(points);
		AABB box = obj->GetComponent<Triangle>()->GetBoundingBox();
		obj->AddComponent<BoundingBox>(box);		m_Objs.push_back(obj);
		return obj;
	}

	Object* ObjectManager::CreateRTXTransformNode(const glm::mat4 &transform_mat, Object* bvh_obj, std::shared_ptr<Material> rtx_material, const std::string name)
	{
		Object* obj = CreateEmpty(name);

		std::shared_ptr<BasicInfo> info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_RTXTransformNode);
		obj->AddComponent<RTXTransformNode>(transform_mat, bvh_obj);
		obj->AddComponent<MeshRenderer>(rtx_material);
		AABB box = obj->GetComponent<RTXTransformNode>()->GetBoundingBox();
		obj->AddComponent<BoundingBox>(box);
		m_Objs.push_back(obj);
		return obj;
	}



	///////////////////////2D///////////////////////////////
	Object* ObjectManager::CreateQuad(const std::string& shaderPath, const std::string& texturePath, const std::string& name)
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
		m_Objs.push_back(obj);
		return obj;
	}

	Object* ObjectManager::CreateTerrain(const std::string& shaderPath, const std::string& heightMapPath, const std::string& texturePath, uint32_t chunkCntX, uint32_t chunkCntZ, const std::string name)
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

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(terrain->GetVertices(), std::vector<uint32_t>(), material, layout, true/*tesselation*/, terrain->GetVertexPerChunk());
		obj->AddComponent<MeshRenderer>(mesh);
		m_Objs.push_back(obj);
		return obj;
	}

	BatchNode* ObjectManager::CreateBatchNode(std::vector<Object*> objs, bool dynamic, const std::string& name)
	{
		Object* obj = CreateEmpty(name);
		auto info = obj->AddComponent<BasicInfo>();
		info->SetObjectType(ObjectType::OT_BatchNode);

		BatchNode* batchNode = DBG_NEW BatchNode(obj, objs, dynamic);
		m_Objs.push_back(obj);


		return batchNode;
	}




	std::vector<Object*> ObjectManager::GetObjects()
	{
		return m_Objs;
	}
	//std::vector<std::string> ObjectManager::GetObjectsName()
	//{
	//	std::vector<std::string>objs;
	//	for (auto pair : m_EntityToObjects) {
	//		Object* obj = pair.second;
	//		if (obj != nullptr) {
	//			objs.push_back(obj->GetName());
	//		}
	//	}
	//	return objs;
	//}
	/*void ObjectManager::DrawShadowMap(std::vector<Object*> objs)
	{

	}*/
	//void ObjectManager::DrawObjects()
	//{
	//	for (auto pair : m_EntityToObjects) {
	//		Object* obj = pair.second;
	//		DrawObject(obj);
	//	}
	//}
	//void ObjectManager::DrawObject(Object * obj)
	//{
	//	if (obj != nullptr &&obj->HasComponent<MeshRenderer>()) {
	//		auto transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
	//		obj->GetComponent<MeshRenderer>()->UpdateTransformMatrix(transformMatrix);

	//		//灯光单独处理 //ParallelLight只有方向所以不需要Draw()
	//		if (obj->HasComponent<PointLight>()  || obj->HasComponent<SpotLight>()) {
	//			obj->GetComponent<MeshRenderer>()->DrawLight();
	//		}
	//		else {
	//			obj->GetComponent<MeshRenderer>()->DrawMeshes();
	//		//	obj->GetComponent<MeshRenderer>()->DrawModel();
	//		}

	//	}
	//}
	//void ObjectManager::DrawObjectsExcept(std::vector<Object *>objs)
	//{

	//	for (auto pair : m_EntityToObjects) {
	//		Object* Obj = pair.second;

	//		bool canDraw = true;
	//		for (auto obj : objs) {
	//			if (Obj->GetId().id == obj->GetId().id) {
	//				canDraw = false;
	//			}
	//		}
	//		if(canDraw)
	//			DrawObject(Obj);
	//			
	//	}
	//}
	//void ObjectManager::DrawObjectsExcept(Object * obj)
	//{
	//	for (auto pair : m_EntityToObjects) {
	//		Object* Obj = pair.second;		
	//		if (Obj->GetId().id != obj->GetId().id)
	//			DrawObject(Obj);
	//	}
	//}
	void ObjectManager::DestroyObjects()
	{
		for (auto obj : m_Objs) {
			if (obj != nullptr) {
				delete obj;
				obj = nullptr;
			}
		}
	}
}