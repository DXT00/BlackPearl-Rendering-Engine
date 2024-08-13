#pragma once
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/RHI/RHIShaderLibrary.h"
#include "BlackPearl/FileSystem/FileSystem.h"
#include<filesystem>
#include "glm/glm.hpp"
namespace BlackPearl {

    // Specific blob implementation that owns the data and frees it when deleted.


    struct ShaderMacro
    {
        std::string name;
        std::string definition;

        ShaderMacro(const std::string& _name, const std::string& _definition)
            : name(_name)
            , definition(_definition)
        { }
    };

    class ShaderFactory
    {
    private:
        DeviceHandle m_Device;
        std::unordered_map<std::string, std::shared_ptr<IBlob>> m_BytecodeCache;
        std::shared_ptr<IFileSystem> m_fs;
        std::filesystem::path m_basePath;

    public:
        ShaderFactory(
            DeviceHandle rendererInterface,
            std::shared_ptr<IFileSystem> fs,
            const std::filesystem::path& basePath);

        void ClearCache();

       ShaderHandle CreateShader(const char* fileName, const char* entryName, const std::vector<ShaderMacro>* pDefines, ShaderType shaderType);
       ShaderHandle CreateShader(const char* fileName, const char* entryName, const std::vector<ShaderMacro>* pDefines, const ShaderDesc& desc);
       ShaderLibraryHandle CreateShaderLibrary(const char* fileName, const std::vector<ShaderMacro>* pDefines);

        std::shared_ptr<IBlob> GetBytecode(const char* fileName, const char* entryName);
    };

}
