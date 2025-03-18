#include "pch.h"
#include "MaterialShader.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"

namespace BlackPearl {
    extern ShaderFactory* g_shaderFactory;

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
            m_VertexShader = g_shaderFactory->CreateShaderFromSource(shaderSources[ShaderType::VertexShader], "main", ShaderType::VertexShader);
        } 
        if (shaderSources.find(ShaderType::Pixel) != shaderSources.end()) {
            m_PixelShader = g_shaderFactory->CreateShaderFromSource(shaderSources[ShaderType::Pixel], "main", ShaderType::Pixel);

        }
        if (shaderSources.find(ShaderType::Geometry) != shaderSources.end()) {
            m_GeometryShader = g_shaderFactory->CreateShaderFromSource(shaderSources[ShaderType::Geometry], "main", ShaderType::Geometry);

        }
        if (shaderSources.find(ShaderType::Compute) != shaderSources.end()) {
            m_ComputeShader = g_shaderFactory->CreateShaderFromSource(shaderSources[ShaderType::Compute], "main", ShaderType::Compute);

        }
	}
	std::string MaterialShader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in) {
			in.seekg(0, std::ios::end);
			int len = in.tellg();
			result.resize(len);
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else {
			GE_ASSERT("Could not open file '{0}'", filepath);
		}
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
        //add common struct source
        if (shaderSources.find(ShaderType::Pixel) != shaderSources.end()) {
            size_t pos = shaderSources[ShaderType::Pixel].find("#version", 0);//find找不到会返回npos
            GE_ASSERT(pos != std::string::npos, "Syntax error");

            size_t eol = shaderSources[ShaderType::Pixel].find_first_of("\r\n", pos);
            GE_ASSERT(eol != std::string::npos, "Syntax error");

            std::string front = shaderSources[ShaderType::Pixel].substr(pos, eol - pos + 1);
            std::string res = shaderSources[ShaderType::Pixel].substr(eol);
            shaderSources[ShaderType::Pixel] = front + commonSource + res;
        }
        return shaderSources;
	}
}