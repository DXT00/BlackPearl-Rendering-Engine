#pragma once
#include "BlackPearl/Renderer/Model/ModelLoader.h"
#include <DirectXMath.h>

using namespace DirectX;
namespace BlackPearl {
    class D3D12ModelLoader : public ModelLoader
    {
    public:
        D3D12ModelLoader(bool isMeshletModel);
        ~D3D12ModelLoader();
        virtual void Load(std::vector<Mesh>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path);
        void LoadMeshletModel(std::vector<Mesh>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path);
        void LoadCommonModel(const std::string& path);

    private:
        /*for mesh shader renderer, to create meshlet*/
        bool m_IsMeshletModel = false;

    };
    

}