#pragma once
#include <string>
#include <vector>
#include <memory>
#include "RHI/RHIResources.h"
#include "Renderer/Model/Model.h"

namespace BlackPearl {
	class MaterialShader;
	class DeviceManager;


	class FBXLoader
	{
	public:
		FBXLoader();
		static constexpr char* EXTENSIONS[] = { "fbx" };
		Model* load(const std::string& fileName, const ModelDesc& desc, const std::string& extension, std::vector<std::shared_ptr<IResource>>& out) const;
	
		virtual void RegisterDeviceManager(DeviceManager* deviceManager);

	protected:
		DeviceManager* m_DeviceManager;
		MaterialShader* m_FBXDefaultShader = nullptr;



		bool m_FirstVertex = true;
	
	};


}
