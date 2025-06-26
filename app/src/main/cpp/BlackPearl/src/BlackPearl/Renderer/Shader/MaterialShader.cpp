#include "pch.h"
#include "Renderer/Shader/MaterialShader.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "Core/AssetManager.h"
#include "Renderer/Shader/GLSLIncluder.h"
#include "Core/AssetManager.h"
#include "hlsl/core/slot_cb.h"
namespace BlackPearl {
    extern ShaderFactory* g_shaderFactory;

    namespace fs = std::filesystem;
    static std::string get_filename(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        if (pos != std::string::npos) {
            return path.substr(pos + 1);
        }
        return path;
    }

    //android 平台 store后要跑 GetShaderFromAndroid.bat
    static void StoreShader(const std::string& shaderCode, const std::string& name) {
        if(!shaderCode.empty())
            AssetManager::StoreGLSLShader(shaderCode, name);

    }
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
        
        GE_CORE_ERROR("Unknown shader type!");
        return ShaderType::Invalid;
    }

    std::string ReadExtentions(std::vector<std::string>* extensions){
        std::string ret;
        if(!extensions)
            return ret;
        for (int i = 0; i <  (*extensions).size(); ++i) {
            ret+= (*extensions)[i];
            ret+="\r\n";
        }
        GE_CORE_INFO("add extentions %s",ret.c_str());
        return ret;
    }

    std::string ReadMacros(std::vector<std::string>* macros){
        std::string ret;
        if(!macros)
            return ret;
        for (int i = 0; i <  (*macros).size(); ++i) {
            ret+= (*macros)[i];
            ret+="\r\n";
        }
        GE_CORE_INFO("add macros %s",ret.c_str());
        return ret;
    }


    MaterialShader::MaterialShader(const std::string& filepath, std::vector<std::string>* extensions, std::vector<std::string>* macros)
	{
        m_GlslIncluder = GLSLIncluder({
            "assets/shaders/hlsl/core",
            "assets/shaders",
            ""
            });

		m_ShaderPath = filepath;
        std::string extentions = ReadExtentions(extensions);
        std::string commonSource = ReadFile(m_CommonStructPath);// m_GlslIncluder.processIncludes(ReadFile(m_CommonStructPath));
        std::string macroSource = "\r\n" + ReadMacros(macros) + "\r\n" + ReadFile(m_MacroPath);
        commonSource = extentions + macroSource + commonSource;
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

    static void CompileInputLocation(std::string& source) {

        std::unordered_map<std::string, int> slotMap;

        slotMap["Slot_aPos"] = Slot_aPos;
        slotMap["Slot_aPrePos"] = Slot_aPrePos;
        slotMap["Slot_aTexCoords"] = Slot_aTexCoords;
        slotMap["Slot_aNormal"] = Slot_aNormal;
        slotMap["Slot_aTangent"] = Slot_aTangent;;
        slotMap["Slot_aJointIndices"] = Slot_aJointIndices;
        slotMap["Slot_aJointWeights"] = Slot_aJointWeights;
        slotMap["Slot_aTexCoords1"] = Slot_aTexCoords1;
        slotMap["Slot_aTransform "] = Slot_aTransform;
        slotMap["Slot_aPrevTransform"] = Slot_aPrevTransform;


        auto it = slotMap.begin();
        for (; it != slotMap.end(); it++)
        {

            size_t pos = source.find(it->first);
            while (pos != std::string::npos) {
                source.replace(pos, it->first.length(), std::to_string(it->second));
                pos = source.find(it->first, pos + std::to_string(it->second).length());
            }


        }


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
            GE_ASSERT(ShaderTypeFromString(type) != ShaderType::Invalid, "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos,
                pos - (nextLinePos == std::string::npos ?
                    source.size() - 1
                    : nextLinePos));//string::npos表示source的末尾位置

        }
        //todo::查找#include ,包含头文件
        std::unordered_set<std::string> includedFiles;
        
        std::string fullShaderPS, fullShaderVS;
            // 直接处理字符串

        if (shaderSources.find(ShaderType::VertexShader) != shaderSources.end()) {
            m_GlslIncluder.reset();
            fullShaderVS = m_GlslIncluder.processIncludes(shaderSources[ShaderType::VertexShader]);
            shaderSources[ShaderType::VertexShader] = fullShaderVS;

        }

        if (shaderSources.find(ShaderType::Pixel) != shaderSources.end()) {
            m_GlslIncluder.reset();
            fullShaderPS = m_GlslIncluder.processIncludes(shaderSources[ShaderType::Pixel]);
            shaderSources[ShaderType::Pixel] = fullShaderPS;
        }

        if (shaderSources.find(ShaderType::Compute) != shaderSources.end()) {
            m_GlslIncluder.reset();
            fullShaderPS = m_GlslIncluder.processIncludes(shaderSources[ShaderType::Compute]);
            shaderSources[ShaderType::Compute] = fullShaderPS;
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
            shaderSources[ShaderType::VertexShader] =  glesVersion + commonSource + res;
#else
            shaderSources[ShaderType::VertexShader] = front + commonSource + res;

#endif
            CompileInputLocation(shaderSources[ShaderType::VertexShader]);
            //attribute 字符穿替换


        }

        //change to gles version if it is andriod platform
        if (shaderSources.find(ShaderType::Compute) != shaderSources.end()) {
            size_t pos = shaderSources[ShaderType::Compute].find("#version", 0);//find找不到会返回npos
            GE_ASSERT(pos != std::string::npos, "Syntax error");

            size_t eol = shaderSources[ShaderType::Compute].find_first_of("\r\n", pos);
            GE_ASSERT(eol != std::string::npos, "Syntax error");

            std::string front = shaderSources[ShaderType::Compute].substr(pos, eol - pos + 1);
            std::string res = shaderSources[ShaderType::Compute].substr(eol);
#ifdef GE_PLATFORM_ANDROID
            //use gles 300
            front = "//" + front;// 注释掉 pc OpenGL version
            std::string glesVersion = "#version 310 es\r\n";
            shaderSources[ShaderType::Compute] = glesVersion + commonSource + res;
#else
            shaderSources[ShaderType::Compute] = front + commonSource + res;

#endif
  

        }
        StoreShader(shaderSources[ShaderType::VertexShader], get_filename(m_ShaderPath)+"_vert");
        StoreShader(shaderSources[ShaderType::Pixel], get_filename(m_ShaderPath) + "_frag");
        StoreShader(shaderSources[ShaderType::Compute], get_filename(m_ShaderPath) + "_comp");

//#ifdef GE_PLATFORM_WINDOWS
//        GE_CORE_INFO("Shader {0}---------------\n, ---------vertex---------\n {1}\n, -----------pixel------------\n {2} \n", m_ShaderPath.c_str(), shaderSources[ShaderType::VertexShader].c_str(), shaderSources[ShaderType::Pixel].c_str());
//#elif defined(GE_PLATFORM_ANDROID)
//        GE_CORE_INFO("Shader %s---------------\n, ---------------vertex---------------: %s\n, ---------------pixel---------------: %s \n", m_ShaderPath.c_str(), shaderSources[ShaderType::VertexShader].c_str(), shaderSources[ShaderType::Pixel].c_str());
//#endif
        return shaderSources;
	}
}