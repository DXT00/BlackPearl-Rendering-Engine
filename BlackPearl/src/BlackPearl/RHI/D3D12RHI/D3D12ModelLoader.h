#include "pch.h"
#include "BlackPearl/Renderer/Model/ModelLoader.h"
#include <DirectXMesh.h>

using namespace DirectX;
namespace BlackPearl {
    class D3D12ModelLoader : public ModelLoader
    {
    public:
        D3D12ModelLoader(/* args */);
        ~D3D12ModelLoader();
        virtual void Load(const std::string& path);
    private:

    };
    

}