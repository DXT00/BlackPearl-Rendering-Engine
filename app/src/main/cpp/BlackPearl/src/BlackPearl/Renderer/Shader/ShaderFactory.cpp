#include "pch.h"
#include <string>
#include "ShaderFactory.h"
#include "BlackPearl/RHI/Common/shader-blob.h"
#include "BlackPearl/Core.h"

namespace BlackPearl {
	ShaderFactory::ShaderFactory(DeviceHandle rendererInterface, IFileSystem* fs, const std::filesystem::path& basePath)
	{
		//m_basePath  = assets/shaders
		m_Device = rendererInterface;
		m_fs = fs;
		m_basePath = basePath;
	}
	void ShaderFactory::ClearCache()
	{
	}
	ShaderHandle ShaderFactory::CreateShader(const char* fileName, const char* entryName, const std::vector<ShaderMacro>* pDefines, ShaderType shaderType)
	{
		ShaderDesc desc = ShaderDesc(shaderType);
		desc.debugName = fileName;
		return CreateShader(fileName, entryName, pDefines, desc);
	}
	ShaderHandle ShaderFactory::CreateShader(const char* fileName, const char* entryName, const std::vector<ShaderMacro>* pDefines, const ShaderDesc& desc)
	{
		std::shared_ptr<IBlob> byteCode = GetBytecode(fileName, entryName);

		if (!byteCode) {
			return m_Device->createShader(desc, nullptr, 0);
		}

		std::vector<ShaderMake::ShaderConstant> constants;
		if (pDefines)
		{
			for (const ShaderMacro& define : *pDefines)
				constants.push_back(ShaderMake::ShaderConstant{ define.name.c_str(), define.definition.c_str() });
		}

		ShaderDesc descCopy = desc;
		descCopy.entryName = entryName;

		const void* permutationBytecode = nullptr;
		size_t permutationSize = 0;
		if (!ShaderMake::findPermutationInBlob(byteCode->data(), byteCode->size(), constants.data(), uint32_t(constants.size()), &permutationBytecode, &permutationSize))
		{
			const std::string message = ShaderMake::formatShaderNotFoundMessage(byteCode->data(), byteCode->size(), constants.data(), uint32_t(constants.size()));
			GE_CORE_ERROR("%s", message.c_str());

			return nullptr;
		}

		return m_Device->createShader(descCopy, permutationBytecode, permutationSize);
	}
	ShaderLibraryHandle ShaderFactory::CreateShaderLibrary(const char* fileName, const std::vector<ShaderMacro>* pDefines)
	{
		std::shared_ptr<IBlob> byteCode = GetBytecode(fileName, nullptr);

		if (!byteCode)
			return nullptr;

		std::vector<ShaderMake::ShaderConstant> constants;
		if (pDefines)
		{
			for (const ShaderMacro& define : *pDefines)
				constants.push_back(ShaderMake::ShaderConstant{ define.name.c_str(), define.definition.c_str() });
		}

		const void* permutationBytecode = nullptr;
		size_t permutationSize = 0;
		if (!ShaderMake::findPermutationInBlob(byteCode->data(), byteCode->size(), constants.data(), uint32_t(constants.size()), &permutationBytecode, &permutationSize))
		{
			const std::string message = ShaderMake::formatShaderNotFoundMessage(byteCode->data(), byteCode->size(), constants.data(), uint32_t(constants.size()));
			GE_CORE_ERROR(message.c_str());

			return nullptr;
		}

		return m_Device->createShaderLibrary(permutationBytecode, permutationSize);
	}
	std::shared_ptr<IBlob> ShaderFactory::GetBytecode(const char* fileName, const char* entryName)
	{
		if (!entryName)
			entryName = "main";

		std::string adjustedName = fileName;
		{
			size_t pos = adjustedName.find(".hlsl");
			if (pos != std::string::npos)
				adjustedName.erase(pos, 5);

			if (entryName && strcmp(entryName, "main"))
				adjustedName += "_" + std::string(entryName);
		}

		std::filesystem::path shaderFilePath = m_basePath /(adjustedName + ".bin");

		std::shared_ptr<IBlob>& data = m_BytecodeCache[shaderFilePath.generic_string()];

		if (data)
			return data;

		data = m_fs->readFile(shaderFilePath);

		if (!data)
		{
			GE_CORE_ERROR("Couldn't read the binary file for shader %s from %s", fileName, shaderFilePath.generic_string().c_str());
			return nullptr;
		}

		return data;
	}

}