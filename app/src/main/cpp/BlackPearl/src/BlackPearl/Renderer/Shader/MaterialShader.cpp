#include "pch.h"
#include "Renderer/Shader/MaterialShader.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "Core/AssetManager.h"
#include "Renderer/Shader/GLSLIncluder.h"
namespace BlackPearl {
    extern ShaderFactory* g_shaderFactory;

    namespace fs = std::filesystem;
   
    static ShaderType ShaderTypeFromString(const std::string& type) {

        if (type == "vertex")
            return ShaderType::VertexShader;
        if (type == "fragment" || type == "pixel")
            return ShaderType::Pixel;
        if (type == "geometry")
            return ShaderType::Geometry;
        if (type == "compute")
            return ShaderType::Compute;
        if (type == "tessellation_control_shader")
            return ShaderType::TessellationControl;
        if (type == "tessellation_evaluation_shader")
            return ShaderType::TesselationEvaluation;
        //GE_ASSERT(false, "Unknown shader type!");
        return ShaderType::Invalid;
    }



	MaterialShader::MaterialShader(const std::string& filepath)
	{
		m_ShaderPath = filepath;
        std::string commonSource = ReadFile(m_CommonStructPath);
        m_GlslCode = ReadFile(m_ShaderPath);
        std::unordered_map<ShaderType, std::string> shaderSources = PreProcess(m_GlslCode, commonSource);
        if (shaderSources.find(ShaderType::VertexShader) != shaderSources.end()) {
            m_VertexShader = g_shaderFactory->CreateShaderFromSource(shaderSources[ShaderType::VertexShader], "main", ShaderType::VertexShader, nullptr, m_ShaderPath);
        } 
        if (shaderSources.find(ShaderType::Pixel) != shaderSources.end()) {
            m_PixelShader = g_shaderFactory->CreateShaderFromSource(shaderSources[ShaderType::Pixel], "main", ShaderType::Pixel, nullptr, m_ShaderPath);

        }
        if (shaderSources.find(ShaderType::Geometry) != shaderSources.end()) {
            m_GeometryShader = g_shaderFactory->CreateShaderFromSource(shaderSources[ShaderType::Geometry], "main", ShaderType::Geometry, nullptr, m_ShaderPath);

        }
        if (shaderSources.find(ShaderType::Compute) != shaderSources.end()) {
            m_ComputeShader = g_shaderFactory->CreateShaderFromSource(shaderSources[ShaderType::Compute], "main", ShaderType::Compute, nullptr, m_ShaderPath);

        }
	}
	std::string MaterialShader::ReadFile(const std::string& filepath)
	{
		std::string result;
        result = AssetManager::LoadGlslFile(filepath);
		return result;

     
 

	}

	std::unordered_map<ShaderType, std::string> MaterialShader::PreProcess(const std::string& source, const std::string& commonSource)
	{
        GE_ASSERT((!source.empty()), "shader source code is empty");
        std::unordered_map<ShaderType, std::string> shaderSources;
        const char* typeToken = "#type";
        size_t typeTockenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);//find�Ҳ����᷵��npos
        while (pos != std::string::npos) {
            size_t eol = source.find_first_of("\r\n", pos);
            GE_ASSERT(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTockenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            GE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos,
                pos - (nextLinePos == std::string::npos ?
                    source.size() - 1
                    : nextLinePos));//string::npos表示source的末尾位置

        }

        //todo::查找#include ,包含头文件
        std::unordered_set<std::string> includedFiles;

        GLSLIncluder includer({ 
            "assets/shaders/hlsl/core",
            "assets/shaders"
            ""
            });

        std::string fullShaderPS, fullShaderVS;
            // 直接处理字符串
        if (shaderSources.find(ShaderType::Pixel) != shaderSources.end()) {
            fullShaderPS = includer.processIncludes(shaderSources[ShaderType::Pixel]);
            shaderSources[ShaderType::Pixel] = fullShaderPS;
        }

        if (shaderSources.find(ShaderType::VertexShader) != shaderSources.end()) {
            fullShaderVS = includer.processIncludes(shaderSources[ShaderType::VertexShader]);
            shaderSources[ShaderType::VertexShader] = fullShaderVS;

        }


        //add common struct source
        if (shaderSources.find(ShaderType::Pixel) != shaderSources.end()) {
            size_t pos = shaderSources[ShaderType::Pixel].find("#version", 0);//find找不到会返回npos
            GE_ASSERT(pos != std::string::npos, "Syntax error");

            size_t eol = shaderSources[ShaderType::Pixel].find_first_of("\r\n", pos);
            GE_ASSERT(eol != std::string::npos, "Syntax error");

            std::string front = shaderSources[ShaderType::Pixel].substr(pos, eol - pos + 1);
            std::string res = shaderSources[ShaderType::Pixel].substr(eol);
#ifdef GE_PLATFORM_ANDROID
            //use gles 300
            front = "//"+front;// 注释掉 pc OpenGL version
            std::string glesVersion = "#version 310 es\r\n";
            // 添加 common structure
            shaderSources[ShaderType::Pixel] =  glesVersion+ commonSource + res;
#else
            shaderSources[ShaderType::Pixel] = front + commonSource + res;

#endif

        }
        //change to gles version if it is andriod platform
        if (shaderSources.find(ShaderType::VertexShader) != shaderSources.end()) {
            size_t pos = shaderSources[ShaderType::VertexShader].find("#version", 0);//find找不到会返回npos
            GE_ASSERT(pos != std::string::npos, "Syntax error");

            size_t eol = shaderSources[ShaderType::VertexShader].find_first_of("\r\n", pos);
            GE_ASSERT(eol != std::string::npos, "Syntax error");

            std::string front = shaderSources[ShaderType::VertexShader].substr(pos, eol - pos + 1);
            std::string res = shaderSources[ShaderType::VertexShader].substr(eol);
#ifdef GE_PLATFORM_ANDROID
            //use gles 300
            front = "//"+front;// 注释掉 pc OpenGL version
            std::string glesVersion = "#version 310 es\r\n";
            shaderSources[ShaderType::VertexShader] =  glesVersion +  res;
#endif

        }
        return shaderSources;
	}
}