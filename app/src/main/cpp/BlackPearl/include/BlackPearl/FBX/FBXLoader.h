#pragma once
#include <string>
#include <vector>
#include <memory>
#include "RHI/RHIResources.h"
#include "Renderer/Model/Model.h"
#include "OpenFBX/include/ofbx.h"
#include "Renderer/Mesh/Mesh.h"
#include "Renderer/Model/Loader.h"
#include "Renderer/Shader/MaterialShader.h"
using namespace ofbx;

namespace BlackPearl {
	class MaterialShader;
	class DeviceManager;
    enum class Orientation
    {
        Y_UP,
        Z_UP,
        Z_MINUS_UP,
        X_MINUS_UP,
        X_UP
    };

	class FBXLoader :public Loader
	{
	public:
		FBXLoader();
		static constexpr char* EXTENSIONS[] = { "fbx" };
		Model* load(const std::string& fileName, const ModelDesc& desc);
	
		virtual void RegisterDeviceManager(DeviceManager* deviceManager);

	private:

        void loadMesh(Model* model, const std::string& fileName, const ofbx::IScene* scene, std::vector<const ofbx::Object*>& sceneBone, Orientation orientation, MaterialShader* shader);

        std::shared_ptr<Mesh> processMesh(Model* model, const ofbx::Mesh* fbxMesh, Orientation orientation);


		DeviceManager* m_DeviceManager;
		MaterialShader* m_FBXDefaultShader = nullptr;



		bool m_FirstVertex = true;
	
	};


}
