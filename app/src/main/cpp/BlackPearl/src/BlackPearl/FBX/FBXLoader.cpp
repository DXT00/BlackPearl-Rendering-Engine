#include "FBX/FBXLoader.h"

#include "glm/glm.hpp"
#include "glm/glm/fwd.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "Core.h"
#include "Core/AssetManager.h"
#include "Renderer/DeviceManager.h"
#include "Component/TransformComponent/Transform.h"
#include "Renderer/Mesh/Vertex.h"
#include "hlsl/core/slot_cb.h"
//#include "Component/BoundingBoxComponent/BoundingBox.h"

namespace BlackPearl {

#ifdef USE_FBX
    extern DeviceManager* g_deviceManager;



    namespace
    {
        static auto operator-(const ofbx::Vec3& a, const ofbx::Vec3& b) -> ofbx::Vec3
        {
            return { a.x - b.x, a.y - b.y, a.z - b.z };
        }

        static auto operator-(const ofbx::Vec2& a, const ofbx::Vec2& b) -> ofbx::Vec2
        {
            return { a.x - b.x, a.y - b.y };
        }

        inline auto getBonePath(const ofbx::Object* bone) -> std::string
        {
            if (bone == nullptr)
            {
                return "";
            }
            return getBonePath(bone->getParent()) + "/" + bone->name;
        }

        inline auto toGlm(const ofbx::Vec2& vec)
        {
            return glm::vec2(float(vec.x), float(vec.y));
        }

        inline auto toGlm(const ofbx::Vec3& vec)
        {
            return glm::vec3(float(vec.x), float(vec.y), float(vec.z));
        }

        inline auto toGlm(const ofbx::Vec4& vec)
        {
            return glm::vec4(float(vec.x), float(vec.y), float(vec.z), float(vec.w));
        }

        inline auto toGlm(const ofbx::Color& vec)
        {
            return glm::vec4(float(vec.r), float(vec.g), float(vec.b), 1.0f);
        }

        inline math::float3 fixOrientation(const math::float3& v, Orientation orientation)
        {
            switch (orientation)
            {
            case Orientation::Y_UP:
                return math::float3(v.x, v.y, v.z);
            case Orientation::Z_UP:
                return math::float3(v.x, v.z, -v.y);
            case Orientation::Z_MINUS_UP:
                return math::float3(v.x, -v.z, v.y);
            case Orientation::X_MINUS_UP:
                return math::float3(v.y, -v.x, v.z);
            case Orientation::X_UP:
                return math::float3(-v.y, v.x, v.z);
            }
            return v;
        }

        inline glm::quat fixOrientation(const glm::quat& v, Orientation orientation)
        {
            switch (orientation)
            {
            case Orientation::Y_UP:
                return { v.w, v.x, v.y, v.z };
            case Orientation::Z_UP:
                return { v.w, v.x, -v.y, v.z };
            case Orientation::Z_MINUS_UP:
                return { v.w, v.x, -v.z, v.y };
            case Orientation::X_MINUS_UP:
                return { v.w, -v.x, v.z, v.y };
            case Orientation::X_UP:
                return { -v.y, v.x, v.z, v.w };
            }
            return v;
        }

        inline auto computeTangents(ofbx::Vec3* out, int32_t vertexCount, const ofbx::Vec3* vertices, const ofbx::Vec3* normals, const ofbx::Vec2* uvs)
        {
            for (int i = 0; i < vertexCount; i += 3)
            {
                const auto& v0 = vertices[i + 0];
                const auto& v1 = vertices[i + 1];
                const auto& v2 = vertices[i + 2];
                const auto& uv0 = uvs[i + 0];
                const auto& uv1 = uvs[i + 1];
                const auto& uv2 = uvs[i + 2];

                const ofbx::Vec3 dv10 = v1 - v0;
                const ofbx::Vec3 dv20 = v2 - v0;
                const ofbx::Vec2 duv10 = uv1 - uv0;
                const ofbx::Vec2 duv20 = uv2 - uv0;

                const float dir = duv20.x * duv10.y - duv20.y * duv10.x < 0 ? -1.f : 1.f;
                ofbx::Vec3  tangent;
                tangent.x = (dv20.x * duv10.y - dv10.x * duv20.y) * dir;
                tangent.y = (dv20.y * duv10.y - dv10.y * duv20.y) * dir;
                tangent.z = (dv20.z * duv10.y - dv10.z * duv20.y) * dir;
                const float l = 1 / sqrtf(float(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z));
                tangent.x *= l;
                tangent.y *= l;
                tangent.z *= l;
                out[i + 0] = tangent;
                out[i + 1] = tangent;
                out[i + 2] = tangent;
            }
        }

        template <typename T>
        inline auto toMatrix(const ofbx::Matrix& mat)
        {
            T result;
            for (int32_t i = 0; i < 4; i++)
                for (int32_t j = 0; j < 4; j++)
                    result[i][j] = (float)mat.m[i * 4 + j];
            return result;
        }

        inline Transform getTransform(const ofbx::Object* object, Orientation orientation)
        {
            Transform transform;

            glm::mat4 localMatrix = glm::mat4(1);

            ofbx::Vec3           p = object->getLocalTranslation();
            glm::vec3 localPos = Math::ToVec3(fixOrientation({ static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z) }, orientation));
            ofbx::Vec3 r = object->getLocalRotation();
            glm::vec3 localOrientation = Math::ToVec3(fixOrientation(math::float3(static_cast<float>(r.x), static_cast<float>(r.y), static_cast<float>(r.z)), orientation));
            ofbx::Vec3 s = object->getLocalScaling();
            glm::vec3 localScale = glm::vec3(static_cast<float>(s.x), static_cast<float>(s.y), static_cast<float>(s.z));


            localMatrix = glm::translate(glm::mat4(1), localPos);
            localMatrix *= glm::toMat4(glm::quat(localOrientation));
            localMatrix = glm::scale(localMatrix, localScale);

            if (object->getParent())
            {
                transform.SetTransformMatrix(getTransform(object->getParent(), orientation).GetTransformMatrix() * localMatrix);
            }
            else
            {
                //transform.SetTransformMatrix(glm::mat4(1));
                transform.SetTransformMatrix(localMatrix);

            }
            return transform;
        }

        BlackPearl::TextureType getTextureType(ofbx::Texture::TextureType type) {
            switch (type)
            {
            case ofbx::Texture::DIFFUSE:
                return TextureType::DiffuseMap;
            case ofbx::Texture::NORMAL:
                return TextureType::NormalMap;
            case ofbx::Texture::SPECULAR:
                return TextureType::MentallicMap;
            case ofbx::Texture::SHININESS:
                return TextureType::RoughnessMap;
            case ofbx::Texture::AMBIENT:
                return TextureType::AoMap;
            case ofbx::Texture::EMISSIVE:
                return TextureType::EmissionMap;
            case ofbx::Texture::REFLECTION:
                //todo::
                return TextureType::MentallicMap;
            case ofbx::Texture::COUNT:
                return TextureType::None;
            default:
                break;
            }
            return TextureType::None;

        }


        inline TextureHandle loadTexture(const ofbx::Material* material, ofbx::Texture::TextureType type)
        {
            const ofbx::Texture* ofbxTexture = material->getTexture(type);
            BlackPearl::TextureType bpTextureType = getTextureType(type);
            TextureHandle texture2D;
            if (ofbxTexture)
            {
                ofbx::DataView filename = ofbxTexture->getRelativeFileName();
                if (filename == "")
                    filename = ofbxTexture->getFileName();

                char filePath[256];
                filename.toString(filePath);

                if (AssetManager::IsFileExist(filePath))
                {
                    texture2D = g_deviceManager->GetDevice()->createTexture(TextureDesc(bpTextureType, filePath));

                    //texture2D = device->createTexture(TextureDesc(TextureType::EmissionMap, "assets/models/sword/textures/Big Sword_Emission_Map.jpg"));

                    //texture2D = Texture2D::create(filePath, filePath);
                }
                else
                {
                    GE_CORE_ERROR("file {0} did not find", filePath);
                }
            }

            return texture2D;
        }

        inline auto loadMaterial(const ofbx::Material* material, MaterialShader* shader, bool animated)
        {
            std::shared_ptr<Material> meshMaterial;
            std::shared_ptr<Material::TextureMaps> textures(DBG_NEW Material::TextureMaps());

            MaterialColor  colors;



            //PBRMataterialTextures textures;
            //MaterialProperties    properties;

            colors.diffuseColor = Math::ToFloat4(toGlm(material->getDiffuseColor()));
            colors.specularColor = Math::ToFloat4(toGlm(material->getSpecularColor()));

            float roughness = 1.0f - std::sqrt(float(material->getShininess()) / 100.0f);
            colors.specularColor = Math::ToFloat4(toGlm(material->getSpecularColor()));


            textures->diffuseTextureMap = loadTexture(material, ofbx::Texture::TextureType::DIFFUSE);
            textures->normalTextureMap = loadTexture(material, ofbx::Texture::TextureType::NORMAL);
            textures->mentallicMap = loadTexture(material, ofbx::Texture::TextureType::SPECULAR);
            textures->roughnessMap = loadTexture(material, ofbx::Texture::TextureType::SHININESS);
            textures->emissionTextureMap = loadTexture(material, ofbx::Texture::TextureType::EMISSIVE);
            textures->aoMap = loadTexture(material, ofbx::Texture::TextureType::AMBIENT);

            //if (!textures.albedo)
            //	properties.usingAlbedoMap = 0.0f;
            //if (!textures.normal)
            //	properties.usingNormalMap = 0.0f;
            //if (!textures.metallic)
            //	properties.usingMetallicMap = 0.0f;
            //if (!textures.roughness)
            //	properties.usingRoughnessMap = 0.0f;
            //if (!textures.emissive)
            //	properties.usingEmissiveMap = 0.0f;
            //if (!textures.ao)
            //	properties.usingAOMap = 0.0f;

            //pbrMaterial->setTextures(textures);
            //pbrMaterial->setMaterialProperites(properties);


            meshMaterial.reset(DBG_NEW Material(shader, textures, colors));
            meshMaterial->SetRoughness(roughness);


            return meshMaterial;
        }

        inline auto getOffsetMatrix(const ofbx::Mesh* mesh, const ofbx::Object* node) -> glm::mat4
        {
            auto* skin = mesh ? mesh->getGeometry()->getSkin() : nullptr;
            if (skin)
            {
                for (int i = 0, c = skin->getClusterCount(); i < c; i++)
                {
                    const ofbx::Cluster* cluster = skin->getCluster(i);
                    if (cluster->getLink() == node)
                    {
                        return toMatrix<glm::mat4>(cluster->getTransformLinkMatrix());
                    }
                }
            }
            return toMatrix<glm::mat4>(node->getGlobalTransform());
        }

        inline ofbx::Vec3 operator*(const ofbx::Vec3& vec3, float v)
        {
            return { vec3.x * v, vec3.y * v, vec3.z * v };
        }
    }
    //void generateVertex(const std::vector<Vertex>& Vertices) {
    //{                                                                                                                                                        \
		//


        //}

    void updateAABB(Model* model, const math::float3& pos, bool firstVertex)
    {
        math::float3 min;
        math::float3 max;
        if (firstVertex) {
            firstVertex = false;
            min = pos;
            max = pos;
            return;
        }
        min = model->boundingbox->GetMinP();
        max = model->boundingbox->GetMaxP();

        if (pos.x < min.x) {
            min.x = pos.x;
        }
        if (pos.y < min.y) {
            min.y = pos.y;
        }
        if (pos.z < min.z) {
            min.z = pos.z;
        }
        if (pos.x > max.x) {
            max.x = pos.x;
        }
        if (pos.y > max.y) {
            max.y = pos.y;
        }
        if (pos.z > max.z) {
            max.z = pos.z;
        }

        model->boundingbox->SetP(min, max);
    }


    std::shared_ptr<Mesh> FBXLoader::processMesh(Model* model, const ofbx::Mesh* fbxMesh, Orientation orientation) {

        //const auto fbxMesh = (const ofbx::Mesh*)scene->getMesh(i);
        const auto geom = fbxMesh->getGeometry();
        const auto numIndices = geom->getIndexCount();
        const int vertexCount = geom->getVertexCount();
        const auto vertices = geom->getVertices();
        const auto normals = geom->getNormals();
        const auto tangents = geom->getTangents();
        const auto colors = geom->getColors();
        const auto uvs = geom->getUVs();
        const auto materials = geom->getMaterials();

        if (numIndices == 0)
            return nullptr;


        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        std::shared_ptr<BufferGroup>& buffers = mesh->buffers;

        std::vector<float3>& positionData = buffers->positionData;
        std::vector<float3>& normalData = buffers->normalData;
        std::vector<float3>& tangentData = buffers->tangentData;
        std::vector<float3>& bitangentData = buffers->bitangentData;
        std::vector<float2>& texcoordData = buffers->texcoordData;

        std::vector<uint32_t>& indicesData = buffers->indexData;
        std::vector<uint32_t>& jointIdData = buffers->jointIdData;
        std::vector<uint32_t>& jointIdData1 = buffers->jointId1Data;
        std::vector<uint32_t>& jointIdData2 = buffers->jointId2Data;

        std::vector<float> jointWeightData = buffers->jointWeightData;
        std::vector<float> jointWeightData1 = buffers->jointWeight1Data;
        std::vector<float> jointWeightData2 = buffers->jointWeight2Data;


        bool hasAnimation = model->desc.bIsAnimated;


        VertexBufferLayout layout = {
            {ElementDataType::Float3,"aPos",false,Slot_aPos},
            {ElementDataType::Float3,"aNormal",false,Slot_aNormal},
            {ElementDataType::Float2,"aTexCoords",false,Slot_aTexCoords}
        };


        if (tangents) {
            layout.AddElement({ ElementDataType::Float3,"aTangent",false,Slot_aTangent });
           // layout.AddElement({ ElementDataType::Float3,"aBitangent",false,4 });
        }
        if (hasAnimation) {
            layout.AddElement({ ElementDataType::Int4,"aJointIndices",false,Slot_aJointIndices });
            //layout.AddElement({ ElementDataType::Int4,"aJointIndices1",false,5 });
            //layout.AddElement({ ElementDataType::Int4,"aJointIndices2",false,7 });
            layout.AddElement({ ElementDataType::Int4,"aWeights",false,Slot_aJointWeights });
            //layout.AddElement({ ElementDataType::Int4,"aWeights1",false,9 });
            //layout.AddElement({ ElementDataType::Int4,"aWeight2",false,10 });

        }









        std::vector<std::shared_ptr<Material>> pbrMaterialsCache;

        std::vector<Vertex> tempVertices;

        tempVertices.resize(vertexCount);

        /* std::vector<uint32_t> indicesArray;

         indicesArray.resize(numIndices);*/

        //auto boundingBox = std::make_shared<BoundingBox>();

        const auto indices = geom->getFaceIndices();

        ofbx::Vec3* generatedTangents = nullptr;
        if (!tangents && normals && uvs)
        {
            generatedTangents = new ofbx::Vec3[vertexCount];
            computeTangents(generatedTangents, vertexCount, vertices, normals, uvs);
        }





        Transform transform = getTransform(fbxMesh, orientation);
        bool skin = false;

        //transform.setLocalScale({ FBXToGameUnit,
        //						 FBXToGameUnit,
        //						 FBXToGameUnit });
        //transform.setWorldMatrix(glm::mat4(1.f));

        for (int32_t i = 0; i < vertexCount; ++i)
        {
            const ofbx::Vec3& cp = vertices[i];


            Vertex& vertex = tempVertices[i];
            glm::vec4 tmpPos = glm::vec4(transform.GetTransformMatrix() * glm::vec4(float(cp.x), float(cp.y), float(cp.z), 1.0));
            vertex.position = float3(tmpPos.x, tmpPos.y, tmpPos.z);
            vertex.position = fixOrientation(vertex.position, orientation);
            positionData.push_back(vertex.position);

            updateAABB(model, vertex.position, m_FirstVertex);


            if (m_FirstVertex)
                m_FirstVertex = false;

            if (normals)
            {
                glm::mat3 matrix(transform.GetTransformMatrix());
                vertex.normal = Math::ToFloat3(glm::transpose(glm::inverse(matrix)) * glm::normalize(glm::vec3{ float(normals[i].x), float(normals[i].y), float(normals[i].z) }));
                vertex.normal = fixOrientation(vertex.normal, orientation);
                normalData.push_back(vertex.normal);

            }
            if (uvs) {
                vertex.texCoords = { float(uvs[i].x), float(uvs[i].y) };
                texcoordData.push_back(vertex.texCoords);
            }
            /*	if (colors)
                    vertex.color = { float(colors[i].x), float(colors[i].y), float(colors[i].z), float(colors[i].w) };
                else
                    vertex.color = { 1, 1, 1, 1 }; */
            if (tangents) {
                glm::vec4 tmpTangent = transform.GetTransformMatrix() * glm::vec4(float(tangents[i].x), float(tangents[i].y), float(tangents[i].z), 1.0);
                vertex.tangent = Math::ToFloat3(glm::vec3(tmpTangent.x, tmpTangent.y, tmpTangent.z));
                vertex.tangent = fixOrientation(vertex.tangent, orientation);
                tangentData.push_back(vertex.tangent);

            }

            if (tangents && normals) {

                vertex.bitTangent = math::cross(math::normalize(vertex.tangent), math::normalize(vertex.normal));
                vertex.bitTangent = math::normalize(vertex.bitTangent);
                bitangentData.push_back(vertex.bitTangent);
            }


        }

        for (int32_t i = 0; i < numIndices; i++)
        {
            int32_t index = (i % 3 == 2) ? (-indices[i] - 1) : indices[i];
            indicesData.push_back(index);
        }

        //std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(indicesArray, tempVertices);

        for (auto i = 0; i < fbxMesh->getMaterialCount(); i++)
        {
            const ofbx::Material* material = fbxMesh->getMaterial(i);
            pbrMaterialsCache.emplace_back(loadMaterial(material, m_FBXDefaultShader, false));
        }

        std::vector<std::shared_ptr<Material>> pbrMaterials;


        const auto trianglesCount = vertexCount / 3;

        std::vector<uint32_t> subMeshIdx;

        if (fbxMesh->getMaterialCount() > 1)
        {
            int32_t rangeStart = 0;
            int32_t rangeStartVal = materials[rangeStart];
            pbrMaterials.emplace_back(pbrMaterialsCache[rangeStartVal]);

            for (int32_t triangleIndex = 1; triangleIndex < trianglesCount; triangleIndex++)
            {
                if (rangeStartVal != materials[triangleIndex])
                {
                    rangeStartVal = materials[triangleIndex];
                    subMeshIdx.emplace_back(triangleIndex);
                    pbrMaterials.emplace_back(pbrMaterialsCache[rangeStartVal]);
                }
            }

            subMeshIdx.emplace_back(trianglesCount);

            //  mesh->setSubMeshIndex(subMeshIdx);
            //  mesh->setSubMeshCount(subMeshIdx.size());

            GE_ASSERT(subMeshIdx.size() == pbrMaterials.size(), "size is not same");
        }
        else
        {
            pbrMaterials = pbrMaterialsCache;
        }
        //TODO:: 要考虑多个submesh和 material的情况
        mesh->material = pbrMaterials[0];

        // mesh->setMaterial(pbrMaterials);

        std::string name = fbxMesh->name;

        GE_ASSERT(name != "", "name should not be null");

        mesh->name = name;
        // meshes->addMesh(name, mesh);

        if (generatedTangents)
            delete[] generatedTangents;


        mesh->m_IndicesCount = indicesData.size();
        mesh->m_IndicesSize = mesh->m_IndicesCount * sizeof(uint32_t);
        mesh->m_VertexBufferLayout = layout;
        //buffers->vertexBufferLayout = layout;

        //   mesh->material = m_CurentModelMaterials[aimesh->mMaterialIndex];

        CreateMeshBuffers(mesh);
        return mesh;
    }

    void FBXLoader::loadMesh(Model* model, const std::string& fileName, const ofbx::IScene* scene, std::vector<const ofbx::Object*>& sceneBone, Orientation orientation, MaterialShader* shader)
    {


        m_FirstVertex = true;

        if (scene->getMeshCount() > 0)
        {




            //auto meshes = std::make_shared<MeshResource>(fileName);
            //outRes.emplace_back(meshes);

            constexpr float FBXToGameUnit = 0.01f;

            for (int32_t i = 0; i < scene->getMeshCount(); ++i)
            {
                const auto& mesh = processMesh(model, (const ofbx::Mesh*)scene->getMesh(i), orientation);
                if (mesh) {
                    model->meshes.push_back(mesh);

                }
            }
        }
    }



    void FBXLoader::RegisterDeviceManager(DeviceManager* deviceManager)
    {
        m_DeviceManager = deviceManager;
    }


    FBXLoader::FBXLoader()
    {

    }

    Model* FBXLoader::load(const std::string& fileName, const ModelDesc& desc)
    {
        if(!m_FBXDefaultShader)
            m_FBXDefaultShader = DBG_NEW MaterialShader("assets/shaders/glsl/forward_shading/forward_shading.glsl");

        if (!AssetManager::IsFileExist(fileName)) {
            GE_ASSERT(0, "open fbx file failed");

        }

        Model* model = DBG_NEW Model(fileName, desc);

        std::vector<uint8_t> fbxData;
        AssetManager::LoadBuffer(fileName, fbxData);
        //		mio::mmap_source mmap(fileName);

        constexpr bool ignoreGeometry = false;
        const uint64_t flags = ignoreGeometry ? (uint64_t)ofbx::LoadFlags::IGNORE_GEOMETRY : (uint64_t)ofbx::LoadFlags::TRIANGULATE;
        auto           scene = ofbx::load(fbxData.data(), fbxData.size(), flags);

        const ofbx::GlobalSettings* settings = scene->getGlobalSettings();

        Orientation orientation = Orientation::Y_UP;

        switch (settings->UpAxis)
        {
        case ofbx::UpVector_AxisX:
            orientation = Orientation::X_UP;
            break;
        case ofbx::UpVector_AxisY:
            orientation = Orientation::Y_UP;
            break;
        case ofbx::UpVector_AxisZ:
            orientation = Orientation::Z_UP;
            break;
        }
        std::vector<const ofbx::Object*> sceneBone;

        loadMesh(model, fileName, scene, sceneBone, orientation, m_FBXDefaultShader);

        return model;
    }
#endif
}