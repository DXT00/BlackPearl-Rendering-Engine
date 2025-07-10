#pragma once
#include "BlackPearl/Renderer/Mesh/Mesh.h"

namespace BlackPearl {
    class Loader
    {
    public:
        Loader() = default;
    protected:
        void CreateMeshBuffers(std::shared_ptr<Mesh>& mesh);

    };


}
