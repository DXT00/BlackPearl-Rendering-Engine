#pragma once
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/RHI/RHIShaderLibrary.h"
#include "BlackPearl/FileSystem/FileSystem.h"
#include <filesystem>
#include <string>
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
        IFileSystem* m_fs;
        std::filesystem::path m_basePath;

        

    public:
        ShaderFactory(
            DeviceHandle rendererInterface,
            IFileSystem* fs,
            const std::filesystem::path& basePath);

        void ClearCache();

       ShaderHandle CreateShader(const char* fileName, const char* entryName, ShaderType shaderType, const std::vector<ShaderMacro>* pDefines = nullptr);
       ShaderHandle CreateShaderFromSource(const std::string& srcCode, const char* entryName, ShaderType shaderType, const std::vector<ShaderMacro>* pDefines = nullptr, const std::string& filepath = "");

       ShaderHandle CreateShader(const char* fileName, const char* entryName,const ShaderDesc& desc, const std::vector<ShaderMacro>* pDefines = nullptr);
       ShaderLibraryHandle CreateShaderLibrary(const char* fileName, const std::vector<ShaderMacro>* pDefines = nullptr);

        std::shared_ptr<IBlob> GetBytecode(const char* fileName, const char* entryName);



    };

}
