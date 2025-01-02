#pragma once

#include <DirectXMath.h>
#include "BlackPearl/Renderer/Mesh/MeshletGenerator.h"
#include "BlackPearl/Renderer/Model/Model.h"
#include "BlackPearl/Renderer/Model/ModelLoader.h"
#include "BlackPearl/Component/BoundingSphereComponent/BoundingSphere.h"

using namespace DirectX;
namespace BlackPearl {

    class D3D12ModelLoader : public ModelLoader
    {
    public:
        D3D12ModelLoader();
        ~D3D12ModelLoader();
        virtual Model* LoadModel(const std::string& path, const ModelDesc& desc) override;
        void Load(std::vector<std::shared_ptr<Mesh>>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path);
        void LoadMeshletModel(std::vector<std::shared_ptr<Mesh>>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path);
        void LoadCommonModel(const std::string& path);

    private:
        /*for mesh shader renderer, to create meshlet*/
        bool m_IsMeshletModel = false;
        std::vector<uint8_t> m_Buffer;
        std::shared_ptr<MeshletGenerator> m_MeshletGenerator;

        BoundingSphere m_BoundingSphere;
    };
    

}