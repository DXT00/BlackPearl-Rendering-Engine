#pragma once
#include "BlackPearl/Renderer/Mesh/Mesh.h"
namespace BlackPearl
{
    class ModelLoader
    {
    private:
        /* data */
    public:
        ModelLoader(/* args */);
        ~ModelLoader();
        virtual void Load(std::vector<std::shared_ptr<Mesh>>& output_meshes, BoundingSphere& bounding_sphere, const std::string& path) = 0;
    };
    

    
} // namespace BlackPearl
